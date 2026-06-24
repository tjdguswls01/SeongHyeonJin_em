#include "xparameters.h"

#include "platform/platform.h"
#include "ov5640/OV5640.h"
#include "ov5640/ScuGicInterruptController.h"
#include "ov5640/PS_GPIO.h"
#include "ov5640/AXI_VDMA.h"
#include "ov5640/PS_IIC.h"

#include "MIPI_D_PHY_RX.h"
#include "MIPI_CSI_2_RX.h"


#define IRPT_CTL_DEVID 		XPAR_PS7_SCUGIC_0_DEVICE_ID
#define GPIO_DEVID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_IRPT_ID			XPAR_PS7_GPIO_0_INTR
#define CAM_I2C_DEVID		XPAR_PS7_I2C_0_DEVICE_ID
#define CAM_I2C_IRPT_ID		XPAR_PS7_I2C_0_INTR
#define VDMA_DEVID			XPAR_AXIVDMA_0_DEVICE_ID
#define VDMA_MM2S_IRPT_ID	XPAR_FABRIC_AXI_VDMA_0_MM2S_INTROUT_INTR
#define VDMA_S2MM_IRPT_ID	XPAR_FABRIC_AXI_VDMA_0_S2MM_INTROUT_INTR
#define CAM_I2C_SCLK_RATE	100000

#define DDR_BASE_ADDR		XPAR_DDR_MEM_BASEADDR
#define MEM_BASE_ADDR		(DDR_BASE_ADDR + 0x0A000000)

#define GAMMA_BASE_ADDR     XPAR_AXI_GAMMACORRECTION_0_BASEADDR

using namespace digilent;

#include "xuartps.h"
#include "xgpio.h"
#include "xil_printf.h"
#include "sleep.h"

#define UART_DEVICE_ID XPAR_XUARTPS_0_DEVICE_ID
#define GPIO_OUT_ID    XPAR_AXI_GPIO_0_DEVICE_ID
#define GPIO_IN_ID     XPAR_AXI_GPIO_1_DEVICE_ID

#define GPIO_TRIGGER_MASK 0x01
#define GPIO_OUT_MASK     0x03
#define UART_NG_VALUE     0xFF
#define IR_DEBOUNCE_US    5000U
#define LOOP_DELAY_US      1000U

XUartPs Uart_Ps;
XGpio GpioOut;
XGpio GpioIn;

static u32 read_ir_level()
{
	return XGpio_DiscreteRead(&GpioIn, 1) & GPIO_TRIGGER_MASK;
}

static void pred_class_self_test()
{
	xil_printf("pred_class self-test start\r\n");

	for (u32 i = 0U; i < 2U; ++i) {
		for (u32 v = 0U; v < 4U; ++v) {
			XGpio_DiscreteWrite(&GpioOut, 1, v & GPIO_OUT_MASK);
			xil_printf("pred_class test -> %d\r\n", (int)(v & GPIO_OUT_MASK));
			usleep(500000U);
		}
	}

	XGpio_DiscreteWrite(&GpioOut, 1, 0U);
	xil_printf("pred_class self-test done\r\n");
}

void pipeline_mode_change(AXI_VDMA<ScuGicInterruptController>& vdma_driver, OV5640& cam, VideoOutput& vid, Resolution res, OV5640_cfg::mode_t mode)
{
	//Bring up input pipeline back-to-front
	{
		vdma_driver.resetWrite();
		MIPI_CSI_2_RX_mWriteReg(XPAR_MIPI_CSI_2_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET, (CR_RESET_MASK & ~CR_ENABLE_MASK));
		MIPI_D_PHY_RX_mWriteReg(XPAR_MIPI_D_PHY_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET, (CR_RESET_MASK & ~CR_ENABLE_MASK));
		cam.reset();
	}

	{
		vdma_driver.configureWrite(timing[static_cast<int>(res)].h_active, timing[static_cast<int>(res)].v_active);
		Xil_Out32(GAMMA_BASE_ADDR, 3); // Set Gamma correction factor to 1/1.8
		//TODO CSI-2, D-PHY config here
		cam.init();
	}

	{
		vdma_driver.enableWrite();
		MIPI_CSI_2_RX_mWriteReg(XPAR_MIPI_CSI_2_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET, CR_ENABLE_MASK);
		MIPI_D_PHY_RX_mWriteReg(XPAR_MIPI_D_PHY_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET, CR_ENABLE_MASK);
		cam.set_mode(mode);
		cam.set_awb(OV5640_cfg::awb_t::AWB_ADVANCED);
	}

	//Bring up output pipeline back-to-front
	{
		vid.reset();
		vdma_driver.resetRead();
	}

	{
		vid.configure(res);
		vdma_driver.configureRead(timing[static_cast<int>(res)].h_active, timing[static_cast<int>(res)].v_active);
	}

	{
		vid.enable();
		vdma_driver.enableRead();
	}
}

int main()
{
	init_platform();

	ScuGicInterruptController irpt_ctl(IRPT_CTL_DEVID);
	PS_GPIO<ScuGicInterruptController> gpio_driver(GPIO_DEVID, irpt_ctl, GPIO_IRPT_ID);
	PS_IIC<ScuGicInterruptController> iic_driver(CAM_I2C_DEVID, irpt_ctl, CAM_I2C_IRPT_ID, 100000);

	OV5640 cam(iic_driver, gpio_driver);
	AXI_VDMA<ScuGicInterruptController> vdma_driver(VDMA_DEVID, MEM_BASE_ADDR, irpt_ctl,
			VDMA_MM2S_IRPT_ID,
			VDMA_S2MM_IRPT_ID);
	VideoOutput vid(XPAR_VTC_0_DEVICE_ID, XPAR_VIDEO_DYNCLK_DEVICE_ID);

	pipeline_mode_change(vdma_driver, cam, vid, Resolution::R1920_1080_60_PP, OV5640_cfg::mode_t::MODE_1080P_1920_1080_30fps);

	XUartPs_Config *UartCfg;
	XGpio_Config *GpioOutCfg;
	XGpio_Config *GpioInCfg;

	u8 uart_data;
	u8 last_val = 0;
	u8 last_valid = 0;

	u32 curr_gpio;
	u32 last_reported_gpio;

	UartCfg = XUartPs_LookupConfig(UART_DEVICE_ID);
	if (UartCfg == NULL) return -1;
	XUartPs_CfgInitialize(&Uart_Ps, UartCfg, UartCfg->BaseAddress);
	XUartPs_SetBaudRate(&Uart_Ps, 115200);

	GpioOutCfg = XGpio_LookupConfig(GPIO_OUT_ID);
	if (GpioOutCfg == NULL) return -1;
	XGpio_CfgInitialize(&GpioOut, GpioOutCfg, GpioOutCfg->BaseAddress);
	XGpio_SetDataDirection(&GpioOut, 1, 0x0);

	GpioInCfg = XGpio_LookupConfig(GPIO_IN_ID);
	if (GpioInCfg == NULL) return -1;
	XGpio_CfgInitialize(&GpioIn, GpioInCfg, GpioInCfg->BaseAddress);
	XGpio_SetDataDirection(&GpioIn, 1, 0xFFFFFFFF);

	XGpio_DiscreteWrite(&GpioOut, 1, 0U);
	pred_class_self_test();

	u8 ir_ready = (read_ir_level() != 0U) ? 1U : 0U;
	last_reported_gpio = read_ir_level();

	xil_printf("Start\r\n");
	xil_printf("UART: raw byte 0..3 selects class, 0xFF means NG.\r\n");
	xil_printf("Output changes only on stable IR falling edge.\r\n");
	xil_printf("Initial IR=%d, ir_ready=%d\r\n", (int)last_reported_gpio, (int)ir_ready);

	while (1) {

	    while (XUartPs_Recv(&Uart_Ps, &uart_data, 1) > 0)
	    {
	        if (uart_data <= GPIO_OUT_MASK) {
	            last_val = uart_data;
	            last_valid = 1U;
	            xil_printf("UART RX class=%d\r\n", last_val);
	        } else if (uart_data == UART_NG_VALUE) {
	            last_valid = 0U;
	            xil_printf("UART RX NG\r\n");
	        } else if ((uart_data >= '0') && (uart_data <= '3')) {
	            last_val = uart_data - '0';
	            last_valid = 1U;
	            xil_printf("UART RX ascii class=%d\r\n", last_val);
	        }
	    }

	    curr_gpio = read_ir_level();
	    if (curr_gpio != last_reported_gpio) {
	    	xil_printf("IR changed: %d -> %d\r\n",
	    			(int)last_reported_gpio,
	    			(int)curr_gpio);
	    	last_reported_gpio = curr_gpio;
	    }

	    if (ir_ready != 0U) {
	        if (curr_gpio == 0U) {
	            usleep(IR_DEBOUNCE_US);
	            curr_gpio = read_ir_level();

	            if (curr_gpio == 0U) {
	            	if (last_valid != 0U) {
	            		XGpio_DiscreteWrite(&GpioOut, 1, last_val & GPIO_OUT_MASK);
	            		xil_printf("EDGE(FALL) -> OUT: %d\r\n", last_val);
	            	} else {
	            		XGpio_DiscreteWrite(&GpioOut, 1, 0U);
	            		xil_printf("EDGE(FALL) -> NG, OUT cleared\r\n");
	            	}
	                ir_ready = 0U;
	                last_reported_gpio = curr_gpio;
	            }
	        }
	    } else {
	        if (curr_gpio != 0U) {
	            usleep(IR_DEBOUNCE_US);
	            curr_gpio = read_ir_level();

	            if (curr_gpio != 0U) {
	                ir_ready = 1U;
	                last_reported_gpio = curr_gpio;
	                xil_printf("IR re-armed\r\n");
	            }
	        }
	    }

	    usleep(LOOP_DELAY_US);
	}
	cleanup_platform();

	return 0;
}
