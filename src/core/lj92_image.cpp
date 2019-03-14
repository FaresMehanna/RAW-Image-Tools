#include "lj92_image.h"

using namespace std;

LJ92Image::LJ92Image(BayerImage img, int num_components, int predictor, hufftable ssss_values, bool normal_dimensions) :
	Image(img.get_bit_depth(),
		img.get_pixel_size(),
		img.get_height(),
		img.get_width(),
		img.get_little_endian())
{
	assert_util(num_components >= 1 && num_components <= 4, "num_components must be from 1 to 4.");
	assert_util(predictor >= 1 && predictor <= 7, "predictor must be from 1 to 7.");
	assert_util(ssss_values.code.size() == 17, "hufftable must handle all 17 possible values.");
	assert_util(ssss_values.code_length.size() == 17, "hufftable must handle all 17 possible values.");

	num_components_ = num_components;
	predictor_ = predictor;
	ssss_values_ = ssss_values;
	bayer_pattern_ = img.get_bayer_pattern();

	load_image(img.get_image(false).get());
	
	if(img.get_bit_depth() > 8) {
		compress<uint16_t>(normal_dimensions);
	} else {
		compress<uint8_t>(normal_dimensions);
	}
}


vector<uint8_t> LJ92Image::generate_header()
{
	
	vector<uint8_t> header;
	#define ADD_HEADER(data) header.push_back(data)

	//SOI
	ADD_HEADER(0xff);	ADD_HEADER(0xd8);

	//HUFF
	ADD_HEADER(0xff);	ADD_HEADER(0xc4);
	ADD_HEADER(0x0);	ADD_HEADER(36);	//length
	ADD_HEADER(0x0);	//table id

	vector<int> ssss_order;
	for(int i=1; i<17; i++) {	//generate bits vector
		int count = 0;
		for(int j=0; j<17; j++) {
			if(ssss_values_.code_length[j] == i) {
				ssss_order.push_back(j);
				count++;
			}
		}
		ADD_HEADER(count);
	}
	for(int i=0; i<17; i++) {	//generate huffval vector
		ADD_HEADER(ssss_order[i]);
	}

	//SOF3
	ADD_HEADER(0xff);	ADD_HEADER(0xc3);
	ADD_HEADER(0x0);	ADD_HEADER(8+num_components_*3);	//length
	ADD_HEADER(bit_depth_);
	ADD_HEADER(height_>>8);	ADD_HEADER(height_&0xFF);
	ADD_HEADER((width_/num_components_)>>8);	ADD_HEADER((width_/num_components_)&0xFF);
	ADD_HEADER(num_components_);
	for(int i=0; i<num_components_; i++){
		ADD_HEADER(i);
		ADD_HEADER(0x11);
		ADD_HEADER(0);
	}

	//SOS
	ADD_HEADER(0xff);	ADD_HEADER(0xda);
	ADD_HEADER(0x0);	ADD_HEADER(6+num_components_*2);	//length
	ADD_HEADER(num_components_);
	for(int i=0; i<num_components_; i++){
		ADD_HEADER(i);
		ADD_HEADER(0);
	}
	ADD_HEADER(predictor_);
	ADD_HEADER(0);
	ADD_HEADER(0);

	return header;
}


template <class type> void LJ92Image::compress(bool normal_dimensions)
{
	//save old pixel size
	int old_bit_depth = bit_depth_;

	//convert image to x-bits per pixel color
	BD_ConverterEqual BDCE;
	convert_pixel_size(sizeof(type)*8, &BDCE);

	//create new Image with x-bit per color by 2*used bytes
	uint8_t safe_factor = 4;
	shared_ptr<uint8_t> compressed_img(new uint8_t[used_bytes_*safe_factor], default_delete<uint8_t[]>());

	//fix dimensions
	if(!normal_dimensions) {
		height_ = height_/2;
		width_ = width_ * 2;
	}

	//generate header
	vector<uint8_t> header = generate_header();

	//get pointers to new and old images
	type* old_img = (type*) (image_.get());
	type* new_img = (type*) (compressed_img.get());
	uint8_t* new_img_8ptr = (uint8_t*) new_img;
	memset(new_img, 0, used_bytes_*safe_factor);
	BitIterator compressed_img_it(new_img_8ptr+(safe_factor/2)*used_bytes_, used_bytes_*(safe_factor/2)*8);

	uint64_t used_bits = 0, ctr = 0;
	uint8_t ssss;
	int32_t Px, diff, X, Ra, Rb, Rc, half;

	//compress
	for(int row=0; row<height_; row++) {
		for(int col=0; col<width_; col++) {

			X = old_img[row * width_ + col];

			if(col >= num_components_) {
				Ra = old_img[row * width_ + col - num_components_];
			}
			
			if(row != 0) {
				Rb = old_img[(row-1) * width_ + col];
				Rc = old_img[(row-1) * width_ + col - num_components_];
			}

			if ((row == 0)&&(col < num_components_)) {
				Px = 1 << (old_bit_depth-1);
			}
			else if (row == 0) {
				Px = Ra;
			}
			else if (col < num_components_) {
				Px = Rb;
			}
			else {
				switch(predictor_) {
					case LJ92_PREDICTOR_1:
						Px = Ra;
					break;
					case LJ92_PREDICTOR_2:
						Px = Rb;
					break;
					case LJ92_PREDICTOR_3:
						Px = Rc;
					break;
					case LJ92_PREDICTOR_4:
						Px = Ra + Rb - Rc;
					break;
					case LJ92_PREDICTOR_5:
						Px = Ra + ((Rb - Rc)/2);
					break;
					case LJ92_PREDICTOR_6:
						Px = Rb + ((Ra - Rc)/2);
					break;
					case LJ92_PREDICTOR_7:
						Px = ((Ra + Rb)/2);
					break;
					default:	//should never happen
						Px = 0;
					break;
				}
			}

			//get diff
			diff = (X - Px) % 65536;

			//get ssss
			ssss = 32 - __builtin_clz(abs(diff));
			if (diff == 0){
				ssss = 0;
			}

			//normalize negative values
			half = ssss>0?(1<<(ssss-1)):0;
			if (diff < half){
				diff += (1 << (ssss))-1;
			}

			//write ssss values
			compressed_img_it.set(ssss_values_.code[ssss], ssss_values_.code_length[ssss]);
			used_bits += ssss_values_.code_length[ssss];
			if(ssss > 0 && ssss < 16) {
				compressed_img_it.set(diff, ssss);
				used_bits += ssss;
			}
		}
	}

	if(!normal_dimensions) {
		height_ = height_*2;
		width_ = width_/2;
	}

	//put header
	for(unsigned int i=0; i<header.size(); i++) {
		new_img_8ptr[ctr++] = header[i];
	}

	//fix 0xFF bytes
	uint32_t used_bytes = used_bits/8 + ((used_bits%8)==0?0:1);
	for(unsigned int i=0; i<used_bytes; i++) {
		if(new_img_8ptr[(safe_factor/2) * used_bytes_ + i] == 0xFF) {
			new_img_8ptr[ctr++] = 0xFF;
			new_img_8ptr[ctr++] = 0x00;
		} else {
			new_img_8ptr[ctr++] = new_img_8ptr[(safe_factor/2) * used_bytes_ + i];	
		}
	}

	//put end EOI
	new_img_8ptr[ctr++] = 0xff;
	new_img_8ptr[ctr++] = 0xd9;

	//that's a wrap
	image_ = compressed_img;
	bit_depth_ = old_bit_depth;
	pixel_size_ = old_bit_depth;
	allocated_bytes_ = used_bytes_*safe_factor;
	used_bytes_ = ctr;
}

uint32_t LJ92Image::get_image_type()
{
	return LJ92_IMAGE;
}

uint8_t LJ92Image::get_pixel_size()
{
	return get_bit_depth();
}

uint64_t LJ92Image::get_used_bits()
{
	return get_used_bytes() * 8;
}

uint32_t LJ92Image::get_bayer_pattern()
{
	return bayer_pattern_;
}