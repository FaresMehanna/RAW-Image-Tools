#include "bayer_image.h"

using namespace std;

BayerImage::BayerImage(uint8_t bit_depth,
					uint8_t pixel_size,
					uint16_t height,
					uint16_t width,
					uint32_t bayer_pattern,
					bool little_endian) :
	Image(bit_depth, pixel_size, height, width, little_endian)
{
	assert_util((bayer_pattern == 0x02010100 || 
				 bayer_pattern == 0x01000201 || 
				 bayer_pattern == 0x01020001 || 
				 bayer_pattern == 0x00010102 ||
				 bayer_pattern == 0x02020202 ||
				 bayer_pattern == 0x01010101 ||
				 bayer_pattern == 0x00000000), "Error in image bayer pattern");
	bayer_pattern_ = bayer_pattern;
}

uint32_t BayerImage::get_image_type()
{
	return BAYER_IMAGE;
}


BayerImage BayerImage::get_channel_image(uint8_t channel_number)
{
	assert_util(channel_number >= 1 && channel_number <= 4, "Error in channel_number in BayerImage::get_channel");
	assert_util((bayer_pattern_ == 0x02010100 || 
		   bayer_pattern_ == 0x01000201 || 
		   bayer_pattern_ == 0x01020001 || 
		   bayer_pattern_ == 0x00010102), "Can't get channel from single channel image!");


	uint8_t channel_code = (bayer_pattern_ >> (8 * (4-channel_number))) & 0xFF;
	uint32_t channel_bayer = (channel_code << 24) |
							 (channel_code << 16) |
							 (channel_code << 8) |
							 (channel_code);

	BayerImage single_channel_image(bit_depth_,
					pixel_size_,
					height_/2,
					width_/2,
					channel_bayer,
					little_endian_);
	// copy the image
	ChannelIterator chn_it(*this, channel_number);
	single_channel_image.load_image(chn_it.get_channel_data(false).get());
	return single_channel_image;	
}

ChannelIterator BayerImage::get_channel_iterator(uint8_t channel_number)
{
	assert_util(channel_number >= 1 && channel_number <= 4, "Error in channel_number in BayerImage::get_channel");
	assert_util((bayer_pattern_ == 0x02010100 || 
		   bayer_pattern_ == 0x01000201 || 
		   bayer_pattern_ == 0x01020001 || 
		   bayer_pattern_ == 0x00010102), "Can't get channel from single channel image!");


	uint8_t channel_code = (bayer_pattern_ >> (8 * (4-channel_number))) & 0xFF;
	uint32_t channel_bayer = (channel_code << 24) |
							 (channel_code << 16) |
							 (channel_code << 8) |
							 (channel_code);

	BayerImage single_channel_image(bit_depth_,
					pixel_size_,
					height_/2,
					width_/2,
					channel_bayer,
					little_endian_);
	// copy the image
	return ChannelIterator(*this, channel_number);
}

uint32_t BayerImage::get_bayer_pattern()
{
	return bayer_pattern_;
}