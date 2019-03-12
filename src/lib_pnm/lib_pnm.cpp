#include "lib_pnm.h"

using namespace std;

static inline string get_magic_number(Image* img, bool binary_output)
{
	if(img->get_image_type() == BAYER_IMAGE) {
		if(binary_output) {
			return "P5";
		} else {
			return "P2";
		}
	} else if (img->get_image_type() == DEBAYERED_IMAGE) {
		if(binary_output) {
			return "P6";
		} else {
			return "P3";
		}
	} else {
		return "INVALID";
	}
}

static inline vector<uint8_t> write_header(string& magic_number, Image* img)
{
	//get width and height and max value
	string height = to_string(img->get_height());
	string width = to_string(img->get_width());
	string max_val = to_string((int)pow(2, img->get_bit_depth())-1);

	//allocate space for data
	vector<uint8_t> data;

	//write magic number
	for (auto chr : magic_number) {
		data.push_back(chr);
	}
	data.push_back('\n');

	//write height & width
	for (auto chr : width) {
		data.push_back(chr);
	}
	data.push_back(' ');
	for (auto chr : height) {
		data.push_back(chr);
	}
	data.push_back('\n');

	//write max value
	for (auto chr : max_val) {
		data.push_back(chr);
	}
	data.push_back('\n');

	//return the header
	return data;
}

lib_pnm_state generate_pnm(bool binary_output, Image* img, shared_ptr<uint8_t>& data, uint32_t* data_length)
{
	//binary or ascii?
	string magic_number = get_magic_number(img, binary_output);
	if(magic_number == "INVALID") {
		return pnm_unsupported_mode_of_operation;
	}

	//handle bit depth
	if(img->get_bit_depth() > 8) {
		return pnm_unsupported_bit_depth;
	}
	//generate header for both binary and ascii
	vector<uint8_t> inner_data = write_header(magic_number, img);

	//write pixels data
	int img_size = img->get_used_bits() / img->get_pixel_size();
	BitIterator img_it(img->get_image(false).get(), img->get_used_bits());

	if(binary_output) {
		for(int i=0; i<img_size; i++) {
			inner_data.push_back(img_it.get(img->get_pixel_size()));
		}
	} else {
		for(int i=0; i<img_size; i++) {
			for (char chr : to_string(img_it.get(img->get_pixel_size()))) {
				inner_data.push_back(chr);
			}
			inner_data.push_back(' ');
		}
	}

	//write data and finish!
	shared_ptr<uint8_t> final_data(new uint8_t[inner_data.size()], [](uint8_t *p) { delete[] p; });
	data = final_data;
	memcpy(final_data.get(), &inner_data[0], inner_data.size());
	*data_length = inner_data.size();
	return pnm_ok;
}

lib_pnm_state generate_pnm(bool binary_output, Image* img, ofstream& out_file)
{
	//binary or ascii?
	string magic_number = get_magic_number(img, binary_output);
	if(magic_number == "INVALID") {
		return pnm_unsupported_mode_of_operation;
	}

	//handle bit depth
	if(img->get_bit_depth() > 8) {
		return pnm_unsupported_bit_depth;
	}
	//generate header for both binary and ascii
	for(uint8_t header_data : write_header(magic_number, img)) {
		out_file << header_data;
	}

	//write pixels data
	int img_size = img->get_used_bits() / img->get_pixel_size();
	BitIterator img_it(img->get_image(false).get(), img->get_used_bits());

	if(binary_output) {
		for(int i=0; i<img_size; i++) {
			char pixel = img_it.get(img->get_pixel_size());
			out_file.write(&pixel,1);
		}
	} else {
		for(int i=0; i<img_size; i++) {
			string pixel = to_string(img_it.get(img->get_pixel_size())) + ' ';
			out_file.write(pixel.c_str(), pixel.length());
		}
	}

	return pnm_ok;
}

lib_pnm_state pnm_file_extension(Image* img, string& file_type_out)
{
	if(img->get_image_type() == BAYER_IMAGE) {
		file_type_out = "pgm";
	} else if (img->get_image_type() == DEBAYERED_IMAGE) {
		file_type_out = "ppm";
	} else {
		return pnm_unsupported_mode_of_operation;
	}
	return pnm_ok;
}