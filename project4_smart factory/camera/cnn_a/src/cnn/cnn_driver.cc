#include "cnn_driver.h"

#include "cnn_weights.h"
#include "xcnn_top.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xparameters.h"

#ifndef XST_FAILURE
#define XST_FAILURE 1
#endif

#define CNN_BYTES_PER_PIXEL 3U
#define CNN_DONE_TIMEOUT_POLLS 300000000U

static XCnn_top g_cnn_ip;
static int g_cnn_ready = 0;
static CnnCrop g_crop = {0U, 0U, 0U, 0U};
static s8 g_cnn_input[CNN_INPUT_SIZE] __attribute__((aligned(64)));

static void normalize_crop(CnnCrop* crop, u32 frame_width, u32 frame_height)
{
	if (frame_width == 0U || frame_height == 0U) {
		crop->x = 0U;
		crop->y = 0U;
		crop->width = 0U;
		crop->height = 0U;
		return;
	}

	if (crop->width == 0U || crop->height == 0U) {
		crop->x = 0U;
		crop->y = 0U;
		crop->width = frame_width;
		crop->height = frame_height;
	}

	if (crop->x >= frame_width) {
		crop->x = frame_width - 1U;
	}
	if (crop->y >= frame_height) {
		crop->y = frame_height - 1U;
	}
	if (crop->width > frame_width - crop->x) {
		crop->width = frame_width - crop->x;
	}
	if (crop->height > frame_height - crop->y) {
		crop->height = frame_height - crop->y;
	}
	if (crop->width == 0U) {
		crop->width = 1U;
	}
	if (crop->height == 0U) {
		crop->height = 1U;
	}
}

static s8 average_rgb_area_to_int8(const u8* frame,
		u32 stride_bytes,
		u32 x0,
		u32 y0,
		u32 x1,
		u32 y1,
		CnnPixelOrder pixel_order)
{
	u32 r_sum = 0U;
	u32 g_sum = 0U;
	u32 b_sum = 0U;
	u32 count = 0U;

	for (u32 y = y0; y < y1; ++y) {
		const u8* row = frame + y * stride_bytes;
		for (u32 x = x0; x < x1; ++x) {
			const u8* pix = row + x * CNN_BYTES_PER_PIXEL;
			u8 r;
			u8 g = pix[1];
			u8 b;

			if (pixel_order == CNN_PIXEL_BGR888) {
				b = pix[0];
				r = pix[2];
			} else {
				r = pix[0];
				b = pix[2];
			}

			r_sum += r;
			g_sum += g;
			b_sum += b;
			++count;
		}
	}

	if (count == 0U) {
		return 0;
	}

	u32 r_avg = r_sum / count;
	u32 g_avg = g_sum / count;
	u32 b_avg = b_sum / count;
	u32 gray = (77U * r_avg + 150U * g_avg + 29U * b_avg) >> 8;
	int centered = (int)gray - 128;
	int abs_centered = (centered < 0) ? -centered : centered;
	int q_abs = (abs_centered * 127 + 64) / 128;
	int q = (centered < 0) ? -q_abs : q_abs;

	if (q < -127) {
		q = -127;
	}
	if (q > 127) {
		q = 127;
	}

	return (s8)q;
}

static void make_cnn_input(const u8* frame,
		u32 stride_bytes,
		CnnCrop crop,
		CnnPixelOrder pixel_order)
{
	const u32 crop_x_end = crop.x + crop.width;
	const u32 crop_y_end = crop.y + crop.height;

	for (u32 oy = 0U; oy < CNN_INPUT_HEIGHT; ++oy) {
		u32 y0 = crop.y + (oy * crop.height) / CNN_INPUT_HEIGHT;
		u32 y1 = crop.y + ((oy + 1U) * crop.height) / CNN_INPUT_HEIGHT;
		if (y1 <= y0) {
			y1 = y0 + 1U;
		}
		if (y1 > crop_y_end) {
			y1 = crop_y_end;
		}

		for (u32 ox = 0U; ox < CNN_INPUT_WIDTH; ++ox) {
			u32 x0 = crop.x + (ox * crop.width) / CNN_INPUT_WIDTH;
			u32 x1 = crop.x + ((ox + 1U) * crop.width) / CNN_INPUT_WIDTH;
			if (x1 <= x0) {
				x1 = x0 + 1U;
			}
			if (x1 > crop_x_end) {
				x1 = crop_x_end;
			}

			g_cnn_input[oy * CNN_INPUT_WIDTH + ox] =
					average_rgb_area_to_int8(frame, stride_bytes, x0, y0, x1, y1, pixel_order);
		}
	}
}

static int run_cnn_ip_from_internal_input(int* out_class)
{
	if (out_class == 0) {
		return XST_FAILURE;
	}

	Xil_DCacheFlushRange((UINTPTR)g_cnn_input, CNN_INPUT_SIZE);
	Xil_DCacheFlushRange((UINTPTR)g_cnn_weights, CNN_WEIGHTS_SIZE);

	XCnn_top_Set_input_r(&g_cnn_ip, (u64)(UINTPTR)g_cnn_input);
	XCnn_top_Set_weights(&g_cnn_ip, (u64)(UINTPTR)g_cnn_weights);
	XCnn_top_Start(&g_cnn_ip);

	u32 timeout = CNN_DONE_TIMEOUT_POLLS;
	while (!XCnn_top_IsDone(&g_cnn_ip) && timeout != 0U) {
		--timeout;
	}

	if (timeout == 0U) {
		xil_printf("CNN IP timeout.\r\n");
		return XST_FAILURE;
	}

	*out_class = (int)XCnn_top_Get_result(&g_cnn_ip);
	return XST_SUCCESS;
}

int cnn_init(void)
{
	if (g_cnn_ready) {
		return XST_SUCCESS;
	}

	int status = XCnn_top_Initialize(&g_cnn_ip, XPAR_CNN_TOP_0_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("CNN IP init failed: %d\r\n", status);
		return status;
	}

	Xil_DCacheFlushRange((UINTPTR)g_cnn_weights, CNN_WEIGHTS_SIZE);
	g_cnn_ready = 1;

	return XST_SUCCESS;
}

int cnn_set_crop(CnnCrop crop, u32 frame_width, u32 frame_height)
{
	if (frame_width == 0U || frame_height == 0U) {
		return XST_FAILURE;
	}

	normalize_crop(&crop, frame_width, frame_height);
	g_crop = crop;

	return XST_SUCCESS;
}

int cnn_set_configured_crop(u32 frame_width, u32 frame_height)
{
	CnnCrop crop = {CNN_ROI_X, CNN_ROI_Y, CNN_ROI_WIDTH, CNN_ROI_HEIGHT};
	return cnn_set_crop(crop, frame_width, frame_height);
}

CnnCrop cnn_get_crop(void)
{
	return g_crop;
}

int cnn_run_from_frame(UINTPTR frame_addr,
		u32 frame_width,
		u32 frame_height,
		u32 frame_stride_bytes,
		CnnPixelOrder pixel_order,
		int* out_class)
{
	if (out_class == 0 || frame_addr == 0U || frame_width == 0U || frame_height == 0U) {
		return XST_FAILURE;
	}
	if (frame_stride_bytes < frame_width * CNN_BYTES_PER_PIXEL) {
		return XST_FAILURE;
	}

	int status = cnn_init();
	if (status != XST_SUCCESS) {
		return status;
	}

	CnnCrop crop = g_crop;
	normalize_crop(&crop, frame_width, frame_height);
	g_crop = crop;

	Xil_DCacheInvalidateRange(frame_addr, frame_stride_bytes * frame_height);
	make_cnn_input((const u8*)frame_addr, frame_stride_bytes, crop, pixel_order);

	return run_cnn_ip_from_internal_input(out_class);
}

int cnn_run_from_input(const s8* input_40x80, int* out_class)
{
	if (input_40x80 == 0 || out_class == 0) {
		return XST_FAILURE;
	}

	int status = cnn_init();
	if (status != XST_SUCCESS) {
		return status;
	}

	for (u32 i = 0U; i < CNN_INPUT_SIZE; ++i) {
		g_cnn_input[i] = input_40x80[i];
	}

	return run_cnn_ip_from_internal_input(out_class);
}

void cnn_get_last_input_stats(int* min_value,
		int* max_value,
		int* sum,
		u32* checksum)
{
	int minv = 127;
	int maxv = -128;
	int total = 0;
	u32 hash = 2166136261U;

	for (u32 i = 0U; i < CNN_INPUT_SIZE; ++i) {
		int v = (int)g_cnn_input[i];
		if (v < minv) {
			minv = v;
		}
		if (v > maxv) {
			maxv = v;
		}
		total += v;
		hash ^= (u8)g_cnn_input[i];
		hash *= 16777619U;
	}

	if (min_value != 0) {
		*min_value = minv;
	}
	if (max_value != 0) {
		*max_value = maxv;
	}
	if (sum != 0) {
		*sum = total;
	}
	if (checksum != 0) {
		*checksum = hash;
	}
}

void cnn_dump_last_input_pgm(const char* name)
{
	const char* label = (name == 0) ? "cnn_input" : name;

	xil_printf("\r\nBEGIN_PGM %s\r\n", label);
	xil_printf("P2\r\n");
	xil_printf("%d %d\r\n", (int)CNN_INPUT_WIDTH, (int)CNN_INPUT_HEIGHT);
	xil_printf("255\r\n");

	for (u32 y = 0U; y < CNN_INPUT_HEIGHT; ++y) {
		for (u32 x = 0U; x < CNN_INPUT_WIDTH; ++x) {
			int v = (int)g_cnn_input[y * CNN_INPUT_WIDTH + x] + 128;
			if (v < 0) {
				v = 0;
			}
			if (v > 255) {
				v = 255;
			}
			xil_printf("%d ", v);
		}
		xil_printf("\r\n");
	}

	xil_printf("END_PGM %s\r\n\r\n", label);
}
