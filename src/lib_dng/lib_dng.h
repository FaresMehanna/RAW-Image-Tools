#ifndef SRC_LIB_DNG_LIB_DNG_H__
#define SRC_LIB_DNG_LIB_DNG_H__

#include <unistd.h>
#include <math.h>

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

#include "../core/bayer_image.h"
#include "../core/bit_iterator.h"
#include "raw.h"
extern "C" {
#include "chdk-dng.h"
}

using namespace std;

enum lib_dng_state {dng_unsupported_mode_of_operation,
					dng_unsupported_bit_depth,
					dng_memory_error,
					dng_ok,
					}; 

//can output Bayer Image objects.
lib_dng_state generate_dng(BayerImage* img, string file_name);

#endif
