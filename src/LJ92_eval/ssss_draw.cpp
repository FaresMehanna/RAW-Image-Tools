#include "ssss_draw.h"

using namespace std;

void draw_ssss(const vector<uint32_t>& ssss_histogram, const vector<uint32_t>& ssss_codes, const vector<uint8_t>& ssss_length)
{
	//convert hist to double values
	uint32_t total_pixels = RAW12_HEIGHT * RAW12_WIDTH;
	vector<double> ssss_histogram_d, ssss_histogram_n;
	double max_ = 0, min_ = 1;
	for(unsigned int i=0; i<ssss_histogram.size(); i++) {
		ssss_histogram_d.push_back((double) ssss_histogram[i] / total_pixels);
		max_ = max(max_, ssss_histogram_d[i]);
		min_ = min(min_, ssss_histogram_d[i]);
	}

	//do min max normalization
	for(unsigned int i=0; i<ssss_histogram_d.size(); i++) {
		ssss_histogram_n.push_back((ssss_histogram_d[i] - min_) / (max_ - min_));
	}

	//the draw
	#define DRAW_LINES (10)
	double step = max_/DRAW_LINES;
	for(unsigned int i=DRAW_LINES; i!=0; i--) {
		//draw max
		if(max_*100 < 10)	printf(" ");
		printf("  %2.2lf", (max_*100));
		cout << "%   | ";

		max_ = max_ - step;
		//draw ssss_histo
		for(unsigned int j=0; j<ssss_histogram_n.size(); j++) {
			uint8_t current = round(ssss_histogram_n[j] * 10);
			if(current >= i) {
				cout << "   |  ";
			} else {
				cout << "      ";
			}
		}
		cout << endl;
	}

	//ssss line
	cout << "   ssss    |    0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16"<< endl;

	//freq line
	cout << " frequency | ";
	for(unsigned int i=0; i<ssss_codes.size(); i++) {
		char buffer[10];
		sprintf(buffer, "%0.4lf ", ssss_histogram_d[i]);
		printf("%s", buffer+1);
	}
	cout << endl;

	//code line
	cout << "   code    | ";
	for(unsigned int i=0; i<ssss_codes.size(); i++) {
		stringstream stream;
		stream << hex << ssss_codes[i];
		string result(stream.str());
		switch(result.length()) {
			case 1:
				cout << "   " + result + "  ";
			break;
			case 2:
				cout << "  " + result + "  ";
			break;
			case 3:
				cout << "  " + result + " ";
			break;
			case 4:
				cout << " " + result + " ";
			break;
		}
	}
	cout << endl;


	//code length line
	cout << "code Length| ";
	for(unsigned int i=0; i<ssss_length.size(); i++) {
		string result = to_string(ssss_length[i]);
		switch(result.length()) {
			case 1:
				cout << "   " + result + "  ";
			break;
			case 2:
				cout << "  " + result + "  ";
			break;
		}
	}
	cout << endl;

	//bits line
	cout << "Encode Bits| ";
	for(unsigned int i=0; i<ssss_length.size(); i++) {
		
		int encoded_len = ssss_length[i]+i;
		if(encoded_len == 32) {
			encoded_len = 16;
		}

		string result = to_string(encoded_len);
		switch(result.length()) {
			case 1:
				cout << "   " + result + "  ";
			break;
			case 2:
				cout << "  " + result + "  ";
			break;
		}
	}
	cout << endl;
}