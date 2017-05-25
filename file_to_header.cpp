
/*
clear && g++ -O3 -std=c++14 file_to_header.cpp -o file_to_header && ./file_to_header
*/

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>

using namespace std;

int main() {
	ifstream in_file("example.rar", std::ifstream::ate | std::ifstream::binary);
	char data;

	ofstream o_file("unrar/example.h", std::ifstream::binary);
	o_file << "#include <cstdint>" << endl;
	o_file << "const size_t rar_file_data_length = " << in_file.tellg() << ";" << endl;
	o_file << "const uint8_t rar_file_data[] = {" << endl;
	in_file.seekg(0, ios::beg);

	while (in_file) {
		in_file.read(&data, 1);

		o_file << (int) static_cast<uint8_t>(data) << ", ";
	}

	o_file << endl << "};" << endl;
	o_file.close();

	cout << endl << "Done!" << endl;
	return 0;
}
