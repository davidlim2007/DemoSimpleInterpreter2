// DemoSimpleInterpreter2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include "lex_specs.h"

extern "C" FILE *yyin;
extern "C" int yylex();
extern "C" int yylineno;
extern "C" char* yytext;
extern "C" void SkipAllTokensInLine();
extern "C" token_value_struct g_token_value;

class NameValue
{
public:
	NameValue()
	{
		token_value.tvalType = INTEGER;
		token_value.tval.intval = 0;
	}

	NameValue(const char* lpszName, token_value_struct& token_value) :
		strName(lpszName)
	{
		copy_token_value(token_value);
	}


	// Copy constructor
	NameValue(const NameValue &rhs)
	{
		strName = rhs.strName;
		copy_token_value(rhs.token_value);
	}

	~NameValue()
	{
		clear_token_value();
	}

	void displayValue()
	{
		if (token_value.tvalType == INTEGER)
		{
			printf("Identifier [%s] : Value [%d]\n", this->strName.c_str(), this->token_value.tval.intval);
		}
		else if (token_value.tvalType == DOUBLE)
		{
			printf("Identifier [%s] : Value [%f]\n", this->strName.c_str(), this->token_value.tval.dblval);
		}
		else if (token_value.tvalType == STRING)
		{
			printf("Identifier [%s] : Value [%s]\n", this->strName.c_str(), this->token_value.tval.pstrval);
		}
		else
		{

		}
	}

	NameValue& operator=(const NameValue& rhs)
	{
		strName = rhs.strName;
		copy_token_value(rhs.token_value);
		return *this;
	}

	NameValue& operator=(const token_value_struct& rhs)
	{
		copy_token_value(rhs);
		return *this;
	}

	NameValue& operator+=(const NameValue& nv)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval += nv.token_value.tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval += nv.token_value.tval.dblval;
		}
		else if (this->token_value.tvalType == STRING)
		{
			// Note: Use strlen() instead of sizeof(). strlen() returns the actual length of the
			// string, while sizeof() simply returns the size of the pointer. (e.g. sizeof int will
			// be 8 bytes).
			char* new_string = new char[strlen(token_value.tval.pstrval) + strlen(nv.token_value.tval.pstrval) + 1];
			strcpy(new_string, this->token_value.tval.pstrval);
			strcat(new_string, nv.token_value.tval.pstrval);

			clear_token_value();

			this->token_value.tval.pstrval = _strdup(new_string);

			// If "new" is used to create an array, the array pointer must later be deleted
			// using [].
			delete[] new_string;
		}
		else
		{

		}

		return *this;
	}

	NameValue& operator+=(const tval_union& tval)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval += tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval += tval.dblval;
		}
		else if (this->token_value.tvalType == STRING)
		{
			char* new_string = new char[strlen(token_value.tval.pstrval) + strlen(tval.pstrval) + 1];
			strcpy(new_string, this->token_value.tval.pstrval);
			strcat(new_string, tval.pstrval);

			clear_token_value();

			this->token_value.tval.pstrval = _strdup(new_string);
			delete [] new_string;
		}
		else
		{
		}

		return *this;
	}

	NameValue& operator+=(const token_value_struct& rhs)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval += rhs.tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval += rhs.tval.dblval;
		}
		else if (this->token_value.tvalType == STRING)
		{
			char *new_string = new char[strlen(token_value.tval.pstrval) + strlen(rhs.tval.pstrval) + 1];
			strcpy(new_string, this->token_value.tval.pstrval);
			strcat(new_string, rhs.tval.pstrval);

			clear_token_value();

			this->token_value.tval.pstrval = _strdup(new_string);
			delete[] new_string;
		}
		else
		{

		}

		return *this;
	}

	NameValue& operator-=(const NameValue& nv)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval -= nv.token_value.tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval -= nv.token_value.tval.dblval;
		}
		else
		{

		}

		return *this;
	}

	NameValue& operator-=(const tval_union& tval)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval -= tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval -= tval.dblval;
		}
		else
		{

		}

		return *this;
	}

	NameValue& operator-=(const token_value_struct& rhs)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval -= rhs.tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval -= rhs.tval.dblval;
		}
		else
		{

		}

		return *this;
	}

	NameValue& operator*=(const NameValue& nv)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval *= nv.token_value.tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval *= nv.token_value.tval.dblval;
		}
		else
		{

		}

		return *this;
	}

	NameValue& operator*=(const tval_union& tval)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval *= tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval *= tval.dblval;
		}
		else
		{

		}

		return *this;
	}

	NameValue& operator*=(const token_value_struct& rhs)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval *= rhs.tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval *= rhs.tval.dblval;
		}
		else
		{

		}

		return *this;
	}

	NameValue& operator/=(const NameValue& nv)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval /= nv.token_value.tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval /= nv.token_value.tval.dblval;
		}
		else
		{

		}

		return *this;
	}

	NameValue& operator/=(const tval_union& tval)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval /= tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval /= tval.dblval;
		}
		else
		{

		}

		return *this;
	}

	NameValue& operator/=(const token_value_struct& rhs)
	{
		if (this->token_value.tvalType == INTEGER)
		{
			this->token_value.tval.intval /= rhs.tval.intval;
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			this->token_value.tval.dblval /= rhs.tval.dblval;
		}
		else
		{

		}

		return *this;
	}

	bool operator==(const NameValue& nv)
	{
		if (this->token_value.tvalType != nv.token_value.tvalType)
		{
			return false;
		}

		if (this->token_value.tvalType == INTEGER)
		{
			return (this->token_value.tval.intval == nv.token_value.tval.intval);
		}
		else if (this->token_value.tvalType == DOUBLE)
		{
			return (this->token_value.tval.dblval == nv.token_value.tval.dblval);
		}
		else
		{

		}
	}

	void clear_token_value()
	{
		if (token_value.tvalType == STRING)
		{
			if (token_value.tval.pstrval != NULL)
			{
				free(token_value.tval.pstrval);
				token_value.tval.pstrval = NULL;
			}
		}
	}

	void copy_token_value(const token_value_struct& other)
	{
		clear_token_value();

		token_value.tvalType = other.tvalType;

		if (other.tvalType == STRING)
		{
			token_value.tval.pstrval = _strdup(other.tval.pstrval);
		}
		else
		{
			token_value.tval = other.tval;
		}
	}

	std::string strName;
	token_value_struct	token_value;
};

std::vector<NameValue> vecNVPairs;

std::vector<NameValue>::iterator GetNameValue(const std::string strName)
{
	std::vector<NameValue>::iterator theIterator;

	for (theIterator = vecNVPairs.begin(); theIterator != vecNVPairs.end(); theIterator++)
	{
		if ((*theIterator).strName == strName)
		{
			return theIterator;
		}
	}

	return vecNVPairs.end();
}

void SetNameValue(const NameValue nameValue)
{
	std::vector<NameValue>::iterator theIterator;

	theIterator = GetNameValue(nameValue.strName);

	if (theIterator == vecNVPairs.end())
	{
		vecNVPairs.push_back(nameValue);
	}
	else
	{
		*theIterator = NameValue(nameValue);
	}
}

void SET_handler(int token)
{
	NameValue nameValue;

	token = yylex();
	if (token != IDENTIFIER)
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	nameValue.strName = yytext;

	token = yylex();
	if (token != ASSIGN)
	{
		printf("Syntax error in Line [%d]. Expecting an ASSIGN but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	token = yylex();
	if (token == IDENTIFIER)
	{
		// Get the NameValue associated with the next IDENTIFIER.
		std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

		if (theIterator == vecNVPairs.end())
		{
			printf("Identifier [%s] Not Found.\n", yytext);
			SkipAllTokensInLine();
			return;
		}
		else
		{
			(*theIterator).displayValue();

			// Do the SET.
			nameValue.token_value.tvalType = (*theIterator).token_value.tvalType;
			nameValue.token_value.tval = (*theIterator).token_value.tval;
			SetNameValue(nameValue);
		}
	}
	else if (token == INTEGER)
	{
		// Do the SET.
		nameValue.copy_token_value(g_token_value);
		SetNameValue(nameValue);
	}
	else if (token == DOUBLE)
	{
		// Do the SET.
		nameValue.copy_token_value(g_token_value);
		SetNameValue(nameValue);
	}
	else if (token == STRING)
	{
		// Do the SET.
		nameValue.copy_token_value(g_token_value);
		SetNameValue(nameValue);
		free(g_token_value.tval.pstrval);
	}
	else
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER, INTEGER, or DOUBLE but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	nameValue.displayValue();
}

void GET_handler(int token)
{
	token = yylex();
	if (token != IDENTIFIER)
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

	if (theIterator == vecNVPairs.end())
	{
		printf("Identifier [%s] Not Found.\n", yytext);
		SkipAllTokensInLine();
	}
	else
	{
		(*theIterator).displayValue();
	}
}

void ADDITION_handler(int token)
{
	// Get the next token and make sure it is an IDENTIFIER.
	token = yylex();
	if (token != IDENTIFIER)
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	// Get the NameValue associated with IDENTIFIER.
	std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

	if (theIterator == vecNVPairs.end())
	{
		printf("Identifier [%s] Not Found.\n", yytext);
		SkipAllTokensInLine();
		return;
	}
	else
	{
		(*theIterator).displayValue();
	}

	// Set nameValue to the discovered NameValue pair found in vecNVPairs.
	NameValue nameValue = (*theIterator);

	// Get the next token and make sure it is a COMMA.
	token = yylex();
	if (token != COMMA)
	{
		printf("Syntax error in Line [%d]. Expecting a COMMA but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	// Get the next token and make sure it is either an IDENTIFIER or a literal INTEGER value.
	token = yylex();
	if (token == IDENTIFIER)
	{
		// Get the NameValue associated with the next IDENTIFIER.
		std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

		if (theIterator == vecNVPairs.end())
		{
			printf("Identifier [%s] Not Found.\n", yytext);
			SkipAllTokensInLine();
			return;
		}
		else
		{
			if (nameValue.token_value.tvalType != (*theIterator).token_value.tvalType)
			{
				printf("Identifier [%s] and Identifier [%s] have incompatible tvalTypes. Cannot perform ADDITION.\n", nameValue.strName.c_str(), (*theIterator).strName.c_str());
				SkipAllTokensInLine();
				return;
			}

			(*theIterator).displayValue();

			// Do the ADDITION.
			nameValue += (*theIterator);
			SetNameValue(nameValue);
		}
	}
	else if (token == INTEGER)
	{
		if (nameValue.token_value.tvalType != INTEGER)
		{
			printf("Identifier [%s] is of type DOUBLE and cannot perform ADDITION on an INTEGER.\n", nameValue.strName.c_str());
			SkipAllTokensInLine();
			return;
		}

		// Do the ADDITION.
		nameValue += g_token_value;
		SetNameValue(nameValue);
	}
	else if (token == DOUBLE)
	{
		if (nameValue.token_value.tvalType != DOUBLE)
		{
			printf("Identifier [%s] is of type INTEGER and cannot perform ADDITION on an DOUBLE.\n", nameValue.strName.c_str());
			SkipAllTokensInLine();
			return;
		}

		// Do the ADDITION.
		nameValue += g_token_value;
		SetNameValue(nameValue);
	}
	else
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER, INTEGER or DOUBLE but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	nameValue.displayValue();
}

void SUBTRACTION_handler(int token)
{
	// Get the next token and make sure it is an IDENTIFIER.
	token = yylex();
	if (token != IDENTIFIER)
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	// Get the NameValue associated with IDENTIFIER.
	std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

	if (theIterator == vecNVPairs.end())
	{
		printf("Identifier [%s] Not Found.\n", yytext);
		SkipAllTokensInLine();
		return;
	}
	else
	{
		(*theIterator).displayValue();
	}

	// Set nameValue to the discovered NameValue pair found in vecNVPairs.
	NameValue nameValue = (*theIterator);

	// Get the next token and make sure it is a COMMA.
	token = yylex();
	if (token != COMMA)
	{
		printf("Syntax error in Line [%d]. Expecting a COMMA but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	// Get the next token and make sure it is either an IDENTIFIER or a literal INTEGER value.
	token = yylex();
	if (token == IDENTIFIER)
	{
		// Get the NameValue associated with the next IDENTIFIER.
		std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

		if (theIterator == vecNVPairs.end())
		{
			printf("Identifier [%s] Not Found.\n", yytext);
			SkipAllTokensInLine();
			return;
		}
		else
		{
			if (nameValue.token_value.tvalType != (*theIterator).token_value.tvalType)
			{
				printf("Identifier [%s] and Identifier [%s] have incompatible tvalTypes. Cannot perform SUBTRACTION.\n", nameValue.strName.c_str(), (*theIterator).strName.c_str());
				SkipAllTokensInLine();
				return;
			}

			(*theIterator).displayValue();

			// Do the SUBTRACTION.
			nameValue -= (*theIterator);
			SetNameValue(nameValue);
		}
	}
	else if (token == INTEGER)
	{
		if (nameValue.token_value.tvalType != INTEGER)
		{
			printf("Identifier [%s] is of type DOUBLE and cannot perform SUBTRACTION on an INTEGER.\n", nameValue.strName.c_str());
			SkipAllTokensInLine();
			return;
		}

		// Do the SUBTRACTION.
		nameValue -= g_token_value;
		SetNameValue(nameValue);
	}
	else if (token == DOUBLE)
	{
		if (nameValue.token_value.tvalType != DOUBLE)
		{
			printf("Identifier [%s] is of type INTEGER and cannot perform SUBTRACTION on an DOUBLE.\n", nameValue.strName.c_str());
			SkipAllTokensInLine();
			return;
		}

		// Do the SUBTRACTION.
		nameValue -= g_token_value;
		SetNameValue(nameValue);
	}
	else
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER, INTEGER or DOUBLE but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	nameValue.displayValue();
}

void MULTIPLICATION_handler(int token)
{
	// Get the next token and make sure it is an IDENTIFIER.
	token = yylex();
	if (token != IDENTIFIER)
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	// Get the NameValue associated with IDENTIFIER.
	std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

	if (theIterator == vecNVPairs.end())
	{
		printf("Identifier [%s] Not Found.\n", yytext);
		SkipAllTokensInLine();
		return;
	}
	else
	{
		(*theIterator).displayValue();
	}

	// Set nameValue to the discovered NameValue pair found in vecNVPairs.
	NameValue nameValue = (*theIterator);

	// Get the next token and make sure it is a COMMA.
	token = yylex();
	if (token != COMMA)
	{
		printf("Syntax error in Line [%d]. Expecting a COMMA but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	// Get the next token and make sure it is either an IDENTIFIER or a literal INTEGER value.
	token = yylex();
	if (token == IDENTIFIER)
	{
		// Get the NameValue associated with the next IDENTIFIER.
		std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

		if (theIterator == vecNVPairs.end())
		{
			printf("Identifier [%s] Not Found.\n", yytext);
			SkipAllTokensInLine();
			return;
		}
		else
		{
			if (nameValue.token_value.tvalType != (*theIterator).token_value.tvalType)
			{
				printf("Identifier [%s] and Identifier [%s] have incompatible tvalTypes. Cannot perform MULTIPLICATION.\n", nameValue.strName.c_str(), (*theIterator).strName.c_str());
				SkipAllTokensInLine();
				return;
			}

			(*theIterator).displayValue();

			// Do the MULTIPLICATION.
			nameValue *= (*theIterator);
			SetNameValue(nameValue);
		}
	}
	else if (token == INTEGER)
	{
		if (nameValue.token_value.tvalType != INTEGER)
		{
			printf("Identifier [%s] is of type DOUBLE and cannot perform MULTIPLICATION on an INTEGER.\n", nameValue.strName.c_str());
			SkipAllTokensInLine();
			return;
		}

		// Do the MULTIPLICATION.
		nameValue *= g_token_value;
		SetNameValue(nameValue);
	}
	else if (token == DOUBLE)
	{
		if (nameValue.token_value.tvalType != DOUBLE)
		{
			printf("Identifier [%s] is of type INTEGER and cannot perform MULTIPLICATION on an DOUBLE.\n", nameValue.strName.c_str());
			SkipAllTokensInLine();
			return;
		}

		// Do the MULTIPLICATION.
		nameValue *= g_token_value;
		SetNameValue(nameValue);
	}
	else
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER, INTEGER or DOUBLE but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	nameValue.displayValue();
}

void DIVISION_handler(int token)
{
	// Get the next token and make sure it is an IDENTIFIER.
	token = yylex();
	if (token != IDENTIFIER)
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	// Get the NameValue associated with IDENTIFIER.
	std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

	if (theIterator == vecNVPairs.end())
	{
		printf("Identifier [%s] Not Found.\n", yytext);
		SkipAllTokensInLine();
		return;
	}
	else
	{
		if (
			((*theIterator).token_value.tvalType == INTEGER) && ((*theIterator).token_value.tval.intval == 0)
												||
			((*theIterator).token_value.tvalType == DOUBLE) && ((*theIterator).token_value.tval.dblval == 0.0)
			)
		{
			printf("Identifier [%s] has a value of 0. Cannot perform DIVISION.\n", yytext);
			SkipAllTokensInLine();
			return;
		}

		(*theIterator).displayValue();
	}

	// Set nameValue to the discovered NameValue pair found in vecNVPairs.
	NameValue nameValue = (*theIterator);

	// Get the next token and make sure it is a COMMA.
	token = yylex();
	if (token != COMMA)
	{
		printf("Syntax error in Line [%d]. Expecting a COMMA but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	// Get the next token and make sure it is either an IDENTIFIER or a literal INTEGER value.
	token = yylex();
	if (token == IDENTIFIER)
	{
		// Get the NameValue associated with the next IDENTIFIER.
		std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

		if (theIterator == vecNVPairs.end())
		{
			printf("Identifier [%s] Not Found.\n", yytext);
			SkipAllTokensInLine();
			return;
		}
		else
		{
			if (nameValue.token_value.tvalType != (*theIterator).token_value.tvalType)
			{
				printf("Identifier [%s] and Identifier [%s] have incompatible tvalTypes. Cannot perform DIVISION.\n", nameValue.strName.c_str(), (*theIterator).strName.c_str());
				SkipAllTokensInLine();
				return;
			}

			(*theIterator).displayValue();

			if (
				((*theIterator).token_value.tvalType == INTEGER) && ((*theIterator).token_value.tval.intval == 0)
													||
				((*theIterator).token_value.tvalType == DOUBLE) && ((*theIterator).token_value.tval.dblval == 0.0)
				)
			{
				printf("Identifier [%s] has a value of 0. Cannot perform DIVISION.\n", yytext);
				SkipAllTokensInLine();
				return;
			}

			// Do the DIVISION.
			nameValue /= (*theIterator);
			SetNameValue(nameValue);
		}
	}
	else if (token == INTEGER)
	{
		// Check if divisor is 0
		if (g_token_value.tval.intval == 0)
		{
			printf("Divisor is 0. Cannot perform DIVISION.\n");
			SkipAllTokensInLine();
			return;
		}

		// Do the DIVISION.
		nameValue /= g_token_value;
		SetNameValue(nameValue);
	}
	else if (token == DOUBLE)
	{
		// Check if divisor is 0
		if (g_token_value.tval.dblval == 0.0)
		{
			printf("Divisor is 0. Cannot perform DIVISION.\n");
			SkipAllTokensInLine();
			return;
		}

		// Do the DIVISION.
		nameValue /= g_token_value;
		SetNameValue(nameValue);
	}
	else
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER, INTEGER or DOUBLE but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	nameValue.displayValue();
}

void CONCATENATION_handler(int token)
{
	NameValue nameValue;

	token = yylex();
	if (token != IDENTIFIER)
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	// Get the NameValue associated with IDENTIFIER.
	std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

	if (theIterator == vecNVPairs.end())
	{
		printf("Identifier [%s] Not Found.\n", yytext);
		SkipAllTokensInLine();
		return;
	}
	else if ((*theIterator).token_value.tvalType != STRING)
	{
		printf("Identifier [%s] is not a STRING, cannot perform CONCATENATION.\n", yytext);
		SkipAllTokensInLine();
		return;
	}
	else
	{
		(*theIterator).displayValue();
	}

	nameValue = (*theIterator);

	token = yylex();
	if (token != COMMA)
	{
		printf("Syntax error in Line [%d]. Expecting a COMMA but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	token = yylex();
	if (token == IDENTIFIER)
	{
		// Get the NameValue associated with the next IDENTIFIER.
		std::vector<NameValue>::iterator theIterator = GetNameValue(std::string(yytext));

		if (theIterator == vecNVPairs.end())
		{
			printf("Identifier [%s] Not Found.\n", yytext);
			SkipAllTokensInLine();
			return;
		}
		else if ((*theIterator).token_value.tvalType != STRING)
		{
			printf("Identifier [%s] is not a STRING, cannot perform CONCATENATION.\n", yytext);
			SkipAllTokensInLine();
			return;
		}
		else
		{
			(*theIterator).displayValue();

			// Do the CONCATENATION.
			nameValue += (*theIterator);
			SetNameValue(nameValue);
		}
	}
	else if (token == STRING)
	{
		// Do the CONCATENATION.
		nameValue += g_token_value;
		SetNameValue(nameValue);
		free(g_token_value.tval.pstrval);
	}
	else
	{
		printf("Syntax error in Line [%d]. Expecting an IDENTIFIER or STRING but found [%s]\n", yylineno, yytext);
		SkipAllTokensInLine();
		return;
	}

	nameValue.displayValue();
}

// <STRING_LIT> string literal processing helper functions.
std::string g_strStringLiteralBuffer;

extern "C" void FreeGlobalStringLitBuffer()
{
	g_strStringLiteralBuffer.clear();
}

extern "C" void InitGlobalStringLitBuffer()
{
	FreeGlobalStringLitBuffer();
}

extern "C" const char* GetGlobalStringLitBuffer()
{
	return g_strStringLiteralBuffer.c_str();
}

extern "C" void AppendGlobalStringLitBuffer(const char* str)
{
	g_strStringLiteralBuffer.append(str);
}

extern "C" void AppendCharGlobalStringLitBuffer(const char ch)
{
	g_strStringLiteralBuffer += ch;
}

int main(int argc, char** argv)
{
	++argv, --argc;  /* skip over program name */
	if (argc > 0)
		fopen_s(&yyin, argv[0], "r");
	else
		yyin = stdin;

	int token;

	while (token = yylex())
	{
		switch (token)
		{
			case SET:
			{
				SET_handler(token);
				break;
			}

			case GET:
			{
				GET_handler(token);
				break;
			}

			case ADDITION :
			{
				ADDITION_handler(token);
				break;
			}

			case SUBTRACTION :
			{
				SUBTRACTION_handler(token);
				break;
			}

			case MULTIPLICATION:
			{
				MULTIPLICATION_handler(token);
				break;
			}

			case DIVISION:
			{
				DIVISION_handler(token);
				break;
			}

			case CONCATENATION:
			{
				CONCATENATION_handler(token);
				break;
			}

			case EXIT:
			{
				return 0;
			}

			case NEWLINE:
			{
				break;
			}

			default:
			{
				printf("Syntax Error. Unrecognized item : [%s]\n", yytext);
				SkipAllTokensInLine();
				break;
			}
		}
	}

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
