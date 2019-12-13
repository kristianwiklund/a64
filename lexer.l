%{

/* lexer for 6502 assembler
 *
 * (C) Kristian Wiklund 2005

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

 */


#include "yaccer.h"
#include <stdio.h>
#include "mnemonics.h"
#include <string.h>

YYSTYPE yylval;
extern YYLTYPE yylloc;
int lexline=1;
int lexchar=0;

extern int pc;
extern int pass;

struct label {
	char *name;
	int pc;
	struct label *n;
};

struct label *labels=0;

#if 0
#define RETURN(x) fprintf(stderr, "%d-", lexchar); lexchar+=strlen(yytext); fprintf(stderr, "%d ", lexchar); return(x)
# else

#define RETURN(x) lexchar+=strlen(yytext); return(x)

# endif
%}


decimal [0-9][0-9]*
hex ($|0x)[0-9a-fA-F][0-9a-fA-F]*
label [a-zA-Z][a-zA-Z0-9_]*":"
string [a-zA-Z][a-zA-Z0-9_]*

%% 

"scr"     {RETURN(t_SCR);}
"byt"     {RETURN(t_DCB);}
"processor" {RETURN(t_PROCESSOR);}
"org"     {RETURN(t_PC);}
"rorg"	  {RETURN(t_RORG);}
"rend"    {RETURN(t_REND);}
"incbin"  {RETURN(t_INCBIN);}
"incprog" {RETURN(t_INCPROG);}
"jmp"     {RETURN(t_JMP);}
{decimal} {yylval.integer = atoi(yytext); RETURN(t_INTEGER);}
{hex}     { 
		int a;
		if(yytext[0] == '0') {
			sscanf(yytext+2, "%x", &a);
		} else {
			sscanf(yytext+1, "%x", &a);
		}

		yylval.integer = a;
		RETURN(t_INTEGER);
		}

{label}   {   
		char *x = (char *)strdup(yytext);
		x[strlen(x)-1] = 0; /* chomp : */
		yylval.str = x;/* */
		
		RETURN(t_LABEL);/* */
	  }
{string}  {
            if(strlen(yytext) != 3) {
		char *x;
		x = strdup(yytext);
		yylval.str = x;
		RETURN(t_IDENTIFIER);
		} else {
			struct opcode *x;
			if(x = find_mnemonic(yytext)) {
				yylval.opcode = x;
				x->u++;
				RETURN(t_MNEM);
			} else {
				char *x;
				x = strdup(yytext);
				yylval.str = x;
				RETURN(t_IDENTIFIER);	
			}
		}

          }
"%"       {RETURN(t_MOD);}
"/"       {RETURN(t_DIV);}
"*"       {RETURN(t_PC);}
"="       {RETURN(t_EQUAL);}
"#"       {RETURN(t_HASH);}
";"[^\n]*"\n" {
	yylloc.first_line = yylloc.last_line = lexline;
	yylloc.first_column = lexchar;
	yylloc.last_column = lexchar+strlen(yytext);
	lexline++; lexchar=0; return(t_EOL); /* chomp comments */}
"("       {RETURN(t_LPAR);}
")"       {RETURN(t_RPAR);}
"<"       {RETURN(t_LT);}
">"       {RETURN(t_GT);}
","[xX]   {RETURN(t_X);}
","[yY]   {RETURN(t_Y);}
[ \t]* {lexchar+=strlen(yytext);}
"\n" {	yylloc.first_line = yylloc.last_line = lexline;
	yylloc.first_column = lexchar;
	yylloc.last_column = lexchar;
	lexline++; lexchar=0; return(t_EOL);}
".byte"   {RETURN(t_BYTE);}
"dc.b"    {RETURN(t_DCB);}
"dc.w"    {RETURN(t_DCW);}
"+"       {RETURN(t_PLUS);}
"-"       {RETURN(t_MINUS);}
","       {RETURN(t_COMMA);}
"."	  {RETURN(t_DOT);}
\".\"     {yylval.integer=*(yytext+1);RETURN(t_INTEGER);} /* single char */
\"[^\"]*\" {
 		char *x = strdup(yytext+1);
		x[strlen(x)-1] = 0;
		yylval.str = x;
		RETURN(t_STRING);
           }
%%

int yywrap()
{
   return 1;
}

struct label *find_label(char *s) {
	struct label *l = labels;

	while(l) {

		if(!strcasecmp(l->name, s)) {
			return l;
		}
		l=l->n;
	}

	return 0;
}

int create_label(char *s, int v) {
	struct label *l;

	if(pass)
		return;

	if(l = find_label(s))
		if(l->pc != v)
			{
			fprintf(stderr, "Duplicate label \"%s\"\n", s);
			
			return 0;
			}

	l = malloc(sizeof(struct label));

	printf("Added label \"%s\" = $%x\n", s, v);	
	
	l->n = labels;
	labels = l;
	l->pc = v;
	l->name = s;	

	return 1;
}

int return_label(char *s) {
	
	struct label *l;

	if(!pass)
	{
		if(l = find_label(s)) 
			return l->pc;
		else
			return pc;
	}
	if(l = find_label(s)) {
		return l->pc;
	}

	throw_op(s,"Cannot find label %s");
}





