#ifndef SRC_CORE_BAYER_IMAGE_H__
#define SRC_CORE_BAYER_IMAGE_H__

/**
 * Note
 * -------------------------------
 * with pixel_size not multiple of 8-bits, the pixel is represented
 * sequentially in memory. for more see: https://wiki.apertus.org/index.php/RAW12
 */

//Declare class
class BayerImage;

#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <algorithm>
#include <string>
#include <memory>

#include "bitdepth_converter.h"
#include "channel_iterator.h"
#include "bit_iterator.h"
#include "assert.h"
#include "image.h"

using namespace std;

class BayerImage : public Image{

	private:
		uint32_t bayer_pattern_;

	public:

		/**
		 * Method : constructor
		 * -------------------------------
		 * The method will construct the image object.
		 * bit_depth: from 1 to 16 only.
		 * pixel_size: from 1 to 16 only, must be >= bit_depth.
		 * height: from 1 to (2^16)-1.
		 * width: from 1 to (2^16)-1.
		 * bayer_pattern: 
		 *	//  0x02010100  0x01000201  0x01020001  0x00010102
		 *	//      R G         G B         G R         B G
		 *	//      G B         R G         B G         G R
		 *	//  0x02020202  0x01010101  0x00000000
		 *	//      R R         G G         B B
		 *	//      R R         G G         B B
		 * little_endian: is the image little endian?
		 */
		BayerImage(uint8_t bit_depth,
				uint8_t pixel_size,
				uint16_t height,
				uint16_t width,
				uint32_t bayer_pattern,
				bool little_endian);

		/**
		 * Methods : load_image
		 * -------------------------------
		 * several functions to load the image from memory or disk.
		 * Note: the image will get allocated inside the class.
		 */
		using Image::load_image;

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
		using Image::convert_pixel_size;

		/**
		 * Methods : get_image_type
		 * -------------------------------
		 * BayerImage will return 0x1
		 */
		virtual uint32_t get_image_type() override;

		
		/**
		 * Method : ChannelIterator
		 * -------------------------------
		 * retrieve channel iterator for specific channel,
		 * when deep_copy = false, iterators are used to save ram, so whenever you do change in the
		 * image class, that will make any initiated iterator useless and you must
		 * create new iterator.
		 * Note: this method can only be called with images bayer-patterns:
		 * 			0x02010100  0x01000201  0x01020001  0x00010102
		 */
		BayerImage get_channel_image(uint8_t channel_number);
		ChannelIterator get_channel_iterator(uint8_t channel_number);
		

		/**
		 * Method : get_image
		 * -------------------------------
		 * return the processed image data.
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
		using Image::get_pixel_size;
		using Image::get_height;
		using Image::get_width;
		using Image::get_allocated_bytes;
		using Image::get_used_bytes;
		using Image::get_used_bits;
		using Image::get_pixels_num;
		
		uint32_t get_bayer_pattern();
};

#endif