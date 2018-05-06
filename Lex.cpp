#include "Lex.hpp"
namespace Pawka {

ostream& operator<< (ostream& stream, Lex lex) {
	static const char* names[] { nullptr };
    if( names[0] == nullptr ) {
		names[0]  = "LEX_NULL      ";
		names[1]  = "LEX_ID        ";
		names[2]  = "LEX_NUM       ";
		names[3]  = "LEX_STRING    ";
		names[4]  = "LEX_OP_BRACE  ";
		names[5]  = "LEX_CL_BRACE  ";
		names[6]  = "LEX_ASSIGN    ";
		names[7]  = "LEX_OP_ROUND  ";
		names[8]  = "LEX_CL_ROUND  ";
		names[9]  = "LEX_COMMA     ";
		names[10] = "LEX_SEMICOLON ";
		names[11] = "LEX_PLUS      ";
		names[12] = "LEX_MINUS     ";
		names[13] = "LEX_MUL       ";
		names[14] = "LEX_DIV       ";
		names[15] = "LEX_LESS      ";
		names[16] = "LEX_GREATER   ";
		names[17] = "LEX_EQ        ";
		names[18] = "LEX_NE        ";
		names[19] = "LEX_LE        ";
		names[20] = "LEX_GE        ";
		names[21] = "LEX_PROGRAM   ";
		names[22] = "LEX_READ      ";
		names[23] = "LEX_WRITE     ";
		names[24] = "LEX_IF        ";
		names[25] = "LEX_ELSE      ";
		names[26] = "LEX_NOT       ";
		names[27] = "LEX_AND       ";
		names[28] = "LEX_OR        ";
		names[29] = "LEX_INT       ";
		names[30] = "LEX_STR       ";
	}

	stream << names[lex.type] << lex.str;
}

Lex::Lex(const LexType type, string str) : type(type), str(str) { }

LexAnalizer::LexAnalizer(istream& stream) : stream(stream), state(nullptr) {
	TW[ string("program")] = LEX_PROGRAM;
	TW[ string("read")   ] = LEX_READ;
	TW[ string("write")  ] = LEX_WRITE;
	TW[ string("if")     ] = LEX_IF;
	TW[ string("else")   ] = LEX_ELSE;
	TW[ string("not")    ] = LEX_NOT;
	TW[ string("and")    ] = LEX_AND;
	TW[ string("or")     ] = LEX_OR;
	TW[ string("int")    ] = LEX_INT;
	TW[ string("string") ] = LEX_STR;
}

LexAnalizer::~LexAnalizer() {  }

void LexAnalizer::makeLex(LexType type) {
	lex = Lex(type, buf);
}

bool LexAnalizer::moveNext() {
	int ch = 0;
	bool readCh = true;

	if (!stream.good())
		return false;

	buf.clear();
	state = &LexAnalizer::FirstSym;
	while ( ( this->*state )( stream.get() ) ) { }

	return true;
}

bool LexAnalizer::FirstSym(int c) {
	buf.push_back(c);
	switch (c) {
		case '\"':
			state = &LexAnalizer::ReadString;
			return true;
			break;

		case '{':
			makeLex(LEX_OP_BRACE);
			return false;

		case '}':
			makeLex(LEX_CL_BRACE);
			return false;

		case '(':
			makeLex(LEX_OP_ROUND);
			return false;

		case ')':
			makeLex(LEX_CL_ROUND);
			return false;

		case ';':
			makeLex(LEX_SEMICOLON);
			return false;

		case ',':
			makeLex(LEX_COMMA);
			return false;

		case '+':
			makeLex(LEX_PLUS);
			return false;

		case '-':
			makeLex(LEX_MINUS);
			return false;

		case '*':
			makeLex(LEX_MUL);
			return false;

		case '/':
			makeLex(LEX_DIV);
			return false;

		case '<':
		case '>':
		case '=':
		case '!':
			state = &LexAnalizer::ComplexOperation;
			return true;

		case EOF:
			makeLex(LEX_NULL);
			return false;

		default:
			if (isspace(c)) {
				buf.pop_back();
				return true;
			}
			else if (isalpha(c)) {
				state = &LexAnalizer::ReadId;
				return true;
			}
			else if (isdigit(c)) {
				state = &LexAnalizer::ReadNum;
				return true;
			}
			else {
				throw "Unexpected sym";
			}
			break;
	}
}

bool LexAnalizer::ComplexOperation(int c) {
	const char ch = buf.back();
	if (c == '=') {
		buf.push_back('=');
		switch (ch) {
			case '<':
				makeLex(LEX_LE);
				break;

			case '>':
				makeLex(LEX_GE);
				break;

			case '=':
				makeLex(LEX_EQ);
				break;

			case '!':
				makeLex(LEX_NE);
				break;
		}
	}
	else {
		stream.unget();
		switch (ch) {
			case '<':
				makeLex(LEX_LESS);
				break;

			case '>':
				makeLex(LEX_GREATER);
				break;

			case '=':
				makeLex(LEX_ASSIGN);
				break;

			case '!':
				throw "Unexpected symbol";
				break;
		}
	}
	return false;
}

bool LexAnalizer::ReadId(int c) {
	if (isalpha(c) || isdigit(c)) {
		buf.push_back(c);
		return true;
	}
	else {
		if (TW.count(buf) > 0)
			makeLex(TW[buf]);
		else
			makeLex(LEX_ID);

		stream.unget();
		return false;
	}
}

bool LexAnalizer::ReadNum(int c) {
	if (isdigit(c)) {
		buf.push_back(c);
		return true;
	}
	else {
		makeLex(LexType::LEX_NUM);
		stream.unget();
	}

	return false;
}

bool LexAnalizer::ReadString(int c) {
	switch(c) {
	case '\\':
		state = &LexAnalizer::Shielding;
		return true;

	case '\"':
		buf.push_back('\"');
		makeLex(LEX_STRING);
		return false;

	case EOF:
		throw "Unexpected end of file";

	default:
		buf.push_back(c);
		return true;
	}
}

bool LexAnalizer::Shielding(int c) {
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
		throw "Bed shielding";
		break;
	}

	state = &LexAnalizer::ReadString;
	return true;
}
}//namespace
