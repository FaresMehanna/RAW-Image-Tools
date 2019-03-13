#include "lib_dng.h"

#define CAM_COLORMATRIX1                          \
   11038, 10000,    -3184, 10000,   -1009, 10000, \
   -3284, 10000,    11499, 10000,    1737, 10000, \
   -1283, 10000,     3550, 10000,    5967, 10000

struct raw_info raw_info = {
    .api_version = 1,
    .calibration_illuminant1 = 1,       // Daylight
    .color_matrix1 = {CAM_COLORMATRIX1},// camera-specific, from dcraw.c
};

static void raw_set_geometry(int width, int height, int skip_left, int skip_right, int skip_top, int skip_bottom)
{
    raw_info.width = width;
    raw_info.height = height;
    raw_info.pitch = raw_info.width * raw_info.bits_per_pixel / 8;
    raw_info.active_area.x1 = skip_left;
    raw_info.active_area.y1 = skip_top;
    raw_info.active_area.x2 = raw_info.width - skip_right;
    raw_info.active_area.y2 = raw_info.height - skip_bottom;
    raw_info.jpeg.x = 0;
    raw_info.jpeg.y = 0;
    raw_info.jpeg.width = raw_info.width - skip_left - skip_right;
    raw_info.jpeg.height = raw_info.height - skip_top - skip_bottom;
}

lib_dng_state generate_dng(Image* img, string file_name) {
    
    uint32_t bayer_pattern;

    if(img->get_image_type() == BAYER_IMAGE) {
        bayer_pattern = ((BayerImage*) img)->get_bayer_pattern();
    } else if (img->get_image_type() == LJ92_IMAGE) {
        bayer_pattern = ((LJ92Image*) img)->get_bayer_pattern();
    } else {
        return dng_unsupported_mode_of_operation;
    }

    //set raw_info data
    raw_info.buffer = img->get_image(false).get();
    raw_info.frame_size = img->get_used_bytes();
    raw_info.bits_per_pixel = img->get_pixel_size();
    raw_info.black_level = 0;
    raw_info.white_level = pow(2, img->get_pixel_size()) - 1;
    raw_info.cfa_pattern = bayer_pattern;
    raw_set_geometry(img->get_width(), img->get_height(), 0, 0, 0, 0);
    //save
	save_dng(file_name.c_str(), &raw_info, (img->get_image_type()==LJ92_IMAGE));
	return dng_ok;
}
