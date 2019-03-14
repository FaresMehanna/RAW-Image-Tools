#include "bitdepth_converter.h"

using namespace std;


/**
 * BD_ConverterEqual
 */
BD_ConverterEqual::BD_ConverterEqual()
{
}

uint16_t BD_ConverterEqual::convert(uint16_t pixel_value)
{
	return pixel_value;
}

void BD_ConverterEqual::add_special_mapping(uint16_t from, uint16_t to)
{
	//do nothing
}

BD_ConverterEqual::~BD_ConverterEqual()
{
}


/**
 * BD_ConverterGGO
 */
BD_ConverterGGO::BD_ConverterGGO(uint8_t bit_depth_from, uint8_t bit_depth_to, double gamma, double gain, double offset)
{	
	assert_util(bit_depth_from >= 1 && bit_depth_from <= 16, "Error in image bit depth.");
	assert_util(bit_depth_to >= 1 && bit_depth_to <= 16, "Error in image bit depth.");
	
	//max values
	int max_val_from = pow(2, bit_depth_from) - 1;
	int max_val_to = pow(2, bit_depth_to) - 1;
	max_val_ = max_val_from;

	//create lut
	shared_ptr<uint16_t> temp = shared_ptr<uint16_t>(new uint16_t[max_val_from+1], [](uint16_t *p) { delete[] p; });
	lookup_table_ = temp;
	lookup_table_ptr_ = temp.get();

	//fill lut
	for(int i=0; i<max_val_from; i++) {
		lookup_table_ptr_[i] = min(max((int)round((pow((((i + offset) * gain) / max_val_from), gamma) * max_val_to)), 0), max_val_to);
	}
}

void BD_ConverterGGO::add_special_mapping(uint16_t from, uint16_t to)
{
	#ifdef DEBUG
		assert_util(from <= max_val_, "pixel_value-from bigger than max expected value.");
	#endif
	lookup_table_ptr_[from] = to;
}

uint16_t BD_ConverterGGO::convert(uint16_t pixel_value)
{
	#ifdef DEBUG
		assert_util(pixel_value <= max_val_, "pixel_value bigger than max expected value.");
	#endif
	return lookup_table_ptr_[pixel_value];
}

BD_ConverterGGO::~BD_ConverterGGO()
{
}