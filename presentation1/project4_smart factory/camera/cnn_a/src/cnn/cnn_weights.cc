#include "cnn_weights.h"

const s8 g_cnn_weights[CNN_WEIGHTS_SIZE] __attribute__((aligned(64))) = {
#include "cnn_weights_data.inc"
};
