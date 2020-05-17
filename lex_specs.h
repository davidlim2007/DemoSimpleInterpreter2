#pragma once

// Only one memory space used. Different interpretations
// based on the attributes.
union tval_union
{
	int		intval;
	double	dblval;
	char*	pstrval;
};

struct token_value_struct
{
	int					tvalType;
	union tval_union	tval;
};

#define ASSIGN			1
#define IDENTIFIER		2
#define INTEGER			3
#define SET				4
#define GET				5
#define ADDITION		6
#define SUBTRACTION		7
#define EXIT			8
#define COMMA			9
#define NEWLINE			10
#define SYNTAX_ERROR	11
#define MULTIPLICATION	12
#define DIVISION		13
#define DOUBLE			14
#define STRING			15
#define CONCATENATION	16