#ifndef SRC_CORE_IMAGE_H__
#define SRC_CORE_IMAGE_H__

/**
 * Note
 * -------------------------------
 * with pixel_size not multiple of 8-bits, the pixel is represented
 * sequentially in memory. for more see: https://wiki.apertus.org/index.php/RAW12
 */

//Declare class
class Image;

//define Image types
#define BAYER_IMAGE 0x1
#define DEBAYERED_IMAGE 0x2

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <string>
#include <memory>

#include "bitdepth_converter.h"
#include "bit_iterator.h"
#include "assert.h"

using namespace std;

class Image{

	protected:
		uint8_t bit_depth_;
		uint8_t pixel_size_;
		uint16_t height_;
		uint16_t width_;
		uint32_t allocated_bytes_;
		uint32_t used_bytes_;
		bool little_endian_;
		shared_ptr<uint8_t> image_;

	public:

		/**
		 * Method : constructor
		 * -------------------------------
		 * The method will construct the image object.
		 * bit_depth: from 1 to 16 only.
		 * pixel_size: from 1 to 16 only, must be >= bit_depth.
		 * height: from 1 to (2^16)-1.
		 * width: from 1 to (2^16)-1.
		 * little_endian: is the image little endian?
		 */
		Image(uint8_t bit_depth,
				uint8_t pixel_size,
				uint16_t height,
				uint16_t width,
				bool little_endian);
		virtual ~Image(){}

		/**
		 * Methods : get_image_type
		 * -------------------------------
		 * BayerImage will return BAYER_IMAGE
		 * DebayeredImage will return DEBAYERED_IMAGE
		 */
		virtual uint32_t get_image_type() = 0;

		/**
		 * Methods : load_image
		 * -------------------------------
		 * several functions to load the image from memory or disk.
		 * Note: the image will get allocated inside the class.
		 */
		virtual void load_image(uint8_t* image);
		virtual void load_image(FILE* img_file);

		/**
		 * Method : convert_bit_depth
		 * -------------------------------
		 * Convert the internal pixel sizes.
		 * if new_pixel_size >= bit_depth, no conversion in pixels value will happen,
		 * only change in ordering and image size.
		 * if new_pixel_size <= bit_depth, conversion in pixels value will happen and
		 * converter object will be used to do the conversion.
		 * new_pixel_size: from 1 to 16 only.
		 */
		virtual void convert_pixel_size(uint8_t new_pixel_size,
									BD_Converter* converter);

		/**
		 * Method : get_image
		 * -------------------------------
		 * return the processed image data.
		 * if deep_copy is true, then the returned image will be copied and will not
		 * be affected by any new processing, if false then the receiver will share same data with
		 * the object.
		 */
		virtual shared_ptr<uint8_t> get_image(bool deep_copy);

		/**
		 * Methods : getters for image data
		 * -------------------------------
		 */
		virtual uint8_t get_bit_depth();
		virtual uint8_t get_pixel_size();
		virtual uint16_t get_height();
		virtual uint16_t get_width();
		virtual uint32_t get_allocated_bytes();
		virtual uint32_t get_used_bytes();
		virtual uint64_t get_used_bits();
		virtual uint64_t get_pixels_num();
		virtual bool get_little_endian();
};

#endif