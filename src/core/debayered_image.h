#ifndef SRC_CORE_DEBAYERED_IMAGE_H__
#define SRC_CORE_DEBAYERED_IMAGE_H__

/**
 * Note
 * -------------------------------
 * with pixel_size not multiple of 8-bits, the pixel is represented
 * sequentially in memory. for more see: https://wiki.apertus.org/index.php/RAW12
 */

//Declare class
class DebayeredImage;

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

class DebayeredImage : public Image{

	private:
		bool debayered_;
		uint32_t bayer_pattern_;

		template <class pixel_type>
		void debayer_nni();
		
	public:

		/**
		 * Method : constructor
		 * -------------------------------
		 * The method will construct the image object.
		 * BayerImage: the image wanted to be debayred.
		 */
		DebayeredImage(BayerImage img);


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
		 * DebayeredImage will return DEBAYERED_IMAGE
		 */
		virtual uint32_t get_image_type() override;


		/**
		 * Method : get_image
		 * -------------------------------
		 * return the processed image data.
		 * if deep_copy is true, then the returned image will be copied and will not
		 * be affected by any new processing, if false then the receiver will share same data with
		 * the object.
		 */
		shared_ptr<uint8_t> get_image(bool deep_copy) override;

		/**
		 * Methods : getters for image data
		 * -------------------------------
		 */
		using Image::get_bit_depth;
		using Image::get_pixel_size;
		using Image::get_height;
		uint16_t get_width() override;
		using Image::get_allocated_bytes;
		using Image::get_used_bytes;
		uint64_t get_used_bits() override;
		uint64_t get_pixels_num() override;
		uint8_t get_debayered_pixel_size();	//3*get_pixel_size()
};

#endif