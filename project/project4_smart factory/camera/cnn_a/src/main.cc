#include "xparameters.h"

#include "cnn/cnn_driver.h"
#include "platform/platform.h"
#include "ov5640/AXI_VDMA.h"
#include "ov5640/OV5640.h"
#include "ov5640/PS_GPIO.h"
#include "ov5640/PS_IIC.h"
#include "ov5640/ScuGicInterruptController.h"

#include "MIPI_CSI_2_RX.h"
#include "MIPI_D_PHY_RX.h"
#include "xgpio.h"
#include "xil_cache.h"
#include "xil_io.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "sleep.h"
#include "xtime_l.h"

#define IRPT_CTL_DEVID      XPAR_PS7_SCUGIC_0_DEVICE_ID
#define GPIO_DEVID          XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_IRPT_ID        XPAR_PS7_GPIO_0_INTR
#define CAM_I2C_DEVID       XPAR_PS7_I2C_0_DEVICE_ID
#define CAM_I2C_IRPT_ID     XPAR_PS7_I2C_0_INTR
#define VDMA_DEVID          XPAR_AXIVDMA_0_DEVICE_ID
#define VDMA_MM2S_IRPT_ID   XPAR_FABRIC_AXI_VDMA_0_MM2S_INTROUT_INTR
#define VDMA_S2MM_IRPT_ID   XPAR_FABRIC_AXI_VDMA_0_S2MM_INTROUT_INTR

#define DDR_BASE_ADDR       XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define MEM_BASE_ADDR       (DDR_BASE_ADDR + 0x0A000000)

#define GAMMA_BASE_ADDR     XPAR_AXI_GAMMACORRECTION_0_BASEADDR

#define CNN_IR_DEBOUNCE_US  5000U
#define LCD_DISPLAY_WIDTH   1024U
#define LCD_DISPLAY_HEIGHT  600U
#define LCD_REFRESH_US      200000U
#define LCD_FRAME_BASE_ADDR (MEM_BASE_ADDR + 0x04000000U)

using namespace digilent;

static int g_last_pred_class = -1;
static u16 g_lcd_xmap[LCD_DISPLAY_WIDTH];
static u16 g_lcd_ymap[LCD_DISPLAY_HEIGHT];
static int g_lcd_maps_ready = 0;

static const char* class_name(int pred_class)
{
	static const char* names[] = {"XK", "MW", "RZ", "OP"};
	if (pred_class < 0 || pred_class >= 4) {
		return "--";
	}
	return names[pred_class];
}

static const u8* glyph5x7(char c)
{
	static const u8 blank[7] = {0, 0, 0, 0, 0, 0, 0};
	static const u8 colon[7] = {0, 4, 4, 0, 4, 4, 0};
	static const u8 dash[7] = {0, 0, 0, 31, 0, 0, 0};
	static const u8 A[7] = {14, 17, 17, 31, 17, 17, 17};
	static const u8 C[7] = {14, 17, 16, 16, 16, 17, 14};
	static const u8 K[7] = {17, 18, 20, 24, 20, 18, 17};
	static const u8 L[7] = {16, 16, 16, 16, 16, 16, 31};
	static const u8 M[7] = {17, 27, 21, 21, 17, 17, 17};
	static const u8 O[7] = {14, 17, 17, 17, 17, 17, 14};
	static const u8 P[7] = {30, 17, 17, 30, 16, 16, 16};
	static const u8 R[7] = {30, 17, 17, 30, 20, 18, 17};
	static const u8 S[7] = {15, 16, 16, 14, 1, 1, 30};
	static const u8 W[7] = {17, 17, 17, 21, 21, 27, 17};
	static const u8 X[7] = {17, 17, 10, 4, 10, 17, 17};
	static const u8 Z[7] = {31, 1, 2, 4, 8, 16, 31};
	static const u8 n0[7] = {14, 17, 19, 21, 25, 17, 14};
	static const u8 n1[7] = {4, 12, 4, 4, 4, 4, 14};
	static const u8 n2[7] = {14, 17, 1, 2, 4, 8, 31};
	static const u8 n3[7] = {30, 1, 1, 14, 1, 1, 30};

	switch (c) {
	case 'A': return A;
	case 'C': return C;
	case 'K': return K;
	case 'L': return L;
	case 'M': return M;
	case 'O': return O;
	case 'P': return P;
	case 'R': return R;
	case 'S': return S;
	case 'W': return W;
	case 'X': return X;
	case 'Z': return Z;
	case '0': return n0;
	case '1': return n1;
	case '2': return n2;
	case '3': return n3;
	case ':': return colon;
	case '-': return dash;
	default: return blank;
	}
}

static void put_pixel_rgb888(UINTPTR frame_addr,
		u32 stride_bytes,
		u32 width,
		u32 height,
		int x,
		int y,
		u8 r,
		u8 g,
		u8 b)
{
	if (x < 0 || y < 0 || x >= (int)width || y >= (int)height) {
		return;
	}

	u8* pix = (u8*)frame_addr + (u32)y * stride_bytes + (u32)x * CNN_FRAME_BYTES_PER_PIXEL;
	pix[0] = r;
	pix[1] = g;
	pix[2] = b;
}

static void fill_rect_rgb888(UINTPTR frame_addr,
		u32 stride_bytes,
		u32 width,
		u32 height,
		int x,
		int y,
		int w,
		int h,
		u8 r,
		u8 g,
		u8 b)
{
	for (int yy = y; yy < y + h; ++yy) {
		for (int xx = x; xx < x + w; ++xx) {
			put_pixel_rgb888(frame_addr, stride_bytes, width, height, xx, yy, r, g, b);
		}
	}
}

static void draw_rect_rgb888(UINTPTR frame_addr,
		u32 stride_bytes,
		u32 width,
		u32 height,
		int x,
		int y,
		int w,
		int h,
		int thickness,
		u8 r,
		u8 g,
		u8 b)
{
	fill_rect_rgb888(frame_addr, stride_bytes, width, height, x, y, w, thickness, r, g, b);
	fill_rect_rgb888(frame_addr, stride_bytes, width, height, x, y + h - thickness, w, thickness, r, g, b);
	fill_rect_rgb888(frame_addr, stride_bytes, width, height, x, y, thickness, h, r, g, b);
	fill_rect_rgb888(frame_addr, stride_bytes, width, height, x + w - thickness, y, thickness, h, r, g, b);
}

static void draw_char_rgb888(UINTPTR frame_addr,
		u32 stride_bytes,
		u32 width,
		u32 height,
		int x,
		int y,
		char c,
		int scale,
		u8 r,
		u8 g,
		u8 b)
{
	const u8* glyph = glyph5x7(c);
	for (int row = 0; row < 7; ++row) {
		for (int col = 0; col < 5; ++col) {
			if ((glyph[row] >> (4 - col)) & 1U) {
				fill_rect_rgb888(frame_addr, stride_bytes, width, height,
						x + col * scale,
						y + row * scale,
						scale,
						scale,
						r,
						g,
						b);
			}
		}
	}
}

static void draw_text_rgb888(UINTPTR frame_addr,
		u32 stride_bytes,
		u32 width,
		u32 height,
		int x,
		int y,
		const char* text,
		int scale,
		u8 r,
		u8 g,
		u8 b)
{
	int cursor = x;
	while (*text != '\0') {
		draw_char_rgb888(frame_addr, stride_bytes, width, height, cursor, y, *text, scale, r, g, b);
		cursor += 6 * scale;
		++text;
	}
}

static void init_lcd_scale_map(void)
{
	if (g_lcd_maps_ready) {
		return;
	}

	for (u32 x = 0U; x < LCD_DISPLAY_WIDTH; ++x) {
		g_lcd_xmap[x] = (u16)((x * CNN_FRAME_WIDTH) / LCD_DISPLAY_WIDTH);
	}
	for (u32 y = 0U; y < LCD_DISPLAY_HEIGHT; ++y) {
		g_lcd_ymap[y] = (u16)((y * CNN_FRAME_HEIGHT) / LCD_DISPLAY_HEIGHT);
	}

	g_lcd_maps_ready = 1;
}

static void draw_cnn_overlay(UINTPTR display_addr, int pred_class)
{
	const u32 stride = LCD_DISPLAY_WIDTH * CNN_FRAME_BYTES_PER_PIXEL;
	const u32 display_w = LCD_DISPLAY_WIDTH;
	const u32 display_h = LCD_DISPLAY_HEIGHT;
	CnnCrop crop = cnn_get_crop();

	fill_rect_rgb888(display_addr, stride, display_w, display_h, 0, 0, display_w, 44, 0, 0, 0);
	draw_text_rgb888(display_addr, stride, display_w, display_h, 12, 10, "CLASS:", 3, 255, 255, 255);
	draw_text_rgb888(display_addr, stride, display_w, display_h, 132, 10, class_name(pred_class), 3, 0, 255, 255);

	if (pred_class >= 0) {
		char n[2] = {(char)('0' + (pred_class & 3)), '\0'};
		draw_text_rgb888(display_addr, stride, display_w, display_h, 188, 10, n, 3, 0, 255, 255);
	}

	int rx = (int)((crop.x * display_w) / CNN_FRAME_WIDTH);
	int ry = (int)((crop.y * display_h) / CNN_FRAME_HEIGHT);
	int rw = (int)((crop.width * display_w) / CNN_FRAME_WIDTH);
	int rh = (int)((crop.height * display_h) / CNN_FRAME_HEIGHT);
	if (rw < 4) {
		rw = 4;
	}
	if (rh < 4) {
		rh = 4;
	}
	draw_rect_rgb888(display_addr, stride, display_w, display_h, rx, ry, rw, rh, 3, 255, 255, 0);
}

static void update_lcd_frame(UINTPTR camera_addr, UINTPTR display_addr, int pred_class)
{
	init_lcd_scale_map();

	const u32 camera_stride = CNN_FRAME_WIDTH * CNN_FRAME_BYTES_PER_PIXEL;
	const u32 display_stride = LCD_DISPLAY_WIDTH * CNN_FRAME_BYTES_PER_PIXEL;

	Xil_DCacheInvalidateRange(camera_addr, camera_stride * CNN_FRAME_HEIGHT);

	for (u32 y = 0U; y < LCD_DISPLAY_HEIGHT; ++y) {
		const u8* src_row = (const u8*)camera_addr + (u32)g_lcd_ymap[y] * camera_stride;
		u8* dst_row = (u8*)display_addr + y * display_stride;

		for (u32 x = 0U; x < LCD_DISPLAY_WIDTH; ++x) {
			const u8* src = src_row + (u32)g_lcd_xmap[x] * CNN_FRAME_BYTES_PER_PIXEL;
			u8* dst = dst_row + x * CNN_FRAME_BYTES_PER_PIXEL;
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
		}
	}

	draw_cnn_overlay(display_addr, pred_class);

	Xil_DCacheFlushRange(display_addr, display_stride * LCD_DISPLAY_HEIGHT);
}

static void pipeline_mode_change(AXI_VDMA<ScuGicInterruptController>& vdma_driver,
		OV5640& cam,
		VideoOutput& vid,
		Resolution camera_res,
		Resolution display_res,
		OV5640_cfg::mode_t mode)
{
	{
		vdma_driver.resetWrite();
		MIPI_CSI_2_RX_mWriteReg(XPAR_MIPI_CSI_2_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET,
				(CR_RESET_MASK & ~CR_ENABLE_MASK));
		MIPI_D_PHY_RX_mWriteReg(XPAR_MIPI_D_PHY_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET,
				(CR_RESET_MASK & ~CR_ENABLE_MASK));
		cam.reset();
	}

	{
		vdma_driver.configureWrite(timing[static_cast<int>(camera_res)].h_active,
				timing[static_cast<int>(camera_res)].v_active);
		Xil_Out32(GAMMA_BASE_ADDR, 3);
		cam.init();
	}

	{
		vdma_driver.enableWrite();
		MIPI_CSI_2_RX_mWriteReg(XPAR_MIPI_CSI_2_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET,
				CR_ENABLE_MASK);
		MIPI_D_PHY_RX_mWriteReg(XPAR_MIPI_D_PHY_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET,
				CR_ENABLE_MASK);
		cam.set_mode(mode);
		cam.set_awb(OV5640_cfg::awb_t::AWB_ADVANCED);
	}

	{
		vid.reset();
		vdma_driver.resetRead();
	}

	{
		vid.configure(display_res);
		vdma_driver.configureReadFixed(timing[static_cast<int>(display_res)].h_active,
				timing[static_cast<int>(display_res)].v_active,
				LCD_FRAME_BASE_ADDR);
	}

	{
		vid.enable();
		vdma_driver.enableRead();
	}
}

static int init_gpio(XGpio* ir_gpio, XGpio* pred_gpio)
{
	int status = XGpio_Initialize(ir_gpio, CNN_IR_GPIO_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("IR GPIO init failed: %d\r\n", status);
		return status;
	}

	status = XGpio_Initialize(pred_gpio, CNN_PRED_GPIO_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("pred_class GPIO init failed: %d\r\n", status);
		return status;
	}

	XGpio_SetDataDirection(ir_gpio, CNN_GPIO_CHANNEL, 0xFFFFFFFFU);
	XGpio_SetDataDirection(pred_gpio, CNN_GPIO_CHANNEL, ~CNN_PRED_CLASS_MASK);
	XGpio_DiscreteWrite(pred_gpio, CNN_GPIO_CHANNEL, 0U);

	return XST_SUCCESS;
}

static u32 read_ir_state(XGpio* ir_gpio)
{
	return (XGpio_DiscreteRead(ir_gpio, CNN_GPIO_CHANNEL) & CNN_IR_BIT_MASK) ? 1U : 0U;
}

static void write_pred_class(XGpio* pred_gpio, int pred_class)
{
	XGpio_DiscreteWrite(pred_gpio, CNN_GPIO_CHANNEL, ((u32)pred_class) & CNN_PRED_CLASS_MASK);
}

static u32 read_pred_class(XGpio* pred_gpio)
{
	return XGpio_DiscreteRead(pred_gpio, CNN_GPIO_CHANNEL) & CNN_PRED_CLASS_MASK;
}

static u32 elapsed_us(XTime start_time, XTime end_time)
{
	u64 ticks = (u64)(end_time - start_time);
	return (u32)((ticks * 1000000ULL) / (u64)COUNTS_PER_SECOND);
}

static void run_cnn_once(XGpio* pred_gpio, const char* trigger_name)
{
	int pred_class = 0;
	int min_value = 0;
	int max_value = 0;
	int input_sum = 0;
	u32 checksum = 0U;

	xil_printf("CNN trigger=%s start\r\n", trigger_name);
	int status = cnn_run_from_frame((UINTPTR)MEM_BASE_ADDR,
			CNN_FRAME_WIDTH,
			CNN_FRAME_HEIGHT,
			CNN_FRAME_WIDTH * CNN_FRAME_BYTES_PER_PIXEL,
			CNN_PIXEL_ORDER,
			&pred_class);

	if (status == XST_SUCCESS) {
		write_pred_class(pred_gpio, pred_class);
		g_last_pred_class = pred_class & (int)CNN_PRED_CLASS_MASK;
		update_lcd_frame((UINTPTR)MEM_BASE_ADDR, (UINTPTR)LCD_FRAME_BASE_ADDR, g_last_pred_class);
		cnn_get_last_input_stats(&min_value, &max_value, &input_sum, &checksum);
		xil_printf("CNN trigger=%s pred_class=%d gpio=0x%x input[min=%d max=%d sum=%d crc=0x%08x]\r\n",
				trigger_name,
				pred_class & (int)CNN_PRED_CLASS_MASK,
				(unsigned int)read_pred_class(pred_gpio),
				min_value,
				max_value,
				input_sum,
				checksum);
	} else {
		write_pred_class(pred_gpio, 0);
		xil_printf("CNN trigger=%s failed status=%d\r\n",
				trigger_name,
				status);
	}
}

int main()
{
	init_platform();
	xil_printf("\r\nBOOT OK\r\n");

	ScuGicInterruptController irpt_ctl(IRPT_CTL_DEVID);
	PS_GPIO<ScuGicInterruptController> gpio_driver(GPIO_DEVID, irpt_ctl, GPIO_IRPT_ID);
	PS_IIC<ScuGicInterruptController> iic_driver(CAM_I2C_DEVID, irpt_ctl, CAM_I2C_IRPT_ID, 100000);

	XGpio ir_gpio;
	XGpio pred_gpio;
	if (init_gpio(&ir_gpio, &pred_gpio) != XST_SUCCESS) {
		xil_printf("GPIO init failed. Stop.\r\n");
		cleanup_platform();
		return XST_FAILURE;
	}

	if (cnn_init() != XST_SUCCESS) {
		xil_printf("CNN init failed. Stop.\r\n");
		cleanup_platform();
		return XST_FAILURE;
	}

	if (cnn_set_configured_crop(CNN_FRAME_WIDTH, CNN_FRAME_HEIGHT) != XST_SUCCESS) {
		xil_printf("CNN crop config failed. Stop.\r\n");
		cleanup_platform();
		return XST_FAILURE;
	}

	CnnCrop crop = cnn_get_crop();
	xil_printf("CNN init done.\r\n");
	xil_printf("CNN ROI x=%d y=%d w=%d h=%d\r\n",
			(int)crop.x, (int)crop.y, (int)crop.width, (int)crop.height);
	xil_printf("Video init start.\r\n");

	OV5640 cam(iic_driver, gpio_driver);
	AXI_VDMA<ScuGicInterruptController> vdma_driver(VDMA_DEVID, MEM_BASE_ADDR, irpt_ctl,
			VDMA_MM2S_IRPT_ID,
			VDMA_S2MM_IRPT_ID);
	VideoOutput vid(XPAR_VTC_0_DEVICE_ID, XPAR_VIDEO_DYNCLK_DEVICE_ID);

	pipeline_mode_change(vdma_driver, cam, vid,
			Resolution::R1920_1080_60_PP,
			Resolution::R1024_600_60_PP,
			OV5640_cfg::mode_t::MODE_1080P_1920_1080_30fps);

	xil_printf("Video init done.\r\n");
	update_lcd_frame((UINTPTR)MEM_BASE_ADDR, (UINTPTR)LCD_FRAME_BASE_ADDR, g_last_pred_class);
	xil_printf("Waiting for IR falling edge.\r\n");
	xil_printf("CNN runs only on IR falling edge.\r\n");

	u32 prev_ir = read_ir_state(&ir_gpio);
	XTime last_overlay_time;
	XTime_GetTime(&last_overlay_time);

	while (1) {
		u32 ir = read_ir_state(&ir_gpio);

		if (prev_ir == 1U && ir == 0U) {
			xil_printf("IR falling edge detected\r\n");
			usleep(CNN_IR_DEBOUNCE_US);
			ir = read_ir_state(&ir_gpio);
			if (ir == 0U) {
				run_cnn_once(&pred_gpio, "ir");
			} else {
				xil_printf("IR edge ignored by debounce\r\n");
			}
		}

		XTime now_time;
		XTime_GetTime(&now_time);
		if (elapsed_us(last_overlay_time, now_time) >= LCD_REFRESH_US) {
			update_lcd_frame((UINTPTR)MEM_BASE_ADDR, (UINTPTR)LCD_FRAME_BASE_ADDR, g_last_pred_class);
			last_overlay_time = now_time;
		}

		prev_ir = ir;
	}

	cleanup_platform();
	return 0;
}
