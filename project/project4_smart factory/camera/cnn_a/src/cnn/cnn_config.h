#ifndef CNN_CONFIG_H_
#define CNN_CONFIG_H_

#include "xparameters.h"

/*
 * Change only these ROI constants when the inference area changes.
 * Coordinates are in the current camera frame, before resize.
 */
#define CNN_FRAME_WIDTH        1920U
#define CNN_FRAME_HEIGHT       1080U
#define CNN_FRAME_BYTES_PER_PIXEL 3U

#define CNN_ROI_X              300U
#define CNN_ROI_Y              300U
#define CNN_ROI_WIDTH          1200U
#define CNN_ROI_HEIGHT         600U

#define CNN_PRED_GPIO_DEVICE_ID    XPAR_AXI_GPIO_0_DEVICE_ID
#define CNN_IR_GPIO_DEVICE_ID      XPAR_AXI_GPIO_1_DEVICE_ID
#define CNN_GPIO_CHANNEL           1U
#define CNN_IR_BIT_MASK            0x1U
#define CNN_PRED_CLASS_MASK        0x3U

#define CNN_PIXEL_ORDER            CNN_PIXEL_RGB888

#endif
