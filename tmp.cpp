#include <iostream>
#include <fstream> 
#include <string>
using namespace std;

int main() {
	istream* stream;

	ifstream fstream(string("test.txt"));
	stream = &fstream;
	
	return 0;
}
