#include "bit_iterator.h"

BitIterator::BitIterator(uint8_t* data_ptr, uint64_t data_bits_num)
{
	data_ptr_ = data_ptr;
	data_bits_num_ = data_bits_num;
	curr_byte_ = 0;
	curr_bit_ = 0;
}

void BitIterator::set(uint64_t value, uint8_t num_bits)
{
	#ifdef DEBUG
		assert_util(num_bits <= 64, "Can't set more than 64 bit at a time.");
		assert_util(curr_byte_*8+curr_bit_+num_bits <= data_bits_num_, "Can't access-set more than data_bits_num.");
	#endif

	//pre-process value
	value = value << (64-num_bits);

	//fill the rest of current byte, bit by bit
	bool used = false;
	while(curr_bit_ != 0 && num_bits != 0) {
		//zero out this bit
		data_ptr_[curr_byte_] &= (0xFF^(1<<(7-curr_bit_)));
		//set the bit
		data_ptr_[curr_byte_] |= ((value>>63)<<(7-curr_bit_));

		//change data
		value = value<<1;
		num_bits--;
		curr_bit_ = (curr_bit_+1)%8;
		used = true;
	}

	//inc current byte
	if(curr_bit_ == 0 && used) {
		curr_byte_++;
	}

	//put rest of bytes
	while(num_bits >= 8) {
		data_ptr_[curr_byte_] = value >> 56;

		//change data
		value = value<<8;
		num_bits -= 8;
		curr_byte_++;
	}

	//handle rest of the bits
	if(num_bits > 0) {
		//zero out num_bits
		data_ptr_[curr_byte_] &= ((1 << (8-num_bits))-1);
		//copy num_bits to the beginning of the byte
		data_ptr_[curr_byte_] |= (value>>56);
		//change data
		curr_bit_ = num_bits;
	}
}

uint64_t BitIterator::get(uint8_t num_bits)
{
	#ifdef DEBUG
		assert_util(num_bits <= 64, "Can't set more than 64 bit at a time.");
		assert_util(curr_byte_*8+curr_bit_+num_bits <= data_bits_num_, "Can't access-get more than data_bits_num.");
	#endif

	//result
	uint64_t result = 0;

	//fill the rest of current byte, bit by bit
	bool used = false;
	while(curr_bit_ != 0 && num_bits != 0) {
		//set result
		result |= ((data_ptr_[curr_byte_] >> ((7-curr_bit_))) & 1);

		//change data
		result = result<<1;
		num_bits--;
		curr_bit_ = (curr_bit_+1)%8;
		used = true;
	}
	result = result>>1;

	//inc current byte
	if(curr_bit_ == 0 && used) {
		curr_byte_++;
	}

	//put rest of bytes
	while(num_bits >= 8) {
		//make space for new byte
		result = result<<8;

		//add data
		result |= data_ptr_[curr_byte_];

		//change data
		num_bits -= 8;
		curr_byte_++;
	}

	//handle rest of the bits
	if(num_bits > 0) {
		//make space for new bits
		result = result<<num_bits;
		//add data
		result |= (data_ptr_[curr_byte_]>>(8-num_bits));
		//change data
		curr_bit_ = num_bits;
	}

	return result;
}