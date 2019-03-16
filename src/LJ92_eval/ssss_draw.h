#ifndef SRC_LJ92_EVAL_SSSS_DRAW_H__
#define SRC_LJ92_EVAL_SSSS_DRAW_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;

#define RAW12_HEIGHT (3072)
#define RAW12_WIDTH (4096)

void draw_ssss(const vector<uint32_t>& ssss_histogram, const vector<uint32_t>& ssss_codes, const vector<uint8_t>& ssss_length);

#endif
