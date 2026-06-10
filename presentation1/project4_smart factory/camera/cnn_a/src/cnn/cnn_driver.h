#ifndef CNN_DRIVER_H_
#define CNN_DRIVER_H_

#include "xil_types.h"
#include "xstatus.h"

#include "cnn_config.h"

#define CNN_INPUT_WIDTH  80U
#define CNN_INPUT_HEIGHT 40U
#define CNN_INPUT_SIZE   (CNN_INPUT_WIDTH * CNN_INPUT_HEIGHT)

typedef enum {
	CNN_PIXEL_RGB888 = 0,
	CNN_PIXEL_BGR888 = 1
} CnnPixelOrder;

typedef struct {
	u32 x;
	u32 y;
	u32 width;
	u32 height;
} CnnCrop;

int cnn_init(void);
int cnn_set_crop(CnnCrop crop, u32 frame_width, u32 frame_height);
int cnn_set_configured_crop(u32 frame_width, u32 frame_height);
CnnCrop cnn_get_crop(void);
int cnn_run_from_frame(UINTPTR frame_addr,
		u32 frame_width,
		u32 frame_height,
		u32 frame_stride_bytes,
		CnnPixelOrder pixel_order,
		int* out_class);
int cnn_run_from_input(const s8* input_40x80, int* out_class);
void cnn_get_last_input_stats(int* min_value,
		int* max_value,
		int* sum,
		u32* checksum);
void cnn_dump_last_input_pgm(const char* name);

#endif
