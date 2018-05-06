#include "Lex.hpp"
namespace Pawka {

ostream& operator<< (ostream& stream, Lex lex) {
	static const char* names[610] { nullptr };
    if( names[0] == nullptr ) {
		names[0] =         "LEX_NULL     ";
		names[1] =         "LEX_ID       ";
		names[2] =         "LEX_NUM      ";
		names[3] =         "LEX_STRING   ";
		names['{'] =       "LEX_OP_BRACE ";
		names['}'] =       "LEX_CL_BRACE "; 
		names['='] =       "LEX_ASSIGN   ";
		names['('] =       "LEX_OP_ROUND ";
		names[')'] =       "LEX_CL_ROUND ";
		names[','] =       "LEX_COMMA    ";
		names[';'] =       "LEX_SEMICOLON";
		names['+'] =       "LEX_PLUS     ";
		names['-'] = 	   "LEX_MINUS    ";
		names['*'] =       "LEX_MUL      ";
		names['/'] =       "LEX_DIV      ";
		names['<'] =       "LEX_LESS     ";
		names['>'] =       "LEX_GREATER  ";
		names['=' + 300] = "LEX_EQ       ";
		names['!' + 300] = "LEX_NE       ";
		names['<' + 300] = "LEX_LE       ";
		names['>' + 300] = "LEX_GE       ";
		names[600] =       "LEX_PROGRAM  ";
		names[601] =       "LEX_READ     ";
		names[602] =       "LEX_WRITE    ";
		names[603] =       "LEX_IF       ";
		names[604] =       "LEX_ELSE     ";
		names[605] =       "LEX_NOT      ";
		names[606] =       "LEX_AND      ";
		names[607] =       "LEX_OR       ";
		names[608] =       "LEX_INT      ";
		names[609] =       "LEX_STR      ";
	}

	stream << names[lex.type] <<"   " << lex.str;
}

Lex::Lex(const LexType type, const char *str) : type(type), str(str) { }
Lex::Lex(const LexType type, string str) : type(type), str(str) { }
Lex::Lex() : type(LEX_NULL), str("") { }


LexAnalizer::LexAnalizer(istream& stream) : stream(stream), state(nullptr) {
	TW.push_back("program");
	TW.push_back("read");
	TW.push_back("write");
	TW.push_back("if");
	TW.push_back("else");
	TW.push_back("not");
	TW.push_back("and");
	TW.push_back("or");
	TW.push_back("int");
	TW.push_back("string");
}

LexAnalizer::~LexAnalizer() {  }

void LexAnalizer::makeLex(LexType type) {
	lex = Lex(type, buf);
}

bool LexAnalizer::moveNext() {
	int ch = 0;
	bool readCh = true;

	buf.clear();
	state = &LexAnalizer::stateA;

	if (!stream.good()) // No more lexems
		return false;

	while (stream.good() && readCh) {
		ch = stream.get();
		readCh = (this->*state)(ch);
	}

	return true;
}

bool LexAnalizer::stateA(int c) {
	if (isspace(c)) {
		return true;
	}
	else if (isalpha(c)) {
		buf.push_back(c);
		state = &LexAnalizer::stateB;
		return true;
	}
	else if (isdigit(c)) {
		buf.push_back(c);
		state = &LexAnalizer::stateC;
		return true;
	}
	else if (c == '\"') {
		buf.push_back('\"');
		state = &LexAnalizer::stateD;
		return true;
	}
	else if (c == '{' || c == '}' || c == '(' || c == ')' || c == ';' || c == ',' ||
			 c == '+' || c == '-' || c == '*' || c == '/') {
		buf.push_back(c);
		makeLex(LexType(c));
		return false;
	}
	else if (c == '<' || c == '>' || c == '=' || c == '!') {
		buf.push_back(c);
		state = &LexAnalizer::stateAA;
		return true;
	}
	else if (c == EOF) {
		makeLex(LexType::LEX_NULL);
		return false;
	}
	else {
		throw "Unexpected sym";
	}
}

bool LexAnalizer::stateAA(int c) {
	const int ch = buf.back();

	if (c == '=') {
		buf.push_back('=');
		makeLex(LexType(ch + 300));
		return false;
	}
	else {
		makeLex(LexType(ch));
		if (c != EOF)
			stream.unget();

		return false;
	}
}

bool LexAnalizer::stateB(int c) {
	if (isalpha(c) || isdigit(c)) {
		buf.push_back(c);
		return true;
	}
	else {
		bool isT = false;

		for (int i = 0; i < TW.size() && !isT; ++i) {
			if (buf == TW[i]) {
				isT = true;
				makeLex(LexType(LEX_PROGRAM+i));
			}
		}

		if (!isT)
			makeLex(LexType::LEX_ID);
		if (c != EOF)
			stream.unget();
	}

	return false;
}

bool LexAnalizer::stateC(int c) {
	if (isdigit(c)) {
		buf.push_back(c);
		return true;
	}
	else {
		makeLex(LexType::LEX_NUM);
		if (c != EOF)
			stream.unget();
	}

	return false;
}

bool LexAnalizer::stateD(int c) {
	switch(c) {
	case '\\':
		state = &LexAnalizer::stateDD;
		return true;

	case '\"':
		buf.push_back('\"');
		makeLex(LexType::LEX_STRING);
		return false;

	case EOF: 
		throw "Unexpected end of file";

	default:
		buf.push_back(c);
		return c != '\"';
	}
}

bool LexAnalizer::stateDD(int c) {
	switch (c) {
	case 'n':
		buf.push_back('\n');
		break;

	case 't':
		buf.push_back('\t');
		break;

	case '\\':
		buf.push_back('\\');
		break;

	case '\"':
		buf.push_back('\"');
		break;

	case EOF:
		throw "Unexpected end of file";
		break;

	default:
		buf.push_back('\\');
		buf.push_back(c);
	}

	state = &LexAnalizer::stateD;
	return true;
}
}//namespace
