#include <string>
#include "utils.hpp"

using namespace std;

namespace Pawka {
Ident::Ident(const string& src)
	: assign(false)
	, type(LEX_NULL)
	, name(src)
{	}

Ident::Ident(const Ident& src)
	: assign(src.assign)
	, type(src.type)
	, name(src.name)
{	}

} //namespace
