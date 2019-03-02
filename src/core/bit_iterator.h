#ifndef SRC_CORE_BIT_ITERATOR_H__
#define SRC_CORE_BIT_ITERATOR_H__

//Declare class
class BitIterator;

#include <unistd.h>

#include "assert.h"

using namespace std;

class BitIterator{
	private:
		uint8_t* data_ptr_;
		uint64_t data_bits_num_;
		uint64_t curr_byte_;
		uint8_t curr_bit_;
	public:
		BitIterator(uint8_t* data_ptr, uint64_t data_bits_num);
		void set(uint64_t value, uint8_t num_bits);
		uint64_t get(uint8_t num_bits);
};

#endif
