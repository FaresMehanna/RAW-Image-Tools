#include "debayered_image.h"

using namespace std;

DebayeredImage::DebayeredImage(BayerImage img) :
	Image(img.get_bit_depth(),
		img.get_pixel_size(),
		img.get_height(),
		img.get_width(),
		img.get_little_endian())
{
	bayer_pattern_ = img.get_bayer_pattern();
	load_image(img.get_image(false).get());
	debayered_ = false;
}

template <class type>
void DebayeredImage::debayer_nni()
{
	//save old pixel size
	int old_pixel_size = pixel_size_;
	//convert image to 16bit per pixel color
	BD_ConverterEqual BDCE;
	convert_pixel_size(sizeof(type)*8, &BDCE);
	//create new Image with 16bit per color by 3*used bytes
	shared_ptr<uint8_t> debayerd_img(new uint8_t[used_bytes_*3], default_delete<uint8_t[]>());
	//get pointers to new and old images
	type* old_img = (type*) (image_.get());
	type* new_img = (type*) (debayerd_img.get());
	uint32_t ctr = 0;

	// bayer_pattern: 
	// //  0x02010100  0x01000201  0x01020001  0x00010102
	// //      R G         G B         G R         B G
	// //      G B         R G         B G         G R
	assert_util(bayer_pattern_ == 0x02010100, "We only support RGGB for now.");

	for(int i=0; i<height_; i++) {
		for(int j=0; j<width_; j+=2) {

			bool i_even = i%2==0;

			int curr_row = (i)*width_;
			int next_row = (i+1)*width_;
			int prev_row = (i-1)*width_;

			int curr_cell = j;
			int next_cell = j+1;

			if(i_even) {
				//R
				new_img[ctr++] = old_img[curr_row+ curr_cell];
				//G
				new_img[ctr++] = old_img[curr_row+ next_cell];
				//B
				new_img[ctr++] = old_img[next_row+ next_cell];
				//R
				new_img[ctr++] = old_img[curr_row+ curr_cell];
				//G
				new_img[ctr++] = old_img[curr_row+ next_cell];
				//B
				new_img[ctr++] = old_img[next_row+ next_cell];
			} else {
				//R
				new_img[ctr++] = old_img[prev_row+ curr_cell];
				//G
				new_img[ctr++] = old_img[curr_row+ curr_cell];
				//B
				new_img[ctr++] = old_img[curr_row+ next_cell];
				//R
				new_img[ctr++] = old_img[prev_row+ curr_cell];
				//G
				new_img[ctr++] = old_img[curr_row+ curr_cell];
				//B
				new_img[ctr++] = old_img[curr_row+ next_cell];
			}

		}
	}


	//that's a wrap
	image_ = debayerd_img;
	width_ = width_ * 3;
	convert_pixel_size(old_pixel_size, &BDCE);
	debayered_ = true;
}

uint32_t DebayeredImage::get_image_type()
{
	return DEBAYERED_IMAGE;
}

shared_ptr<uint8_t> DebayeredImage::get_image(bool deep_copy)
{
	if(!debayered_) {
		if(pixel_size_ > 8) {
			debayer_nni<uint16_t>();
		} else {
			debayer_nni<uint8_t>();
		}
	}
	if(!deep_copy) {
		return image_;
	}
	std::shared_ptr<uint8_t> new_img(new uint8_t[used_bytes_], default_delete<uint8_t[]>());
	memcpy(new_img.get(), image_.get(), used_bytes_);
	return new_img;
}

uint16_t DebayeredImage::get_width()
{
	if(!debayered_) {
		return width_;
	} else {
		return width_/3;
	}
}

uint64_t DebayeredImage::get_used_bits()
{
	return (uint64_t)height_ * width_ * pixel_size_;
}

uint64_t DebayeredImage::get_pixels_num()
{
	if(!debayered_) {
		return height_ * width_;
	} else {
		return height_ * width_/3;
	}
}

uint8_t DebayeredImage::get_debayered_pixel_size()
{
	return 3*get_pixel_size();
}