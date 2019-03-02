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

enum lib_pnm_state {unsupported_mode_of_operation,
					unsupported_bit_depth,
					memory_error,
					ok,
					}; 

//can output any king of Image objects
lib_pnm_state generate_pnm(string magic_number, Image* img, shared_ptr<uint8_t>& data, uint32_t* data_length);
lib_pnm_state generate_pnm(string magic_number, Image* img, ofstream& out_file);

#endif
