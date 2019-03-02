#ifndef SRC_CORE_CHANNEL_ITERATOR_H__
#define SRC_CORE_CHANNEL_ITERATOR_H__

//Declare class
class ChannelIterator;

#include <unistd.h>

#include <memory>

#include "bayer_image.h"
#include "bit_iterator.h"
#include "assert.h"


class ChannelIterator{

	private:
		BayerImage& img_;
		uint8_t channel_number_;

		bool single_channel_;
		uint32_t curr_ptr_;
		BitIterator img_b_it_;

	public:
		
		ChannelIterator(BayerImage& img, uint8_t channel_number);

		/**
		 * Note: deep_copy here is only a hint to the object to not copy
		 * the data, but in most cases full channel data will get deep-copied
		 * whether deep_copy is true or false.
		 */
		shared_ptr<uint8_t> get_channel_data(bool deep_copy);

		//iterator functions
		bool is_end();
		uint16_t get_current_go_next();
		void reset_to_begining();

		//general data getter
		uint8_t get_channel_number();
		BayerImage& get_img();
};

#endif