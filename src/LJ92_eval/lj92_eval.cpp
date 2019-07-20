#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <sstream>

#include "../lib_pnm/lib_pnm.h"
#include "../lib_dng/lib_dng.h"
#include "../core/lj92_image.h"
#include "../core/bayer_image.h"
#include "../core/debayered_image.h"
#include "../core/bitdepth_converter.h"
#include "ssss_draw.h"

using namespace std;

#define MAIN_ASSERT(exp, msg) {if(!(exp)) {std::cout << msg << std::endl; exit(1);}}
#define LOG(msg) {cout << msg << endl;}
#define LINE() {cout << "----------------------------------------" << endl;} 

#define RAW12_IMAGE_SIZE (3072*4096*12/8)
#define RAW12_BIT_DEPTH (12)
#define RAW12_PIXEL_SIZE (12)
#define RAW12_HEIGHT (3072)
#define RAW12_WIDTH (4096)
#define RAW12_BAYER_PATTERN (0x02010100)
#define RAW12_LITTLE_ENDIAN (1)
#define RAW12_GAMMA_TO_BITS (8)	//how many bits will each pixel from RAW12 converted to.

#define LJ92_HEADER true
#define LJ92_NO_HEADER false

void flush_lj92img(ofstream& img_file,LJ92Image& lj92_img, bool header)
{
	uint8_t* img = lj92_img.get_image(false).get();
	uint32_t used_bytes = lj92_img.get_used_bytes();
	int i = 0, j = used_bytes;
	if(!header) {
		i += lj92_img.get_header_size();
		j -= 2;
	}
	for(; i<j; i++) {
		img_file.write((char *)(img+i),1);
	}
}

int main(int argc, char** argv)
{
	
	//check image file is provided
	MAIN_ASSERT(argc==2, "usage: " + string(argv[0]) + " input_image.RAW12");

	//check image size
	struct stat info;
	stat(argv[1], &info);

	//check for valid size
	MAIN_ASSERT(RAW12_IMAGE_SIZE <= info.st_size, "RAW12 file is corrupted!");

	//open the image file
	FILE* raw12_fp = fopen(argv[1], "rb");
	MAIN_ASSERT(raw12_fp != NULL, "Can't read RAW12 file!");

	//print info about lj92 encoder
	LINE();
	LOG("LJ92: 4 components.");
	LOG("LJ92: predictor 1 is used.");
	LOG("LJ92: normal height and width.");
	LINE();
	LOG("LJ92: huffman codes for 12-bit image.");
	LOG("0b1110, 0b000,0b001,0b010,0b011,0b100,0b101,0b110,0b11110,0b111110,0b1111110,0b11111110,0b111111110,0b1111111110,0b11111111110,0b111111111110,0b1111111111110");
	LINE();
	LOG("LJ92: huffman codes for 8-bit image.");
	LOG("0b00,0b01,0b10,0b110,0b1110,0b11110,0b111110,0b1111110,0b11111110,0b111111110,0b1111111110,0b11111111110,0b111111111110,0b1111111111110,0b11111111111110,0b111111111111110,0b1111111111111110");
	LINE();

	//create image object and load the data from the disk
	LOG("Loading the image."); LINE();
	BayerImage img(RAW12_BIT_DEPTH,
			RAW12_PIXEL_SIZE, 
			RAW12_HEIGHT,
			RAW12_WIDTH,
			RAW12_BAYER_PATTERN,
			RAW12_LITTLE_ENDIAN);
	img.load_image(raw12_fp);
	int img_size = img.get_used_bytes();

	{
		LOG("Creating dng for the main image."); LINE();
		MAIN_ASSERT(generate_dng(&img, string(argv[1]) + "_main.dng") == dng_ok, "Error while encoding to DNG file.");
	}

	{
		LOG("Compressing 12-bits image with LJ92.");
		hufftable ssss_values;
		ssss_values.code = {0b1110, 0b000,0b001,0b010,0b011,0b100,0b101,0b110,0b11110,0b111110,0b1111110,0b11111110,0b111111110,0b1111111110,0b11111111110,0b111111111110,0b1111111111110};
		ssss_values.code_length = {4,3,3,3,3,3,3,3,5,6,7,8,9,10,11,12,13};
		LJ92Image lj92_img(img, LJ92_COMPONENTS_4, LJ92_PREDICTOR_1, ssss_values, LJ92_NORMAL_HEIGHT_WIDTH, LJ92_0XFF_FIX);
		LOG("Compression ratio: " + to_string((double)lj92_img.get_used_bytes()/img_size) + ".");
		LOG("Pixel size: " + to_string((double)lj92_img.get_used_bytes()/img_size*12) + " bits.");
		LOG("Information about four pixel patches."); LINE();
		long long sum = 0;
		for(int i=0; i<128; i++) {
			if (lj92_img.get_four_pixels_count()[i] != 0) {
				sum += lj92_img.get_four_pixels_count()[i]*4;
				LOG("Four pixels accumulated sum up to (" + to_string(i) +  "Bits) : " + to_string((((double)sum)/(RAW12_HEIGHT*RAW12_WIDTH))) + "%.");
			}
		}
		LINE(); draw_ssss(lj92_img.get_ssss_histogram(), ssss_values.code, ssss_values.code_length); LINE();
		LOG("Creating dng for the compressed main image."); LINE();
		MAIN_ASSERT(generate_dng(&lj92_img, string(argv[1]) + "_lj92.dng") == dng_ok, "Error while encoding to DNG file.");
		LOG("Generating pure (header, 0xFF fix) lj92 file."); LINE();
		ofstream main_image;
		string image_file_name = string(argv[1]) + "_header_fix.lj92";
		main_image.open(image_file_name, ios::out | ios::trunc | ios::binary);
		MAIN_ASSERT(main_image.is_open() == true, "Can't open file to write image data.");
		flush_lj92img(main_image, lj92_img, LJ92_HEADER);
		main_image.close();
		LOG("Generating headerless (no header, 0xFF fix) lj92 file."); LINE();
		ofstream nh_main_image;
		image_file_name = string(argv[1]) + "_headerless_fix.lj92";
		nh_main_image.open(image_file_name, ios::out | ios::trunc | ios::binary);
		MAIN_ASSERT(nh_main_image.is_open() == true, "Can't open file to write image data.");
		flush_lj92img(nh_main_image, lj92_img, LJ92_NO_HEADER);
		nh_main_image.close();
	}

	{
		LOG("Compressing 12-bits with no 0xFF fix image with LJ92.");
		hufftable ssss_values;
		ssss_values.code = {0b1110, 0b000,0b001,0b010,0b011,0b100,0b101,0b110,0b11110,0b111110,0b1111110,0b11111110,0b111111110,0b1111111110,0b11111111110,0b111111111110,0b1111111111110};
		ssss_values.code_length = {4,3,3,3,3,3,3,3,5,6,7,8,9,10,11,12,13};
		LJ92Image lj92_img(img, LJ92_COMPONENTS_4, LJ92_PREDICTOR_1, ssss_values, LJ92_NORMAL_HEIGHT_WIDTH, LJ92_0XFF_NO_FIX);
		LOG("Compression ratio: " + to_string((double)lj92_img.get_used_bytes()/img_size) + ".");
		LOG("Pixel size: " + to_string((double)lj92_img.get_used_bytes()/img_size*12) + " bits.");
		LOG("Generating pure (header, no 0xFF fix) lj92 file."); LINE();
		ofstream main_image;
		string image_file_name = string(argv[1]) + "_header_nofix.lj92";
		main_image.open(image_file_name, ios::out | ios::trunc | ios::binary);
		MAIN_ASSERT(main_image.is_open() == true, "Can't open file to write image data.");
		flush_lj92img(main_image, lj92_img, LJ92_HEADER);
		main_image.close();
		LOG("Generating headerless (no header, no 0xFF fix) lj92 file."); LINE();
		ofstream nh_main_image;
		image_file_name = string(argv[1]) + "_headerless_nofix.lj92";
		nh_main_image.open(image_file_name, ios::out | ios::trunc | ios::binary);
		MAIN_ASSERT(nh_main_image.is_open() == true, "Can't open file to write image data.");
		flush_lj92img(nh_main_image, lj92_img, LJ92_NO_HEADER);
		nh_main_image.close();
	}

	{
		//create optimal lut to convert to 8-bits from 12-bits.
		double gamma = 0.54115, gain = 1.02447, offset = -21.74171;
		BD_ConverterGGO optimal_lut(RAW12_BIT_DEPTH, RAW12_GAMMA_TO_BITS, gamma, gain, offset);
		//adjust the optimal lut to make use of all possible values
		optimal_lut.add_special_mapping(20, 1);
		optimal_lut.add_special_mapping(21, 2);
		optimal_lut.add_special_mapping(22, 3);
		optimal_lut.add_special_mapping(23, 4);

		//convert to 8-bits from 12-bits.
		LOG("Converting image from 12-bits to 8-bits using optimal lut."); LINE();
		img.convert_pixel_size(RAW12_GAMMA_TO_BITS, &optimal_lut);
	}

	{
		LOG("Compressing 8-bits image with LJ92.");
		hufftable ssss_values;
		ssss_values.code = {0b00,0b01,0b10,0b110,0b1110,0b11110,0b111110,0b1111110,0b11111110,0b111111110,0b1111111110,0b11111111110,0b111111111110,0b1111111111110,0b11111111111110,0b111111111111110,0b1111111111111110};
		ssss_values.code_length = {2,2,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
		LJ92Image lj92_img(img, LJ92_COMPONENTS_4, LJ92_PREDICTOR_1, ssss_values, LJ92_NORMAL_HEIGHT_WIDTH, LJ92_0XFF_FIX);
		LOG("Compression ratio: " + to_string((double)lj92_img.get_used_bytes()/img_size) + ".");
		LOG("Pixel size: " + to_string((double)lj92_img.get_used_bytes()/img_size*12) + " bits.");
		LINE(); draw_ssss(lj92_img.get_ssss_histogram(), ssss_values.code, ssss_values.code_length); LINE();
		LOG("Creating dng for the compressed 8-bits main image."); LINE();
		MAIN_ASSERT(generate_dng(&lj92_img, string(argv[1]) + "_8bits_lj92.dng") == dng_ok, "Error while encoding to DNG file.");
	}

	return 0;
}