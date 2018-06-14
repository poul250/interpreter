#include <iostream>
#include <fstream>
#include "Interpretator.hpp"

using namespace std;
using namespace Pawka;

int main(int argc, char* argv[])
{
	ifstream fstream;
	bool fromFile = false;
	if (argc >= 3) {
		cout << "Usage: " << argv[0] << " [file_name]" << endl;
		return -1;
	} else if (argc == 2) {
		fstream.open(argv[1]);
		fromFile = true;
		if (!fstream) {
			cout << "Error: can't open file" << endl;
			return -1;
		}
	}

	istream& stream = fromFile ? fstream : cin;
	Interpretator inter(stream);
	inter.Interpretate();

	return 0;
}
