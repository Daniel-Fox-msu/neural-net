#pragma once
#include <fstream>
using std::ifstream;
#include <math.h>  
#include <iostream>
using std::endl;
#include <fstream>
#include <sstream>
#include <string>
using std::to_string;
using std::string;
using std::wstring;
#include <cstdint>
#include <vector>
using std::vector;
#include <utility>
using std::pair;
#include <cstdio>
#include <cerrno>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>

double sigm(double in){
	return (1)/(1 + (exp(-in)));
}

double sig_prime(double in){
	return (sigm(in) * (1-sigm(in)));
}

//convert 4 bytes into a 32 bit int (little endian)
int bytes_to_int32(std::string bytes);

//convert 2 bytes into a 16 bit int (little endian)
int bytes_to_int16(std::string bytes);

/*
Given a file name as a char array, add each byte from the file
to a string and return that string.
*/
string get_file_contents(const char *filename);

//convert 2 bytes to a gb2312 chinese character
std::wstring bytes_to_gb2312(const std::string &str);

vector<double> cost_derivative (vector<double> actual_output, 
								vector<double> desired_output);

struct file_data{
	vector<vector<double>> img_list;
	vector<wchar_t> correct_chars;
	int biggest_sz = 0;
	void clear(){
		img_list.clear();
		correct_chars.clear();
	}
	file_data() = default;
};
//returns a file_data object with data from the given file number
file_data get_file_data(int fnum);
//vector<file_data> get_multiple_file_data(int start, int end);
