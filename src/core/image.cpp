#include "image.h"

#include <iostream>

using namespace std;

static bool is_little_endian()
{
	uint32_t x = 0x76543210;
	char *c = (char*) &x;
	return (*c == 0x10);
}

Image::Image(uint8_t bit_depth,
	uint8_t pixel_size,
	uint16_t height,
	uint16_t width,
	bool little_endian)
{
	//assert_utils
	assert_util(height%2==0, "Error, height must be multiple of two");
	assert_util(width%2==0, "Error, width must be multiple of two");
	assert_util(pixel_size >= bit_depth, "Error, pixel_size < bit_depth!");
	assert_util(bit_depth >= 1 && bit_depth <= 16, "Error in image bit depth");
	assert_util(pixel_size >= 1 && pixel_size <= 16, "Error in image pixel size");

	bit_depth_ = bit_depth;
	pixel_size_ = pixel_size;
	height_ = height;
	width_ = width;
	little_endian_ = (little_endian && is_little_endian());
	allocated_bytes_ = 0;
	used_bytes_ = 0;
}

void Image::load_image(uint8_t* image)
{
	//allocate space needed
	uint32_t space_needed = pixel_size_*height_*width_/8;
	space_needed += (((pixel_size_*height_*width_)%8)==0)?0:1;
	image_ = shared_ptr<uint8_t>(new uint8_t[space_needed], default_delete<uint8_t[]>());
	uint8_t* img_ptr = image_.get();
	//copy the image
	memcpy(img_ptr, image, space_needed);
	//set other internal data
	allocated_bytes_ = space_needed;
	used_bytes_ = space_needed;
}

void Image::load_image(FILE* img_file)
{
	//allocate space needed
	uint32_t space_needed = pixel_size_*height_*width_/8;
	space_needed += (((pixel_size_*height_*width_)%8)==0)?0:1;
	image_ = shared_ptr<uint8_t>(new uint8_t[space_needed], default_delete<uint8_t[]>());
	uint8_t* img_ptr = image_.get();
	//read the image
	assert_util(fread(img_ptr, space_needed, 1, img_file) == 1,
			"Can't read full image from the disk.");
	//set other internal data
	allocated_bytes_ = space_needed;
	used_bytes_ = space_needed;
}

void Image::convert_pixel_size(uint8_t new_pixel_size, BD_Converter* converter)
{
	assert_util(used_bytes_ != 0, "No image loaded yet");
	assert_util((new_pixel_size >= 1 && new_pixel_size <= 16), "new_pixel_size is incorrect");

	//if same pixel size
	if(new_pixel_size == pixel_size_) {
		return;
	}

	//if new_pixel_size is bigger, we have to allocate new image with the new dimensions
	if(new_pixel_size > pixel_size_) {
		//we have to allocate new image with the new dimensions
		uint32_t new_space_needed = new_pixel_size*height_*width_/8;
		new_space_needed += (((new_pixel_size*height_*width_)%8)==0)?0:1;
		std::shared_ptr<uint8_t> new_img(new uint8_t[new_space_needed], default_delete<uint8_t[]>());
		//old image
		uint8_t* old_img_ptr = image_.get();
		BitIterator img_bit_iterator(old_img_ptr, height_*width_*pixel_size_);
		//new image setter
		uint8_t* new_img_ptr = new_img.get();
		BitIterator img_bit_setter(new_img_ptr, height_*width_*new_pixel_size);
		//read and assigned
		for(int i=0; i<height_; i++) {
			for(int j=0; j<width_; j++) {
				uint16_t pixel_val = img_bit_iterator.get(pixel_size_);
				img_bit_setter.set(converter->convert(pixel_val), new_pixel_size);
			}
		}
		//set the shared_ptr
		image_ = new_img;
	}

	//if new_pixel_size is smaller, we will use same object
	if(new_pixel_size < pixel_size_) {
		//image
		uint8_t* img_ptr = image_.get();
		BitIterator img_bit_iterator(img_ptr, height_*width_*pixel_size_);
		BitIterator img_bit_setter(img_ptr, height_*width_*new_pixel_size);
		//read and assigned
		for(int i=0; i<height_; i++) {
			for(int j=0; j<width_; j++) {
				uint16_t pixel_val = img_bit_iterator.get(pixel_size_);
				img_bit_setter.set(converter->convert(pixel_val), new_pixel_size);
			}
		}
	}

	uint32_t new_space_needed = new_pixel_size*height_*width_/8;
	new_space_needed += (((new_pixel_size*height_*width_)%8)==0)?0:1;

	//update object state
	allocated_bytes_ = max(allocated_bytes_, new_space_needed);
	used_bytes_ = new_space_needed;
	bit_depth_ = min(bit_depth_, new_pixel_size);
	pixel_size_ = new_pixel_size;
}

shared_ptr<uint8_t> Image::get_image(bool deep_copy)
{
	if(!deep_copy) {
		return image_;
	}
	shared_ptr<uint8_t> new_img(new uint8_t[used_bytes_], default_delete<uint8_t[]>());
	memcpy(new_img.get(), image_.get(), used_bytes_);
	return new_img;
}

uint8_t Image::get_bit_depth()
{
	return bit_depth_;
}

uint8_t Image::get_pixel_size()
{
	return pixel_size_;
}

uint16_t Image::get_height()
{
	return height_;
}

uint16_t Image::get_width()
{
	return width_;
}

uint32_t Image::get_allocated_bytes()
{
	return allocated_bytes_;
}

uint32_t Image::get_used_bytes()
{
	return used_bytes_;
}

bool Image::get_little_endian()
{
	return little_endian_;
}

uint64_t Image::get_used_bits()
{
	return height_ * width_ * pixel_size_;
}

uint64_t Image::get_pixels_num()
{
	return height_ * width_;
}