#pragma once
#include <string>
#include "Lex.hpp"
using namespace std;

namespace Pawka {
struct Ident {
	Ident(const string& src);
	Ident(const Ident&);

	string name;
	LexType type;
	bool assign;
};
}//namespace
