#ifndef CNN_TEST_H_
#define CNN_TEST_H_

#include "../cnn/cnn_driver.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "xtime_l.h"

typedef void (*CnnTestPatternFillFn)(s8* input);

typedef struct {
	const char* name;
	CnnTestPatternFillFn fill;
} CnnTestPattern;

static s8 g_cnn_test_input[CNN_INPUT_SIZE] __attribute__((aligned(64)));

#define CNN_TEST_FRAME_STRIDE_BYTES (CNN_FRAME_WIDTH * CNN_FRAME_BYTES_PER_PIXEL)
#define CNN_TEST_FRAME_SIZE_BYTES   (CNN_TEST_FRAME_STRIDE_BYTES * CNN_FRAME_HEIGHT)

typedef void (*CnnFramePatternFillFn)(u8* frame);

typedef struct {
	const char* name;
	CnnFramePatternFillFn fill;
} CnnFrameTestPattern;

static u8 g_cnn_test_frame[CNN_TEST_FRAME_SIZE_BYTES] __attribute__((aligned(64)));

static inline u32 cnn_test_elapsed_us(XTime start_time, XTime end_time)
{
	u64 ticks = (u64)(end_time - start_time);
	return (u32)((ticks * 1000000ULL) / (u64)COUNTS_PER_SECOND);
}

static inline void cnn_test_fill_all_zero(s8* input)
{
	for (u32 i = 0U; i < CNN_INPUT_SIZE; ++i) {
		input[i] = 0;
	}
}

static inline void cnn_test_fill_all_max(s8* input)
{
	for (u32 i = 0U; i < CNN_INPUT_SIZE; ++i) {
		input[i] = 127;
	}
}

static inline void cnn_test_fill_vertical_split(s8* input)
{
	for (u32 y = 0U; y < CNN_INPUT_HEIGHT; ++y) {
		for (u32 x = 0U; x < CNN_INPUT_WIDTH; ++x) {
			input[y * CNN_INPUT_WIDTH + x] =
					(x < (CNN_INPUT_WIDTH / 2U)) ? 0 : 127;
		}
	}
}

static inline void cnn_test_fill_horizontal_gradient(s8* input)
{
	for (u32 y = 0U; y < CNN_INPUT_HEIGHT; ++y) {
		for (u32 x = 0U; x < CNN_INPUT_WIDTH; ++x) {
			input[y * CNN_INPUT_WIDTH + x] =
					(s8)((x * 127U) / (CNN_INPUT_WIDTH - 1U));
		}
	}
}

static inline u32 cnn_test_input_checksum(const s8* input)
{
	u32 hash = 2166136261U;

	for (u32 i = 0U; i < CNN_INPUT_SIZE; ++i) {
		hash ^= (u8)input[i];
		hash *= 16777619U;
	}

	return hash;
}

static inline void cnn_test_input_stats(const s8* input,
		int* min_value,
		int* max_value,
		int* sum)
{
	int minv = 127;
	int maxv = -128;
	int total = 0;

	for (u32 i = 0U; i < CNN_INPUT_SIZE; ++i) {
		int v = (int)input[i];
		if (v < minv) {
			minv = v;
		}
		if (v > maxv) {
			maxv = v;
		}
		total += v;
	}

	*min_value = minv;
	*max_value = maxv;
	*sum = total;
}

static inline int cnn_test_run_one_pattern(const CnnTestPattern* pattern)
{
	int pred0 = -1;
	int pred1 = -1;
	XTime start_time;
	XTime end_time;

	pattern->fill(g_cnn_test_input);

	int minv;
	int maxv;
	int sum;
	cnn_test_input_stats(g_cnn_test_input, &minv, &maxv, &sum);
	u32 checksum = cnn_test_input_checksum(g_cnn_test_input);

	xil_printf("[CNN TEST] pattern=%s min=%d max=%d sum=%d checksum=0x%08x\r\n",
			pattern->name,
			minv,
			maxv,
			sum,
			(unsigned int)checksum);

	XTime_GetTime(&start_time);
	int status0 = cnn_run_from_input(g_cnn_test_input, &pred0);
	XTime_GetTime(&end_time);
	u32 elapsed0 = cnn_test_elapsed_us(start_time, end_time);

	XTime_GetTime(&start_time);
	int status1 = cnn_run_from_input(g_cnn_test_input, &pred1);
	XTime_GetTime(&end_time);
	u32 elapsed1 = cnn_test_elapsed_us(start_time, end_time);

	if (status0 != XST_SUCCESS || status1 != XST_SUCCESS) {
		xil_printf("[CNN TEST] result=FAIL status0=%d status1=%d time0=%d us time1=%d us\r\n",
				status0,
				status1,
				(int)elapsed0,
				(int)elapsed1);
		return XST_FAILURE;
	}

	xil_printf("[CNN TEST] result=%s pred0=%d pred1=%d time0=%d us time1=%d us\r\n",
			(pred0 == pred1) ? "PASS" : "DIFF",
			pred0,
			pred1,
			(int)elapsed0,
			(int)elapsed1);

	return (pred0 == pred1) ? XST_SUCCESS : XST_FAILURE;
}

static inline void cnn_run_fixed_input_tests(void)
{
	static const CnnTestPattern patterns[] = {
		{"all_zero", cnn_test_fill_all_zero},
		{"all_127", cnn_test_fill_all_max},
		{"vertical_split", cnn_test_fill_vertical_split},
		{"horizontal_gradient", cnn_test_fill_horizontal_gradient},
	};

	xil_printf("\r\n[CNN TEST] fixed 40x80 input test start\r\n");
	xil_printf("[CNN TEST] input range is signed INT8 grayscale -127..127\r\n");

	u32 pass_count = 0U;
	const u32 pattern_count = sizeof(patterns) / sizeof(patterns[0]);

	for (u32 i = 0U; i < pattern_count; ++i) {
		if (cnn_test_run_one_pattern(&patterns[i]) == XST_SUCCESS) {
			++pass_count;
		}
	}

	xil_printf("[CNN TEST] summary pass=%d total=%d\r\n\r\n",
			(int)pass_count,
			(int)pattern_count);
}

static inline void cnn_test_fill_frame_rgb(u8* frame, u8 r, u8 g, u8 b)
{
	for (u32 y = 0U; y < CNN_FRAME_HEIGHT; ++y) {
		u8* row = frame + y * CNN_TEST_FRAME_STRIDE_BYTES;
		for (u32 x = 0U; x < CNN_FRAME_WIDTH; ++x) {
			u8* pix = row + x * CNN_FRAME_BYTES_PER_PIXEL;
			pix[0] = r;
			pix[1] = g;
			pix[2] = b;
		}
	}
}

static inline void cnn_test_fill_roi_rgb(u8* frame, u8 r, u8 g, u8 b)
{
	CnnCrop crop = cnn_get_crop();
	u32 x_end = crop.x + crop.width;
	u32 y_end = crop.y + crop.height;

	for (u32 y = crop.y; y < y_end; ++y) {
		u8* row = frame + y * CNN_TEST_FRAME_STRIDE_BYTES;
		for (u32 x = crop.x; x < x_end; ++x) {
			u8* pix = row + x * CNN_FRAME_BYTES_PER_PIXEL;
			pix[0] = r;
			pix[1] = g;
			pix[2] = b;
		}
	}
}

static inline void cnn_test_fill_frame_roi_black(u8* frame)
{
	cnn_test_fill_frame_rgb(frame, 0, 0, 0);
	cnn_test_fill_roi_rgb(frame, 0, 0, 0);
}

static inline void cnn_test_fill_frame_roi_white(u8* frame)
{
	cnn_test_fill_frame_rgb(frame, 0, 0, 0);
	cnn_test_fill_roi_rgb(frame, 255, 255, 255);
}

static inline void cnn_test_fill_frame_roi_vertical_split(u8* frame)
{
	cnn_test_fill_frame_rgb(frame, 0, 0, 0);

	CnnCrop crop = cnn_get_crop();
	u32 x_end = crop.x + crop.width;
	u32 y_end = crop.y + crop.height;
	u32 split_x = crop.x + crop.width / 2U;

	for (u32 y = crop.y; y < y_end; ++y) {
		u8* row = frame + y * CNN_TEST_FRAME_STRIDE_BYTES;
		for (u32 x = crop.x; x < x_end; ++x) {
			u8 v = (x < split_x) ? 0 : 255;
			u8* pix = row + x * CNN_FRAME_BYTES_PER_PIXEL;
			pix[0] = v;
			pix[1] = v;
			pix[2] = v;
		}
	}
}

static inline void cnn_test_fill_frame_roi_horizontal_gradient(u8* frame)
{
	cnn_test_fill_frame_rgb(frame, 0, 0, 0);

	CnnCrop crop = cnn_get_crop();
	u32 x_end = crop.x + crop.width;
	u32 y_end = crop.y + crop.height;
	u32 denom = (crop.width > 1U) ? (crop.width - 1U) : 1U;

	for (u32 y = crop.y; y < y_end; ++y) {
		u8* row = frame + y * CNN_TEST_FRAME_STRIDE_BYTES;
		for (u32 x = crop.x; x < x_end; ++x) {
			u8 v = (u8)(((x - crop.x) * 255U) / denom);
			u8* pix = row + x * CNN_FRAME_BYTES_PER_PIXEL;
			pix[0] = v;
			pix[1] = v;
			pix[2] = v;
		}
	}
}

static inline int cnn_test_run_one_frame_pattern(const CnnFrameTestPattern* pattern)
{
	int pred0 = -1;
	int pred1 = -1;
	XTime start_time;
	XTime end_time;

	pattern->fill(g_cnn_test_frame);
	Xil_DCacheFlushRange((UINTPTR)g_cnn_test_frame, CNN_TEST_FRAME_SIZE_BYTES);

	CnnCrop crop = cnn_get_crop();
	xil_printf("[CNN FRAME TEST] pattern=%s roi=x:%d y:%d w:%d h:%d\r\n",
			pattern->name,
			(int)crop.x,
			(int)crop.y,
			(int)crop.width,
			(int)crop.height);

	XTime_GetTime(&start_time);
	int status0 = cnn_run_from_frame((UINTPTR)g_cnn_test_frame,
			CNN_FRAME_WIDTH,
			CNN_FRAME_HEIGHT,
			CNN_TEST_FRAME_STRIDE_BYTES,
			CNN_PIXEL_RGB888,
			&pred0);
	XTime_GetTime(&end_time);
	u32 elapsed0 = cnn_test_elapsed_us(start_time, end_time);

	int minv0;
	int maxv0;
	int sum0;
	u32 checksum0;
	cnn_get_last_input_stats(&minv0, &maxv0, &sum0, &checksum0);

	XTime_GetTime(&start_time);
	int status1 = cnn_run_from_frame((UINTPTR)g_cnn_test_frame,
			CNN_FRAME_WIDTH,
			CNN_FRAME_HEIGHT,
			CNN_TEST_FRAME_STRIDE_BYTES,
			CNN_PIXEL_RGB888,
			&pred1);
	XTime_GetTime(&end_time);
	u32 elapsed1 = cnn_test_elapsed_us(start_time, end_time);

	int minv1;
	int maxv1;
	int sum1;
	u32 checksum1;
	cnn_get_last_input_stats(&minv1, &maxv1, &sum1, &checksum1);

	xil_printf("[CNN FRAME TEST] input0 min=%d max=%d sum=%d checksum=0x%08x\r\n",
			minv0,
			maxv0,
			(int)sum0,
			(unsigned int)checksum0);
	xil_printf("[CNN FRAME TEST] input1 min=%d max=%d sum=%d checksum=0x%08x\r\n",
			minv1,
			maxv1,
			(int)sum1,
			(unsigned int)checksum1);

	if (status0 != XST_SUCCESS || status1 != XST_SUCCESS) {
		xil_printf("[CNN FRAME TEST] result=FAIL status0=%d status1=%d time0=%d us time1=%d us\r\n",
				status0,
				status1,
				(int)elapsed0,
				(int)elapsed1);
		return XST_FAILURE;
	}

	const int same_input = (minv0 == minv1) &&
			(maxv0 == maxv1) &&
			(sum0 == sum1) &&
			(checksum0 == checksum1);
	const int same_pred = (pred0 == pred1);

	xil_printf("[CNN FRAME TEST] result=%s pred0=%d pred1=%d time0=%d us time1=%d us\r\n",
			(same_input && same_pred) ? "PASS" : "DIFF",
			pred0,
			pred1,
			(int)elapsed0,
			(int)elapsed1);

	return (same_input && same_pred) ? XST_SUCCESS : XST_FAILURE;
}

static inline void cnn_run_frame_preprocess_tests(void)
{
	static const CnnFrameTestPattern patterns[] = {
		{"roi_black", cnn_test_fill_frame_roi_black},
		{"roi_white", cnn_test_fill_frame_roi_white},
		{"roi_vertical_split", cnn_test_fill_frame_roi_vertical_split},
		{"roi_horizontal_gradient", cnn_test_fill_frame_roi_horizontal_gradient},
	};

	xil_printf("\r\n[CNN FRAME TEST] synthetic 1920x1080 RGB frame test start\r\n");
	xil_printf("[CNN FRAME TEST] path: frame -> ROI crop -> resize 80x40 -> grayscale -> CNN IP\r\n");

	u32 pass_count = 0U;
	const u32 pattern_count = sizeof(patterns) / sizeof(patterns[0]);

	for (u32 i = 0U; i < pattern_count; ++i) {
		if (cnn_test_run_one_frame_pattern(&patterns[i]) == XST_SUCCESS) {
			++pass_count;
		}
	}

	xil_printf("[CNN FRAME TEST] summary pass=%d total=%d\r\n\r\n",
			(int)pass_count,
			(int)pattern_count);
}

typedef struct {
	const char* serial;
	int expected_class;
	int x_offset;
	int y_offset;
	u32 scale;
	u8 bg;
	u8 fg;
} CnnSerialTestCase;

static inline const char* cnn_test_class_name(int class_id)
{
	switch (class_id) {
	case 0:
		return "XK";
	case 1:
		return "MW";
	case 2:
		return "RZ";
	case 3:
		return "OP";
	default:
		return "NA";
	}
}

static inline u8 cnn_test_font_row(char ch, u32 row)
{
	static const u8 digit_font[10][7] = {
		{0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}, /* 0 */
		{0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}, /* 1 */
		{0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F}, /* 2 */
		{0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E}, /* 3 */
		{0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}, /* 4 */
		{0x1F, 0x10, 0x10, 0x1E, 0x01, 0x01, 0x1E}, /* 5 */
		{0x0E, 0x10, 0x10, 0x1E, 0x11, 0x11, 0x0E}, /* 6 */
		{0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}, /* 7 */
		{0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}, /* 8 */
		{0x0E, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x0E}, /* 9 */
	};

	if (ch >= '0' && ch <= '9') {
		return digit_font[ch - '0'][row];
	}

	switch (ch) {
	case 'K':
	{
		static const u8 font[7] = {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11};
		return font[row];
	}
	case 'M':
	{
		static const u8 font[7] = {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11};
		return font[row];
	}
	case 'O':
	{
		static const u8 font[7] = {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
		return font[row];
	}
	case 'P':
	{
		static const u8 font[7] = {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10};
		return font[row];
	}
	case 'R':
	{
		static const u8 font[7] = {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11};
		return font[row];
	}
	case 'W':
	{
		static const u8 font[7] = {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11};
		return font[row];
	}
	case 'X':
	{
		static const u8 font[7] = {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11};
		return font[row];
	}
	case 'Z':
	{
		static const u8 font[7] = {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F};
		return font[row];
	}
	default:
		return 0U;
	}
}

static inline void cnn_test_put_pixel_rgb(u8* frame, u32 x, u32 y, u8 value)
{
	if (x >= CNN_FRAME_WIDTH || y >= CNN_FRAME_HEIGHT) {
		return;
	}

	u8* pix = frame + y * CNN_TEST_FRAME_STRIDE_BYTES + x * CNN_FRAME_BYTES_PER_PIXEL;
	pix[0] = value;
	pix[1] = value;
	pix[2] = value;
}

static inline void cnn_test_draw_filled_rect(u8* frame,
		u32 x0,
		u32 y0,
		u32 width,
		u32 height,
		u8 value)
{
	u32 x1 = x0 + width;
	u32 y1 = y0 + height;

	if (x1 > CNN_FRAME_WIDTH) {
		x1 = CNN_FRAME_WIDTH;
	}
	if (y1 > CNN_FRAME_HEIGHT) {
		y1 = CNN_FRAME_HEIGHT;
	}

	for (u32 y = y0; y < y1; ++y) {
		for (u32 x = x0; x < x1; ++x) {
			cnn_test_put_pixel_rgb(frame, x, y, value);
		}
	}
}

static inline void cnn_test_draw_char(u8* frame, char ch, u32 x0, u32 y0, u32 scale, u8 value)
{
	for (u32 row = 0U; row < 7U; ++row) {
		u8 bits = cnn_test_font_row(ch, row);
		for (u32 col = 0U; col < 5U; ++col) {
			if ((bits & (1U << (4U - col))) != 0U) {
				cnn_test_draw_filled_rect(frame,
						x0 + col * scale,
						y0 + row * scale,
						scale,
						scale,
						value);
			}
		}
	}
}

static inline u32 cnn_test_strlen(const char* text)
{
	u32 len = 0U;
	while (text[len] != '\0') {
		++len;
	}
	return len;
}

static inline void cnn_test_draw_text(u8* frame,
		const char* text,
		u32 x0,
		u32 y0,
		u32 scale,
		u8 value)
{
	u32 cursor_x = x0;

	for (u32 i = 0U; text[i] != '\0'; ++i) {
		cnn_test_draw_char(frame, text[i], cursor_x, y0, scale, value);
		cursor_x += 6U * scale;
	}
}

static inline void cnn_test_fill_serial_frame(u8* frame, const CnnSerialTestCase* test_case)
{
	cnn_test_fill_frame_rgb(frame, 0, 0, 0);

	CnnCrop crop = cnn_get_crop();
	cnn_test_draw_filled_rect(frame,
			crop.x,
			crop.y,
			crop.width,
			crop.height,
			test_case->bg);

	u32 text_len = cnn_test_strlen(test_case->serial);
	u32 text_w = (text_len == 0U) ? 0U : ((text_len * 6U - 1U) * test_case->scale);
	u32 text_h = 7U * test_case->scale;
	u32 base_x = crop.x + ((crop.width > text_w) ? ((crop.width - text_w) / 2U) : 0U);
	u32 base_y = crop.y + ((crop.height > text_h) ? ((crop.height - text_h) / 2U) : 0U);
	int text_x_signed = (int)base_x + test_case->x_offset;
	int text_y_signed = (int)base_y + test_case->y_offset;
	u32 text_x = (text_x_signed < 0) ? 0U : (u32)text_x_signed;
	u32 text_y = (text_y_signed < 0) ? 0U : (u32)text_y_signed;
	u32 max_text_x = crop.x + ((crop.width > text_w) ? (crop.width - text_w) : 0U);
	u32 max_text_y = crop.y + ((crop.height > text_h) ? (crop.height - text_h) : 0U);

	if (text_x < crop.x) {
		text_x = crop.x;
	}
	if (text_y < crop.y) {
		text_y = crop.y;
	}
	if (text_x > max_text_x) {
		text_x = max_text_x;
	}
	if (text_y > max_text_y) {
		text_y = max_text_y;
	}

	cnn_test_draw_text(frame,
			test_case->serial,
			text_x,
			text_y,
			test_case->scale,
			test_case->fg);
}

static inline int cnn_test_run_one_serial_case(const CnnSerialTestCase* test_case, u32 index)
{
	int pred = -1;
	XTime start_time;
	XTime end_time;

	cnn_test_fill_serial_frame(g_cnn_test_frame, test_case);
	Xil_DCacheFlushRange((UINTPTR)g_cnn_test_frame, CNN_TEST_FRAME_SIZE_BYTES);

	XTime_GetTime(&start_time);
	int status = cnn_run_from_frame((UINTPTR)g_cnn_test_frame,
			CNN_FRAME_WIDTH,
			CNN_FRAME_HEIGHT,
			CNN_TEST_FRAME_STRIDE_BYTES,
			CNN_PIXEL_RGB888,
			&pred);
	XTime_GetTime(&end_time);

	int minv;
	int maxv;
	int sum;
	u32 checksum;
	cnn_get_last_input_stats(&minv, &maxv, &sum, &checksum);

	u32 elapsed = cnn_test_elapsed_us(start_time, end_time);
	const int pass = (status == XST_SUCCESS) && (pred == test_case->expected_class);

	xil_printf("[CNN SERIAL TEST] #%d serial=%s expected=%s(%d) pred=%s(%d) result=%s time=%d us\r\n",
			(int)index,
			test_case->serial,
			cnn_test_class_name(test_case->expected_class),
			test_case->expected_class,
			cnn_test_class_name(pred),
			pred,
			pass ? "PASS" : "FAIL",
			(int)elapsed);
	xil_printf("[CNN SERIAL TEST] input min=%d max=%d sum=%d checksum=0x%08x status=%d\r\n",
			minv,
			maxv,
			(int)sum,
			(unsigned int)checksum,
			status);

	return pass ? XST_SUCCESS : XST_FAILURE;
}

static inline void cnn_run_serial_image_tests(void)
{
	static const CnnSerialTestCase cases[] = {
		{"XK03", 0,   0,   0, 42, 230,  20},
		{"XK17", 0,  40,   0, 42, 230,  20},
		{"XK28", 0, -40,   0, 42, 230,  20},
		{"XK41", 0,   0,  30, 38, 220,  30},
		{"XK59", 0,   0, -30, 38, 240,  15},
		{"XK72", 0,  60,  25, 34, 210,  20},
		{"XK84", 0, -60, -25, 34, 235,  25},
		{"XK96", 0,  20, -20, 46, 225,  10},

		{"MW04", 1,   0,   0, 42, 230,  20},
		{"MW16", 1,  40,   0, 42, 230,  20},
		{"MW27", 1, -40,   0, 42, 230,  20},
		{"MW39", 1,   0,  30, 38, 220,  30},
		{"MW52", 1,   0, -30, 38, 240,  15},
		{"MW63", 1,  60,  25, 34, 210,  20},
		{"MW75", 1, -60, -25, 34, 235,  25},
		{"MW88", 1,  20, -20, 46, 225,  10},

		{"RZ02", 2,   0,   0, 42, 230,  20},
		{"RZ15", 2,  40,   0, 42, 230,  20},
		{"RZ26", 2, -40,   0, 42, 230,  20},
		{"RZ38", 2,   0,  30, 38, 220,  30},
		{"RZ49", 2,   0, -30, 38, 240,  15},
		{"RZ61", 2,  60,  25, 34, 210,  20},
		{"RZ73", 2, -60, -25, 34, 235,  25},
		{"RZ95", 2,  20, -20, 46, 225,  10},

		{"OP01", 3,   0,   0, 42, 230,  20},
		{"OP14", 3,  40,   0, 42, 230,  20},
		{"OP25", 3, -40,   0, 42, 230,  20},
		{"OP37", 3,   0,  30, 38, 220,  30},
		{"OP48", 3,   0, -30, 38, 240,  15},
		{"OP60", 3,  60,  25, 34, 210,  20},
		{"OP82", 3, -60, -25, 34, 235,  25},
		{"OP94", 3,  20, -20, 46, 225,  10},
	};

	xil_printf("\r\n[CNN SERIAL TEST] synthetic serial-number frame test start\r\n");
	xil_printf("[CNN SERIAL TEST] path: 1920x1080 RGB -> ROI -> resize 80x40 -> grayscale -> CNN IP\r\n");
	xil_printf("[CNN SERIAL TEST] labels: XK=0 MW=1 RZ=2 OP=3\r\n");

	u32 pass_count = 0U;
	const u32 total_count = sizeof(cases) / sizeof(cases[0]);

	for (u32 i = 0U; i < total_count; ++i) {
		if (cnn_test_run_one_serial_case(&cases[i], i) == XST_SUCCESS) {
			++pass_count;
		}
	}

	xil_printf("[CNN SERIAL TEST] summary pass=%d total=%d accuracy=%d%%\r\n\r\n",
			(int)pass_count,
			(int)total_count,
			(int)((pass_count * 100U) / total_count));
}

static inline void cnn_preview_serial_input_images(void)
{
	static const CnnSerialTestCase preview_cases[] = {
		{"XK03", 0, 0, 0, 42, 230, 20},
		{"MW04", 1, 0, 0, 42, 230, 20},
		{"RZ02", 2, 0, 0, 42, 230, 20},
		{"OP01", 3, 0, 0, 42, 230, 20},
	};

	xil_printf("\r\n[CNN PREVIEW] dump preprocessed 80x40 PGM images start\r\n");
	xil_printf("[CNN PREVIEW] capture each block from BEGIN_PGM to END_PGM and save as .pgm\r\n");

	const u32 total_count = sizeof(preview_cases) / sizeof(preview_cases[0]);

	for (u32 i = 0U; i < total_count; ++i) {
		int pred = -1;
		const CnnSerialTestCase* test_case = &preview_cases[i];

		cnn_test_fill_serial_frame(g_cnn_test_frame, test_case);
		Xil_DCacheFlushRange((UINTPTR)g_cnn_test_frame, CNN_TEST_FRAME_SIZE_BYTES);

		int status = cnn_run_from_frame((UINTPTR)g_cnn_test_frame,
				CNN_FRAME_WIDTH,
				CNN_FRAME_HEIGHT,
				CNN_TEST_FRAME_STRIDE_BYTES,
				CNN_PIXEL_RGB888,
				&pred);

		xil_printf("[CNN PREVIEW] serial=%s expected=%s(%d) pred=%s(%d) status=%d\r\n",
				test_case->serial,
				cnn_test_class_name(test_case->expected_class),
				test_case->expected_class,
				cnn_test_class_name(pred),
				pred,
				status);

		cnn_dump_last_input_pgm(test_case->serial);
	}

	xil_printf("[CNN PREVIEW] done\r\n\r\n");
}

#define CNN_STRESS_TEST_COUNT             10000U
#define CNN_STRESS_PROGRESS_INTERVAL      100U
#define CNN_STRESS_FAIL_PRINT_LIMIT       50U

static inline u32 cnn_test_prng_next(u32* state)
{
	*state = (*state * 1664525U) + 1013904223U;
	return *state;
}

static inline int cnn_test_rand_range(u32* state, int min_value, int max_value)
{
	u32 span = (u32)(max_value - min_value + 1);
	return min_value + (int)(cnn_test_prng_next(state) % span);
}

static inline void cnn_test_make_generated_case(u32 index,
		u32* rng,
		CnnSerialTestCase* test_case,
		char serial_buf[5])
{
	int class_id = (int)(index & 0x3U);
	const char* prefix = cnn_test_class_name(class_id);
	int d0 = cnn_test_rand_range(rng, 0, 9);
	int d1 = cnn_test_rand_range(rng, 0, 9);

	serial_buf[0] = prefix[0];
	serial_buf[1] = prefix[1];
	serial_buf[2] = (char)('0' + d0);
	serial_buf[3] = (char)('0' + d1);
	serial_buf[4] = '\0';

	test_case->serial = serial_buf;
	test_case->expected_class = class_id;
	test_case->x_offset = cnn_test_rand_range(rng, -90, 90);
	test_case->y_offset = cnn_test_rand_range(rng, -60, 60);
	test_case->scale = (u32)cnn_test_rand_range(rng, 32, 46);
	test_case->bg = (u8)cnn_test_rand_range(rng, 210, 245);
	test_case->fg = (u8)cnn_test_rand_range(rng, 5, 45);
}

static inline void cnn_test_fill_serial_frame_roi_only(u8* frame,
		const CnnSerialTestCase* test_case)
{
	CnnCrop crop = cnn_get_crop();

	cnn_test_draw_filled_rect(frame,
			crop.x,
			crop.y,
			crop.width,
			crop.height,
			test_case->bg);

	u32 text_len = cnn_test_strlen(test_case->serial);
	u32 text_w = (text_len == 0U) ? 0U : ((text_len * 6U - 1U) * test_case->scale);
	u32 text_h = 7U * test_case->scale;
	u32 base_x = crop.x + ((crop.width > text_w) ? ((crop.width - text_w) / 2U) : 0U);
	u32 base_y = crop.y + ((crop.height > text_h) ? ((crop.height - text_h) / 2U) : 0U);
	int text_x_signed = (int)base_x + test_case->x_offset;
	int text_y_signed = (int)base_y + test_case->y_offset;
	u32 text_x = (text_x_signed < 0) ? 0U : (u32)text_x_signed;
	u32 text_y = (text_y_signed < 0) ? 0U : (u32)text_y_signed;
	u32 max_text_x = crop.x + ((crop.width > text_w) ? (crop.width - text_w) : 0U);
	u32 max_text_y = crop.y + ((crop.height > text_h) ? (crop.height - text_h) : 0U);

	if (text_x < crop.x) {
		text_x = crop.x;
	}
	if (text_y < crop.y) {
		text_y = crop.y;
	}
	if (text_x > max_text_x) {
		text_x = max_text_x;
	}
	if (text_y > max_text_y) {
		text_y = max_text_y;
	}

	cnn_test_draw_text(frame,
			test_case->serial,
			text_x,
			text_y,
			test_case->scale,
			test_case->fg);
}

static inline void cnn_run_serial_stress_tests(void)
{
	xil_printf("\r\n[CNN STRESS] generated serial-number test start\r\n");
	xil_printf("[CNN STRESS] total=%d path=1920x1080 RGB -> ROI -> resize 80x40 -> grayscale -> CNN IP\r\n",
			(int)CNN_STRESS_TEST_COUNT);
	xil_printf("[CNN STRESS] labels: XK=0 MW=1 RZ=2 OP=3\r\n");
	xil_printf("[CNN STRESS] printing progress every %d cases, failures up to %d cases\r\n",
			(int)CNN_STRESS_PROGRESS_INTERVAL,
			(int)CNN_STRESS_FAIL_PRINT_LIMIT);

	cnn_test_fill_frame_rgb(g_cnn_test_frame, 0, 0, 0);

	u32 rng = 0x13579BDFU;
	u32 pass_count = 0U;
	u32 fail_count = 0U;
	u32 fail_print_count = 0U;
	u32 class_total[4] = {0U, 0U, 0U, 0U};
	u32 class_pass[4] = {0U, 0U, 0U, 0U};
	u32 total_elapsed_us = 0U;
	XTime total_start;
	XTime total_end;

	XTime_GetTime(&total_start);

	for (u32 i = 0U; i < CNN_STRESS_TEST_COUNT; ++i) {
		CnnSerialTestCase test_case;
		char serial_buf[5];
		int pred = -1;
		XTime start_time;
		XTime end_time;

		cnn_test_make_generated_case(i, &rng, &test_case, serial_buf);
		cnn_test_fill_serial_frame_roi_only(g_cnn_test_frame, &test_case);
		Xil_DCacheFlushRange((UINTPTR)g_cnn_test_frame, CNN_TEST_FRAME_SIZE_BYTES);

		XTime_GetTime(&start_time);
		int status = cnn_run_from_frame((UINTPTR)g_cnn_test_frame,
				CNN_FRAME_WIDTH,
				CNN_FRAME_HEIGHT,
				CNN_TEST_FRAME_STRIDE_BYTES,
				CNN_PIXEL_RGB888,
				&pred);
		XTime_GetTime(&end_time);

		u32 elapsed = cnn_test_elapsed_us(start_time, end_time);
		total_elapsed_us += elapsed;

		int expected = test_case.expected_class;
		class_total[expected]++;

		if (status == XST_SUCCESS && pred == expected) {
			++pass_count;
			class_pass[expected]++;
		} else {
			++fail_count;

			if (fail_print_count < CNN_STRESS_FAIL_PRINT_LIMIT) {
				int minv;
				int maxv;
				int sum;
				u32 checksum;
				cnn_get_last_input_stats(&minv, &maxv, &sum, &checksum);

				xil_printf("[CNN STRESS FAIL] #%d serial=%s expected=%s(%d) pred=%s(%d) status=%d time=%d us sum=%d checksum=0x%08x\r\n",
						(int)i,
						test_case.serial,
						cnn_test_class_name(expected),
						expected,
						cnn_test_class_name(pred),
						pred,
						status,
						(int)elapsed,
						(int)sum,
						(unsigned int)checksum);
				++fail_print_count;
			}
		}

		if (((i + 1U) % CNN_STRESS_PROGRESS_INTERVAL) == 0U) {
			u32 done = i + 1U;
			u32 acc_x100 = (pass_count * 10000U) / done;
			u32 avg_us = total_elapsed_us / done;

			xil_printf("[CNN STRESS] progress=%d/%d pass=%d fail=%d acc=%d.%02d%% avg=%d us\r\n",
					(int)done,
					(int)CNN_STRESS_TEST_COUNT,
					(int)pass_count,
					(int)fail_count,
					(int)(acc_x100 / 100U),
					(int)(acc_x100 % 100U),
					(int)avg_us);
		}
	}

	XTime_GetTime(&total_end);

	u32 total_wall_us = cnn_test_elapsed_us(total_start, total_end);
	u32 acc_x100 = (pass_count * 10000U) / CNN_STRESS_TEST_COUNT;
	u32 avg_us = total_elapsed_us / CNN_STRESS_TEST_COUNT;

	xil_printf("[CNN STRESS] summary pass=%d fail=%d total=%d accuracy=%d.%02d%% avg=%d us wall=%d sec\r\n",
			(int)pass_count,
			(int)fail_count,
			(int)CNN_STRESS_TEST_COUNT,
			(int)(acc_x100 / 100U),
			(int)(acc_x100 % 100U),
			(int)avg_us,
			(int)(total_wall_us / 1000000U));

	for (u32 c = 0U; c < 4U; ++c) {
		u32 class_acc_x100 = (class_total[c] == 0U) ? 0U : ((class_pass[c] * 10000U) / class_total[c]);
		xil_printf("[CNN STRESS] class=%s(%d) pass=%d total=%d accuracy=%d.%02d%%\r\n",
				cnn_test_class_name((int)c),
				(int)c,
				(int)class_pass[c],
				(int)class_total[c],
				(int)(class_acc_x100 / 100U),
				(int)(class_acc_x100 % 100U));
	}

	xil_printf("[CNN STRESS] done\r\n\r\n");
}

#endif
