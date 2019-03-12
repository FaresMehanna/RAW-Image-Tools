#ifndef SRC_LIB_PNM_LIB_PNM_H__
#define SRC_LIB_PNM_LIB_PNM_H__

#include <unistd.h>

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

#include "../core/image.h"
#include "../core/bit_iterator.h"

using namespace std;

#define LIB_PNM_BINARY_OUTPUT (true)
#define LIB_PNM_ASCII_OUTPUT (false)


enum lib_pnm_state {pnm_unsupported_mode_of_operation,
					pnm_unsupported_bit_depth,
					pnm_memory_error,
					pnm_ok,
					}; 

//can output any kind of Image objects.
lib_pnm_state generate_pnm(bool binary_output, Image* img, shared_ptr<uint8_t>& data, uint32_t* data_length);
lib_pnm_state generate_pnm(bool binary_output, Image* img, ofstream& out_file);

//file_type_out will have the correct file extension output for the given image. 
lib_pnm_state pnm_file_extension(Image* img, string& file_type_out);

#endif
