#ifndef SRC_CORE_BITDEPTH_CONVERTER_H__
#define SRC_CORE_BITDEPTH_CONVERTER_H__

//Declare classes
class BD_Converter;
class BD_ConverterEqual;
class BD_ConverterGGO;

#include <unistd.h>
#include <math.h>

#include <algorithm>
#include <memory>

#include "assert.h"

/**
 * base class.
 */
class BD_Converter{
	public:
		BD_Converter(){}
		virtual uint16_t convert(uint16_t pixel_value) = 0;
		virtual void add_special_mapping(uint16_t from, uint16_t to) = 0;
		virtual ~BD_Converter(){}
};

/**
 * return same value, used when increasing pixel size.
 */
class BD_ConverterEqual : public BD_Converter{
	public:
		BD_ConverterEqual();
		virtual uint16_t convert(uint16_t pixel_value) override;
		virtual void add_special_mapping(uint16_t from, uint16_t to) override;
		virtual ~BD_ConverterEqual();
};

/**
 * gamma = 0.5
 * gain = 0.85
 * offset = 50
 * for more: https://files.apertus.org/AXIOM-Beta/optimal_curve.html
 * gamma_lut = min(max(((((x + offset) .* gain) / 4095) .^ gamma) * 255, 0), 255);
 */
class BD_ConverterGGO : public BD_Converter{
	private:
		std::shared_ptr<uint16_t> lookup_table_;
		uint16_t* lookup_table_ptr_;
		uint16_t max_val_;
	public:
		BD_ConverterGGO(uint8_t bit_depth_from, uint8_t bit_depth_to, double gamma, double gain, double offset);
		virtual uint16_t convert(uint16_t pixel_value) override;
		virtual void add_special_mapping(uint16_t from, uint16_t to) override;
		virtual ~BD_ConverterGGO();
};

#endif