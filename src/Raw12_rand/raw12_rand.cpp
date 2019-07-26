#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <sstream>

using namespace std;

#define RAW12_IMAGE_SIZE (3072*4096*12/8)
#define RAND_ARRAY_SIZE (8192)
#define RAND_FACTOR (64)

#define MAIN_ASSERT(exp, msg) {if(!(exp)) {std::cout << msg << std::endl; exit(1);}}
#define LOG(msg) {cout << msg << endl;}
#define LINE() {cout << "----------------------------------------" << endl;} 

int main(int argc, char** argv)
{
	
	//check image file is provided
	MAIN_ASSERT(argc==2, "usage: " + string(argv[0]) + " input_image.RAW12");

	//check image size
	struct stat info;
	stat(argv[1], &info);

	//check for valid size
	uint32_t img_size = info.st_size;
	MAIN_ASSERT(RAW12_IMAGE_SIZE <= img_size, "RAW12 file is corrupted!");

	//open the image file
	FILE* raw12_fp = fopen(argv[1], "rb");
	MAIN_ASSERT(raw12_fp != NULL, "Can't read RAW12 file!");

	//read the image
	LOG("Read RAW12 image.");
	uint32_t* img_ptr = (uint32_t*) malloc(img_size);	//allocate space needed
	MAIN_ASSERT(fread(img_ptr, img_size, 1, raw12_fp) == 1,
			"Can't read full image from the disk.");

	// set random data
	LOG("Set random array.");
	srand(time(0));
	uint32_t random_array[RAND_ARRAY_SIZE];
	for(int i=0; i<RAND_ARRAY_SIZE; i++) {
		random_array[i] = (((uint32_t)rand()%256) << 24) |
						  (((uint32_t)rand()%256) << 16) |
						  (((uint32_t)rand()%256) << 8) |
						  (((uint32_t)rand()%256));
	}

	// 196,608 for RAND_FACTOR=64
	uint32_t random_size = RAW12_IMAGE_SIZE/RAND_FACTOR;

	// random start
	uint32_t start = random_array[0] % ((RAW12_IMAGE_SIZE/4) - random_size);
	LOG("Random start: " + to_string(start));
	uint32_t end = start + random_size;

	LOG("Random Raw12 file.");
	for(int i=start; i<end; i++) {
		img_ptr[i] = random_array[rand()%RAND_ARRAY_SIZE];
	}

	// write to file
	LOG("Write RAW12 image.");
	string file_w = string(argv[1]) + string(".random");
	FILE* raw12_fw = fopen(file_w.c_str(), "wb");
	MAIN_ASSERT(raw12_fw != NULL, "Can't write RAW12 file!");
	MAIN_ASSERT(fwrite(img_ptr, img_size, 1, raw12_fw) == 1,
			"Can't write full image to the disk.");


	return 0;
}