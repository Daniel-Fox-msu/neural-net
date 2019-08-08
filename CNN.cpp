#include "CNN.h"
#include "Network.h"
#include<algorithm>
using std::find;


//convert 4 bytes into a 32 bit int (little endian)
int bytes_to_int32(std::string bytes){
	int result = int((unsigned char)(bytes[0]) |
					(unsigned char)(bytes[1]) << 8 |
					(unsigned char)(bytes[2]) << 16 |
					(unsigned char)(bytes[3]) << 24);
	return result;
}

//convert 2 bytes into a 16 bit int (little endian)
int bytes_to_int16(std::string bytes){
	int result = int((unsigned char)(bytes[0]) |
					(unsigned char)(bytes[1]) << 8 );
	return result;
}

/*
Given a file name as a char array, add each byte from the file
to a string and return that string.
*/
string get_file_contents(const char *filename)
{
	ifstream t(filename);
	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	string buffer(size, ' ');
	t.seekg(0);
	t.read(&buffer[0], size);
	return buffer;
}

//convert 2 bytes to a gb2312 chinese character
std::wstring bytes_to_gb2312(const std::string &str)
{
    int num_chars = MultiByteToWideChar(936, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstrTo;
    if (num_chars)
    {
        wstrTo.resize(num_chars);
        if (MultiByteToWideChar(936, 0, str.c_str(), str.length(), &wstrTo[0], num_chars))
        {
            return wstrTo;
        }
    }
}

/* 
takes the number of the file, returns file_data object which has members for 
the image and correct answer of a data file.
*/
file_data get_file_data(int fnum){
	
	//the data files of the CASIA offline dataset are named "1xxx-c.gnt"
	string fnum_str = std::to_string(fnum + 1000);
	string fname = fnum_str + "-c.gnt";
	const char* filename = fname.c_str();
	
	//open file with the filename and get iterator
	ifstream infile;
	infile.open(filename, std::ios::binary | std::ios::in);
	std::istreambuf_iterator<char> it (infile);
	std::istreambuf_iterator<char> eof; //end of file
	file_data result;
	
	string entry_size_b, width_b, height_b, char_b;
	vector<double> bmp;
	int entry_size, width, height, bmp_size;
	wstring correct_chard;
	wchar_t cor_ch;

	while(it != eof){ //main loop through file
	
	//next get the bytes for each data for this entry
		for (int i = 0; i < 4; i++){
			entry_size_b.push_back(*it);
			it++;
		}
		entry_size_b.clear();
		for (int i = 0; i < 2; i++){
			char_b.push_back(*it);
			it++;
		}
		for (int i = 0; i < 2; i++){
			width_b.push_back(*it);
			it++;
		}
		for (int i = 0; i < 2; i++){
			height_b.push_back(*it);
			it++;
		}
		
		//get all appropriate values from bytes
		width = bytes_to_int16(width_b);
		height = bytes_to_int16(height_b);
		correct_char_ws = bytes_to_gb2312(char_b);
		cor_ch = correct_char_ws[0];
		bmp_size = width * height;		
		
		//clear byte strings for next loop
		width_b.clear();
		height_b.clear();
		char_b.clear();

		//for each byte of the bitmap, 255 represents white and 0 represents black. All white 
		//pixels are assigned 0, while all nonwhite pixels are assigned 1.
		for (int i = 0; i < bmp_size; i++){
			if((int((unsigned char)*it)) == 255){
				bmp.push_back(0.0);
				it++;
			}
			else{
				bmp.push_back(1.0);
				it++;
			}
		}
		
		//filler so all bmp are same size, bandaid fix
		while(bmp_size < 21615){
			bmp.push_back(0);
			bmp_size++;
		}
		
		//add data to result (file_data object)
		result.correct_chars.push_back(cor_ch);
		result.img_list.push_back(bmp);
		bmp.clear();
	}
	return result;
}


//given an example (input x, ideal output y), return the difference between the two
vector<double> cost_derivative(vector<double> x, vector<double> y){
	vector<double> result;
	for (int i = 0; i < x.size(); i++){
		result.push_back(x[i] - y[i]);
	}
	return result;
}


