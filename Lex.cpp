#include "Lex.hpp"
namespace Pawka {

ostream& operator<< (ostream& stream, Lex lex) {
	static const char* names[36] { nullptr };
    if( names[0] == nullptr ) {
		names[LEX_NULL     ] = "LEX_NULL      ";
		names[LEX_ID       ] = "LEX_ID        ";
		names[LEX_NUM      ] = "LEX_NUM       ";
		names[LEX_REAL_NUM ] = "LEX_REAL_NUM  ";
		names[LEX_STRING   ] = "LEX_STRING    ";
		names[LEX_OP_BRACE ] = "LEX_OP_BRACE  ";
		names[LEX_CL_BRACE ] = "LEX_CL_BRACE  ";
		names[LEX_ASSIGN   ] = "LEX_ASSIGN    ";
		names[LEX_OP_ROUND ] = "LEX_OP_ROUND  ";
		names[LEX_CL_ROUND ] = "LEX_CL_ROUND  ";
		names[LEX_COMMA    ] = "LEX_COMMA     ";
		names[LEX_SEMICOLON] = "LEX_SEMICOLON ";
		names[LEX_PLUS     ] = "LEX_PLUS      ";
		names[LEX_MINUS    ] = "LEX_MINUS     ";
		names[LEX_MUL      ] = "LEX_MUL       ";
		names[LEX_DIV      ] = "LEX_DIV       ";
		names[LEX_LESS     ] = "LEX_LESS      ";
		names[LEX_GREATER  ] = "LEX_GREATER   ";
		names[LEX_EQ       ] = "LEX_EQ        ";
		names[LEX_NE       ] = "LEX_NE        ";
		names[LEX_LE       ] = "LEX_LE        ";
		names[LEX_GE       ] = "LEX_GE        ";
		names[LEX_PROGRAM  ] = "LEX_PROGRAM   ";
		names[LEX_READ     ] = "LEX_READ      ";
		names[LEX_WRITE    ] = "LEX_WRITE     ";
		names[LEX_IF       ] = "LEX_IF        ";
		names[LEX_ELSE     ] = "LEX_ELSE      ";
		names[LEX_DO       ] = "LEX_DO        ";
		names[LEX_BREAK    ] = "LEX_BREAK     ";
		names[LEX_NOT      ] = "LEX_NOT       ";
		names[LEX_AND      ] = "LEX_AND       ";
		names[LEX_OR       ] = "LEX_OR        ";
		names[LEX_INT      ] = "LEX_INT       ";
		names[LEX_STR      ] = "LEX_STR       ";
		names[LEX_REAL     ] = "LEX_REAL      ";
		names[LEX_WHILE    ] = "LEX_WHILE     ";
	}

	stream << names[lex.type] << lex.str;
}

Lex::Lex(const LexType type, string str) : type(type), str(str) { }

void LexAnalizer::init() {
	state = nullptr;
	line = 1;
	TW[string("program")] = LEX_PROGRAM;
	TW[string("read"   )] = LEX_READ;
	TW[string("write"  )] = LEX_WRITE;
	TW[string("if"     )] = LEX_IF;
	TW[string("else"   )] = LEX_ELSE;
	TW[string("do"     )] = LEX_DO;
	TW[string("break"  )] = LEX_BREAK;
	TW[string("not"    )] = LEX_NOT;
	TW[string("and"    )] = LEX_AND;
	TW[string("or"     )] = LEX_OR;
	TW[string("int"    )] = LEX_INT;
	TW[string("string" )] = LEX_STR;
	TW[string("real"   )] = LEX_REAL;
	TW[string("while"  )] = LEX_WHILE;
}

LexAnalizer::LexAnalizer(istream& stream)
		: stream(stream) {
	init();
}

LexAnalizer::~LexAnalizer() {  }

bool LexAnalizer::moveNext() {
	int ch = 0;

	buf.clear();
	state = &LexAnalizer::FirstSym;
	while ((this->*state)(get())) { }

	return lex.type != LEX_NULL;
}

int inline LexAnalizer::get() {
	return stream.get();
}

void inline LexAnalizer::unget() {
	stream.unget();
}

bool LexAnalizer::FirstSym(int c) {
	buf.push_back(c);
	switch (c) {
		case '{': makeLex(LEX_OP_BRACE);  return false;
		case '}': makeLex(LEX_CL_BRACE);  return false;
		case '(': makeLex(LEX_OP_ROUND);  return false;
		case ')': makeLex(LEX_CL_ROUND);  return false;
		case ';': makeLex(LEX_SEMICOLON); return false;
		case ',': makeLex(LEX_COMMA);     return false;
		case '+': makeLex(LEX_PLUS);      return false;
		case '-': makeLex(LEX_MINUS);     return false;
		case '*': makeLex(LEX_MUL);       return false;
		case EOF: makeLex(LEX_NULL);      return false;

		case '<':
		case '>':
		case '=':
		case '!':
		case '/':
			state = &LexAnalizer::ComplexOperation;
			return true;
		case '\"':
			buf.pop_back();
			state = &LexAnalizer::ReadString;
			return true;

		default:
			if (isspace(c)) {
				buf.pop_back();
				if (c == '\n')
					++line;
				return true;
			} else if (isalpha(c) || c == '_') {
				state = &LexAnalizer::ReadId;
				return true;
			} else if (isdigit(c)) {
				state = &LexAnalizer::ReadNum;
				return true;
			} else {	//APASNA
				throw "Unexpected sym '" + string((char*)(&c)) + "'";
			}
			break;
	}
}

bool LexAnalizer::ComplexOperation(int c) {
	const char ch = buf.back();

	if (ch == '/') {
		if (c == '/') {
			buf.pop_back();
			state = &LexAnalizer::Comment;
			return true;
		} else if (c == '*') {
			buf.pop_back();
			state = &LexAnalizer::BigComment;
			return true;
		} else {
			unget();
			makeLex(LEX_DIV);
		}
	} else if (c == '=') {
		buf.push_back('=');
		switch (ch) {
			case '<': makeLex(LEX_LE); break;
			case '>': makeLex(LEX_GE); break;
			case '=': makeLex(LEX_EQ); break;
			case '!': makeLex(LEX_NE); break;
		}
	} else {
		unget();
		switch (ch) {
			case '<': makeLex(LEX_LESS);    break;
			case '>': makeLex(LEX_GREATER); break;
			case '=': makeLex(LEX_ASSIGN);  break;
			case '!': throw "Unexpected symbol '!'";break;
		}
	}

	return false;
}

bool LexAnalizer::Comment(int c) {
	if (c == '\n')
		state = &LexAnalizer::FirstSym;
	return true;
}

bool LexAnalizer::BigComment(int c) {
	static bool lastStar = false;
	if (c == '*')
		lastStar = true;
	else if (c == '\n')
		++line;
	else if (c == '/' && lastStar)
		state = &LexAnalizer::FirstSym;
	else if (c == EOF)
		throw "Unexpected end of file";
	else
		lastStar = false;
	return true;
}

bool LexAnalizer::ReadId(int c) {
	if (isalpha(c) || isdigit(c) || c == '_') {
		buf.push_back(c);
		return true;
	} else {
		if (TW.count(buf) > 0)
			makeLex(TW[buf]);
		else
			makeLex(LEX_ID);

		unget();
		return false;
	}
}

bool LexAnalizer::ReadNum(int c) {
	if (isdigit(c)) {
		buf.push_back(c);
		return true;
	} else if (c == '.') {
		buf.push_back(c);
		state = &LexAnalizer::ReadRealNum;
		return true;
	} else {
		makeLex(LEX_NUM);
		unget();
	}

	return false;
}

bool LexAnalizer::ReadRealNum(int c) {
	static int num = 0;
	if (isdigit(c)) {
		++num;
		buf.push_back(c);
		return true;
	} else {
		if (num == 0)
			throw "unexpected symbol '" + string((char*)(&c)) + "'";
		makeLex(LEX_REAL_NUM);
		unget();
		num = 0;
		return false;
	}
}

bool LexAnalizer::ReadString(int c) {
	switch(c) {
		case '\\':
			state = &LexAnalizer::Shielding;
			return true;
		case '\"':
			makeLex(LEX_STRING);
			return false;
		case '\n':
			throw "Unexpected end of line";
		case EOF:
			throw "Unexpected end of file";
		default:
			buf.push_back(c);
			return true;
	}
}

bool LexAnalizer::Shielding(int c) {
	switch (c) {
		case 'n':  buf.push_back('\n'); break;
		case 't':  buf.push_back('\t'); break;
		case '\\': buf.push_back('\\'); break;
		case '\"': buf.push_back('\"'); break;
		case EOF:  throw "Unexpected end of file"; break;
		default:   throw "Bed shielding"; break;
	}

	state = &LexAnalizer::ReadString;
	return true;
}
}//namespace
