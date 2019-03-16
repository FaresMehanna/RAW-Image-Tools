#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <memory>

#include "../lib_pnm/lib_pnm.h"
#include "../lib_dng/lib_dng.h"
#include "../core/lj92_image.h"
#include "../core/bayer_image.h"
#include "../core/debayered_image.h"
#include "../core/bitdepth_converter.h"

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

static void print_square (BayerImage& img)
{
	uint8_t* img_data = img.get_image(false).get();
	for(int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			cout << (int) img_data[i*RAW12_WIDTH + j] << " ";
		}
		cout << endl;
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

	//create image object and load the data from the disk
	LINE(); LOG("Loading the image."); LINE();
	BayerImage img(RAW12_BIT_DEPTH,
			RAW12_PIXEL_SIZE, 
			RAW12_HEIGHT,
			RAW12_WIDTH,
			RAW12_BAYER_PATTERN,
			RAW12_LITTLE_ENDIAN);
	img.load_image(raw12_fp);

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

	//generate pgm for the main image
	{
		LOG("Creating pgm for the main image."); LINE();
		//open file for write
		string file_type;
		MAIN_ASSERT(pnm_file_extension(&img, file_type) == pnm_ok, "Error while encoding to PNM file.");
		ofstream main_image;
		string image_file_name = string(argv[1]) + "_bayer_image." + file_type;
		main_image.open(image_file_name, ios::out | ios::trunc | ios::binary);
		MAIN_ASSERT(main_image.is_open() == true, "Can't open file to write image data.");
		//generate and write to the file
		MAIN_ASSERT(generate_pnm(LIB_PNM_BINARY_OUTPUT, &img, main_image) == pnm_ok, "Error while encoding to PNM file.");
		main_image.close();
	}


	//generate pgm for the 4 channels
	for(int i=1; i<=4; i++) {
		//create channel-i
		BayerImage channel_x = img.get_channel_image(i);

		//print 5x5 of this channel
		LOG("Printing first 5x5 for channel " + to_string(i) + ".");
		print_square(channel_x);

		//open file for write
		string file_type;
		MAIN_ASSERT(pnm_file_extension(&channel_x, file_type) == pnm_ok, "Error while encoding to PNM file.");
		ofstream channel_image;
		string image_file_name = string(argv[1]) + "_channel_" + to_string(i) + "." + file_type;
		channel_image.open(image_file_name, ios::out | ios::trunc | ios::binary);
		MAIN_ASSERT(channel_image.is_open() == true, "Can't open file to write channel_image data.");

		//generate and write to the file
		LOG("Creating pgm for the channel " + to_string(i) + "."); LINE();
		MAIN_ASSERT(generate_pnm(LIB_PNM_BINARY_OUTPUT, &channel_x, channel_image) == pnm_ok, "Error while encoding to PNM file.");
		channel_image.close();
	}

	{
		//debayer the image
		DebayeredImage d_img(img);

		//open file for write
		string file_type;
		MAIN_ASSERT(pnm_file_extension(&d_img, file_type) == pnm_ok, "Error while encoding to PNM file.");
		ofstream debayered_image;
		string image_file_name = string(argv[1]) + "_debayered_image." + file_type;
		debayered_image.open(image_file_name, ios::out | ios::trunc | ios::binary);
		MAIN_ASSERT(debayered_image.is_open() == true, "Can't open file to write debayered_image data.");

		//generate ppm for the debayered image
		LOG("Debayering and creating ppm for the debayered image."); LINE();
		MAIN_ASSERT(generate_pnm(LIB_PNM_BINARY_OUTPUT, &d_img, debayered_image) == pnm_ok, "Error while encoding to PNM file.");
		debayered_image.close();
	}

	return 0;
}