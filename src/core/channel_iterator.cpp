#include "channel_iterator.h"

using namespace std;

ChannelIterator::ChannelIterator(BayerImage& img, uint8_t channel_number) :
	img_(img),
	img_b_it_(img.get_image(false).get(), img.get_used_bits())
{

	assert_util(channel_number >= 1 && channel_number <= 4, "channel_number must be from 1 to 4.")
	channel_number_ = channel_number;

	curr_ptr_ = 0;

	single_channel_ = img_.get_bayer_pattern() == 0x00000000 ||
			img_.get_bayer_pattern() == 0x01010101 ||
			img_.get_bayer_pattern() == 0x02020202;

	reset_to_begining();
}

void ChannelIterator::reset_to_begining()
{
	//update bit iterator object
	img_b_it_ = BitIterator(img_.get_image(false).get(), img_.get_used_bits());

	//update pointer
	curr_ptr_ = 0;

	//if single channel, we are done
	if(single_channel_) {
		return;
	}

	int ctr;
	switch(channel_number_) {
		case 1:
			//nothing
		break;
		case 2:
			//read single pixel
			img_b_it_.get(img_.get_pixel_size());
		break;
		case 3:
			//read image_width pixels
			ctr = img_.get_width();
			while(ctr--) {
				img_b_it_.get(img_.get_pixel_size());
			}
		break;
		case 4:
			//read image_width+1 pixels
			ctr = img_.get_width()+1;
			while(ctr--) {
				img_b_it_.get(img_.get_pixel_size());
			}
		break;
	}
}


//get full channel data
shared_ptr<uint8_t> ChannelIterator::get_channel_data(bool deep_copy)
{
	if(!deep_copy) {
		//if current image only containing single channel
		if (single_channel_) {
			return img_.get_image(false);
		}
	}

	if(deep_copy) {
		//if current image only containing single channel
		if (single_channel_) {
			return img_.get_image(true);
		}
	}

	//if deep copy enabled or the image contain more than single component then,
	//we have to deep_copy it.
	uint32_t space_needed;
	space_needed = img_.get_used_bytes()/4 + (img_.get_used_bytes()%4==0?0:1);
	shared_ptr<uint8_t> img(new uint8_t[space_needed], default_delete<uint8_t[]>());
	BitIterator img_bit_iterator(img.get(), img_.get_used_bits()/4);

	//copy all data
	ChannelIterator temp_it(img_, channel_number_);
	while(!temp_it.is_end()) {
		img_bit_iterator.set(temp_it.get_current_go_next(), img_.get_pixel_size());
	}

	return img;
}

bool ChannelIterator::is_end()
{
	//if single channel, all image is read
	if(single_channel_) {
		return curr_ptr_ == img_.get_width()*img_.get_height();
	}
	return curr_ptr_ == img_.get_width()*img_.get_height()/4;	
}

uint16_t ChannelIterator::get_current_go_next()
{
	//get current
	uint16_t pix_val = img_b_it_.get(img_.get_pixel_size());
	curr_ptr_++;

	//if single channel, then end here
	if(single_channel_ || is_end()) {
		return pix_val;
	}

	//skip following channel
	img_b_it_.get(img_.get_pixel_size());

	//skip row if in last col in current row
	if(curr_ptr_%(img_.get_width()/2) == 0) {
		int ctr = img_.get_width();
		while(ctr--) {
			img_b_it_.get(img_.get_pixel_size());
		}
	}
	
	//return data
	return pix_val;
}

BayerImage& ChannelIterator::get_img()
{
	return img_;
}

uint8_t ChannelIterator::get_channel_number()
{
	return channel_number_;
}