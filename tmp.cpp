#include <iostream>
#include <locale>
#include <fstream> 
#include <string>
#include <libintl.h>
using namespace std;

int main() {
	setLocale(LC_ALL, "Russian");
	cout << gettext("Hello world!");
	return 0;
}
