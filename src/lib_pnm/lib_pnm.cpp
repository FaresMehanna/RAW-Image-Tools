#include "lib_pnm.h"

using namespace std;

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

lib_pnm_state generate_pnm(string magic_number, Image* img, shared_ptr<uint8_t>& data, uint32_t* data_length)
{
	//binary or ascii?
	bool binary;
	if(magic_number == "P2" || magic_number == "P3") {
		binary = false;
	} else if(magic_number == "P5" || magic_number == "P6") {
		binary = true;
	} else {
		return unsupported_mode_of_operation;
	}

	//handle bit depth
	if(img->get_bit_depth() > 8) {
		return unsupported_bit_depth;
	}
	//generate header for both binary and ascii
	vector<uint8_t> inner_data = write_header(magic_number, img);

	//write pixels data
	int img_size = img->get_used_bits() / img->get_pixel_size();
	BitIterator img_it(img->get_image(false).get(), img->get_used_bits());

	if(binary) {
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
	return ok;
}

lib_pnm_state generate_pnm(string magic_number, Image* img, ofstream& out_file)
{
	//binary or ascii?
	bool binary;
	if(magic_number == "P2" || magic_number == "P3") {
		binary = false;
	} else if(magic_number == "P5" || magic_number == "P6") {
		binary = true;
	} else {
		return unsupported_mode_of_operation;
	}

	//handle bit depth
	if(img->get_bit_depth() > 8) {
		return unsupported_bit_depth;
	}
	//generate header for both binary and ascii
	for(uint8_t header_data : write_header(magic_number, img)) {
		out_file << header_data;
	}

	//write pixels data
	int img_size = img->get_used_bits() / img->get_pixel_size();
	BitIterator img_it(img->get_image(false).get(), img->get_used_bits());

	if(binary) {
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

	return ok;
}