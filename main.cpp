#include "Network.cpp"
#include "CNN.cpp"
#include<set>
using std::set;
using std::cout;
using std::vector;
using std::wcout;

/*
get the small subset of the data that was prepared
*/
file_data get_small_data(){
	file_data result;
	ifstream data_file;
	data_file.open("small.gnt", std::ios::binary | std::ios::in);
	std::istreambuf_iterator<char> it (data_file);
	std::istreambuf_iterator<char> eof;
	int pixel;
	string char_b;
	wstring correct_char_ws;
	wchar_t correct_char;
	vector<double> bmp;
	while (it != eof){
		for (int i = 0; i < 2; i++){
			char_b.push_back(*it);
			it++;
		}
		for (int i = 0; i < 900; i++){
			pixel = int((unsigned char)(*it));
			bmp.push_back(pixel);
			it++;
		}
		correct_char_ws = bytes_to_gb2312(char_b);
		correct_char = correct_char_ws[0];
		result.correct_chars.push_back(correct_char);
		result.img_list.push_back(bmp);
		
		bmp.clear();
		char_b.clear();
	}
	return result;
}

int main(){

	vector<int> sizes {900, 500, 50};
	
    file_data data2 = get_small_data();
	
	
	
	vector<wchar_t> chars_set;
	for(int i = 0; i < 50; i++){
		chars_set.push_back(data2.correct_chars[i]);
	}

	

	Network net = Network(sizes);
	vector<vector<double>> ys;
	vector<double> y;
	int batch_size;
	for (int f = 0; f < 3; f++){
	for (int i = 1; i < 11; i++){
		net.evaluate(data2.img_list, data2.correct_chars, chars_set);

		for (int j = 0; j < data2.img_list.size(); j++){
			for (int k = 0; k < 50; k++){
				if(data2.correct_chars[j] == chars_set[k]){
					y.push_back(1.0);
					

				}
				else{
					y.push_back(0.0);
				}
			}
			ys.push_back(y);
			y.clear();
		}
		net.update(data2.img_list, ys, 25.0);
		cout << "file " << i << " done" << endl;
	}

	}

	

	
}
