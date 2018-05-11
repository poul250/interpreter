#include <string>
#include "utils.hpp"

namespace Pawka {
int strToInt(const std::string& src) {
	int answer = 0;
	int sign = 1;
	int start = 0;
	if (src[0] == '+' || src[0] == '-') {
		start = 1;
		sign = src[0] == '+' ? 1 : -1;
	}
	for (int i = start; i < src.size(); ++i) {
		answer = answer * 10 + src[i] - '0';
	}
	return answer * sign;
}

double strToDouble (const std::string& src) {
	double answer = 0;
	int sign = 1;
	int start = 0;
	if (src[0] == '+' || src[0] == '-') {
		start = 1;
		sign = src[0] == '+' ? 1 : -1;
	} 
	for (int i = start; i < src.size(); ++i) {
		if (src[i] == '.') {
			start = i+1;
			break;
		}
		answer = answer * 10 + src[i] - '0';
	}
	for (int i = start, div = 10; i < src.size(); ++i, div *= 10) {
		answer += (src[i] - '0') / div;
	}
	return answer * sign;
}

} //namespace
