
%{

// need header for the call to exit() in lex.yy.c.
#include <stdlib.h>
#include <io.h>
#include "lex_specs.h"

#define isatty	_isatty
#define fileno	_fileno
/* union tval_union	tval; */

void FreeGlobalStringLitBuffer();
void InitGlobalStringLitBuffer();

const char* GetGlobalStringLitBuffer();
void AppendGlobalStringLitBuffer(const char* str);
void AppendCharGlobalStringLitBuffer(const char ch);

struct token_value_struct	g_token_value;

%}

%option yylineno
%x STRING_LIT

DIGIT						[0-9]
ID							[a-zA-Z_][a-zA-Z_0-9]*

%%

"SET"						return SET;
"GET"						return GET;
=							return ASSIGN;
"ADD"						return ADDITION;
"SUB"						return SUBTRACTION;
"MUL"						return MULTIPLICATION;
"DIV"						return DIVISION;
"CONCAT"					return CONCATENATION;
"EXIT"						return EXIT;
,							return COMMA;
{ID}						return IDENTIFIER;
(-)?{DIGIT}+				{
								g_token_value.tvalType = INTEGER;
								g_token_value.tval.intval = atoi(yytext);
								return INTEGER;
							}

(-)?{DIGIT}+"."{DIGIT}*		{
								g_token_value.tvalType = DOUBLE;
								g_token_value.tval.dblval = atof(yytext);
								return DOUBLE;
							}

[ \t]						{
								/*
									This rule is for processing white spaces 
									(space and tab characters).
									By specifying this rule, we are telling the
									lexer to do the following :
									
									1. Discover a white space wherever it occurs. 

									2. Then do the action code associated with it.

									3. The action code (below) actually does nothing.
									so the white space is effectively ignored.
								*/
								;  
							}

\n							return NEWLINE;

"//".*                      { 
								/* 
								Single Line Comment Processing.
								The regular expression associated with this rule means : 
								on encountering a token that begins with "//" followed by 
								zero or more characters except '\n'.
								*/			
								/* Do nothing. Just consume comment */ 
							}

"##".*                      { 
								/*
								Another Single Line Comment Processing.
								The regular expression associated with this rule means :
								on encountering a token that begins with "##" followed by 
								zero or more characters except '\n'.
								*/
								/* Do nothing. Just consume comment */ 
							}

\"							{ 
								/* 
									<STRING_LIT> Rule 1. 
									- yytext contents : single " character without the rest of the string literal.
									- Preparations will be made to perform the mechanism for building up string literal buffer character by character.									
								*/
								/*
									For information on Literal String Processing and Flex Start Conditions see :
									0.11 Start conditions
									in Flex Documentation.

									According to the rules set out here in lex_specs.l,
									Flex parses string literals as follows :
									1. Take, for example the following string literal : "My friend said : \"Hello.\"\n"
									
									2. Flex will initially detect the single " character.
									- The current state is INITIAL and so <STRING_LIT> Rule 1 will be applied.
									- yytext at this time is a single " character without the rest of the string literal.
									- Flex will enter the <STRING_LIT> state and prepare the mechanism for string literal character by character buildup.
									
									3. The first part of the string literal "My friend said : " will be collected.
									- <STRING_LIT> Rule 10 will be applied.
									- yytext at this time is "My friend said : ".
									- See the comments for this rule to understand how it works.
									
									4. Flex will then detect \" string (i.e. a string of 2 characters \ followed by ").
									- <STRING_LIT> Rule 9 will be applied.
									- yytext at this time is \" (i.e. a string of 2 characters \ followed by ").
									
									5. Flex will then detect the string "Hello.".
									- <STRING_LIT> Rule 10 will be applied.
									- yytext at this time is "Hello.".

									6. Flex will then detect the next \" string (i.e. a string of 2 characters \ followed by ").
									- <STRING_LIT> Rule 9 will be applied.
									- yytext at this time is \" (i.e. a string of 2 characters \ followed by ").

									7. Flex will then detect \n (i.e. the newline character which is a single character).
									- <STRING_LIT> Rule 4 will be applied.
									- yytext at this time is \n (i.e. the newline character which is a single character).

									8. Flex will then detect the single " character. 
									- <STRING_LIT> Rule 1 will not be applied because the current state is STRING_LIT.
									- <STRING_LIT> Rule 2 will be applied.
									- The built-up string literal buffer is now ready to be used.
									- The current state is re-instated to INITIAL.
									- The STRING token type is returned from yylex().
								*/
								InitGlobalStringLitBuffer();
								BEGIN(STRING_LIT); 
							}

<STRING_LIT>\"				{ 
								/* 
									<STRING_LIT> Rule 2. 
									- yytext contents : single " character.
									- Because we are in <STRING_LIT> state, this " character is likely the enclosing " of a string literal.
								*/
								/* saw closing quote - all done */
								BEGIN(INITIAL);
								/* 
									return string constant token type and value to parser.
								*/
								g_token_value.tvalType = STRING;
								g_token_value.tval.pstrval = _strdup(GetGlobalStringLitBuffer());

								/* 
									After the Global String Literal Buffer has been copied, 
									we free it.
								*/
								FreeGlobalStringLitBuffer();
								return STRING;
							}

<STRING_LIT>\n					{
									/* 
										<STRING_LIT> Rule 3. 
										- yytext contents : single newline character.
										- We are currently in STRING_LIT state and if we encounter a newline character, this is considered an error. 
										- This is like processing a string such as "My string 
										- where a newline character is enciuntered at the end without any enclosing " character.
									*/
									/* error - unterminated string constant */
									/* generate error message */
									printf("Unexpected newline in a string literal.\n"); 
									return SYNTAX_ERROR;
								}

<STRING_LIT>\\n					{ 
									/* 
										<STRING_LIT> Rule 4.
										- yytext contents : "\n" (i.e. 2 characters '\' and 'n')
										- This indicates that the string literal intends to includes a newline in the middle.
										- E.g. "First line\nSecond Line\nThird Line\n"
										- We add in a newline character into the literal string buffer.
										- The result will be a line like this :
										"First Line
										 Second Line
										 Third Line
										"
									*/ 
									AppendCharGlobalStringLitBuffer('\n');
								}

<STRING_LIT>\\t					{ 
									/* 
										<STRING_LIT> Rule 5. 
										- yytext contents : "\t" (i.e. 2 characters '\' and 't')
										- This indicates that the string literal intends to includes a tab in the middle.
										- E.g. "First\tSecond\tThird\t"
										- We add in a tab character into the literal string buffer.
										- The result will be a line like this :
										"First	Second	Third	"
									*/ 
									AppendCharGlobalStringLitBuffer('\t');
								}

<STRING_LIT>\\r					{ 
									/* 
										<STRING_LIT> Rule 6. 
										- yytext contents : "\r" (i.e. 2 characters '\' and 'r')
										- This indicates that the string literal intends to includes a carriage return in the middle.
										- E.g. "First\rSecond\rThird\r"
										- We add in a carriage return character into the literal string buffer.
									*/ 
									AppendCharGlobalStringLitBuffer('\r');
								}

<STRING_LIT>\\b					{ 
									/* 
										<STRING_LIT> Rule 7. 
										- yytext contents : "\b" (i.e. 2 characters '\' and 'b')
										- This indicates that the string literal intends to includes a backspace character in the middle.
										- E.g. "First\bSecond\bThird\b"
										- We add in a backspace character into the literal string buffer.
									*/ 
									AppendCharGlobalStringLitBuffer('\b');
								}

<STRING_LIT>\\f					{ 
									/* 
										<STRING_LIT> Rule 8. 
										- yytext contents : "\f" (i.e. 2 characters '\' and 'f')
										- This indicates that the string literal intends to includes a formfeed character in the middle.
										- E.g. "First Page\fSecond Page\fThird Page\f"
										- We add in a formfeed character into the literal string buffer.
									*/ 
									AppendCharGlobalStringLitBuffer('\f');
								}
     
<STRING_LIT>\\(.|\n)			{
									/* 
										<STRING_LIT> Rule 9. 
										- yytext contents : "\c" (i.e. 2 characters '\' and c)
										- where c is any character considered reserved by the current language.
										- This Regular Expression is for tokens like : \=, \". 
										Basically to include a language specific reserved character.

										For a list of ASCII Escape Sequences in C, see :
										Escape sequences in C
										https://en.wikipedia.org/wiki/Escape_sequences_in_C
									*/
									AppendCharGlobalStringLitBuffer(yytext[1]);
								}
     
<STRING_LIT>[^\\\n\"]+			{
									/* 
										<STRING_LIT> Rule 10. 
										- yytext contents : characters except \, newline and ".
										- Hence any normal set of characters that can form a string.
										- Take, for example, a string literal like the following :
											"Part 1\nPart 2"
										  When Flex applies this rule yytext will be : "Part 1" and "Part 2".
										- The action code will collect the characters of yytext (one character at a time) and build up string_buf.
									*/
									AppendGlobalStringLitBuffer(yytext);
								}


	/* Must always be the last rule. */
.							{ 
								printf("Unexpected Text [%s].\n", yytext); 
								return SYNTAX_ERROR;
							}

%%

int yywrap(void)
{
	return 1;
}

void SkipAllTokensInLine()
{
	while (yylex() != NEWLINE);
}

