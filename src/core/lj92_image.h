#ifndef SRC_CORE_LJ92_IMAGE_H__
#define SRC_CORE_LJ92_IMAGE_H__

//Declare class
class LJ92Image;

#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <memory>
#include <vector>

#include "bitdepth_converter.h"
#include "channel_iterator.h"
#include "bit_iterator.h"
#include "assert.h"
#include "image.h"

using namespace std;

#define LJ92_PREDICTOR_1 (1)
#define LJ92_PREDICTOR_2 (2)
#define LJ92_PREDICTOR_3 (3)
#define LJ92_PREDICTOR_4 (4)
#define LJ92_PREDICTOR_5 (5)
#define LJ92_PREDICTOR_6 (6)
#define LJ92_PREDICTOR_7 (7)

#define LJ92_COMPONENTS_1 (1)
#define LJ92_COMPONENTS_2 (2)
#define LJ92_COMPONENTS_3 (3)
#define LJ92_COMPONENTS_4 (4)

#define LJ92_0XFF_FIX (true)
#define LJ92_0XFF_NO_FIX (false)


#define LJ92_NORMAL_HEIGHT_WIDTH (true)
#define LJ92_TRICK_HEIGHT_WIDTH (false)

struct hufftable{
	vector<uint32_t> code;
	vector<uint8_t> code_length;
};

class LJ92Image : public Image{

	private:
		int num_components_;
		int predictor_;
		hufftable ssss_values_;
		uint32_t bayer_pattern_;
		uint32_t header_size_;
		bool xff_fix_;
		vector<uint32_t> ssss_histogram_;
		vector<uint32_t> four_pixels_count_;

		vector<uint8_t> generate_header();
		template <class pixel_type> void compress(bool normal_dimensions);
		
	public:

		/**
		 * Method : constructor
		 * -------------------------------
		 * The method will construct the image object.
		 * BayerImage: the image wanted to be compressed.
		 * num_components: in the image.
		 * predictor: LJ92 predictor used.
		 * normal_dimensions: if LJ92_NORMAL_HEIGHT_WIDTH, then no change will happen,
		 * if LJ92_TRICK_HEIGHT_WIDTH, then encoder will treat width*2, height/2.
		 */
		LJ92Image(BayerImage img, int num_components, int predictor,
		 hufftable ssss_values, bool normal_dimensions, bool ones_fix);


		/**
		 * Method : convert_bit_depth
		 * -------------------------------
		 * NOT SUPPORTED.
		 * SHOULD NEVER CALLED.
		 */
		using Image::convert_pixel_size;

		/**
		 * Methods : get_image_type
		 * -------------------------------
		 * LJ92Image will return LJ92_IMAGE
		 */
		virtual uint32_t get_image_type() override;

		/**
		 * Method : get_image
		 * -------------------------------
		 * return the compressed image data.
		 * if deep_copy is true, then the returned image will be copied and will not
		 * be affected by any new processing, if false then the receiver will share same data with
		 * the object.
		 */
		using Image::get_image;

		/**
		 * Methods : getters for image data
		 * -------------------------------
		 */
		using Image::get_bit_depth;
		uint8_t get_pixel_size() override;	//always = get_bit_depth
		using Image::get_height;
		using Image::get_width;
		using Image::get_allocated_bytes;
		using Image::get_used_bytes;
		uint64_t get_used_bits() override;	//always = get_used_bytes*8
		uint32_t get_bayer_pattern();
		uint32_t get_header_size();
		vector<uint32_t> get_ssss_histogram();
		vector<uint32_t> get_four_pixels_count();
};

#endif