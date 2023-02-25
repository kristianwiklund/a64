%{
  /*

Copyright (C) 2006, Kristian Wiklund

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

    */

  #include <stdio.h>
  #include "mnemonics.h"
  #include "yyltype.h"
  #include <stdlib.h>
  #include <unistd.h>
  #include <string.h>
  extern  int create_label(char *s, int v) ;
  extern int return_label(char *s);
  extern int lexline;
  extern int pass;
  extern char *tmpfn;
  extern int yyerror(char *s);
  extern int yylex (void);
  
  int pc=-1;
  int realpc;
  int reloc=0;
  int org_is_set = 0;

  FILE *out=0;

  #include "emit.h"

%}

%locations

%token <integer> t_INTEGER
%token <str> t_LABEL t_IDENTIFIER t_STRING
%token <opcode> t_MNEM
%token t_EQUAL t_HASH t_LPAR t_RPAR t_X t_Y t_LT t_GT
%token t_BYTE t_PLUS t_MINUS t_COMMA t_DOT t_INCBIN 
%token t_DCB t_JMP t_DCW t_INCPROG t_SCR
%token t_PROCESSOR t_EOL t_RORG t_REND t_COLON
%left t_PLUS t_MINUS
%left NEG
%left t_LT t_GT
%left t_PC t_DIV t_MOD
%type <integer> expr expr1 integer

%start begin
%union {
	char *str;
	struct opcode *opcode;
	int integer;
}



%%

begin:    line t_EOL begin 
	| t_LABEL {create_label($1,pc);} line t_EOL begin
	| t_LABEL {create_label($1,pc);} t_EOL begin
	| t_EOL begin
	|
	; 		


line:		mnemonic {}
	|	pseudoop {}
	|	assignment {}
	|	include {}
	|	pragmas {}
	|	error {
#if 0
			int i;
			fprintf(stderr, "%d-%d\n", @1.first_column, @1.last_column);
			fprintf(stderr, "              ");
			
			for(i=0;i<@1.first_column;i++)
				fprintf(stderr, " ");
			for(i=@1.first_column;i<@1.last_column;i++)
				fprintf(stderr, "~");
			fprintf(stderr, "\n");
#endif
		}
	;

pseudoop: t_BYTE bytelist
	| t_DCB bytelist2
	| t_DCW wordlist2
	| t_SCR t_STRING {
				int i;

				for(i=0;i<strlen($2);i++)
				  {
				    int x = $2[i] & 0x3f;
				    
				    emit(x);
				  }
			}

  
	;

pragmas : t_PROCESSOR t_IDENTIFIER {}
	| t_PROCESSOR t_STRING {}
	| t_PROCESSOR t_INTEGER {}
	| t_PC t_EQUAL expr { set_org($3);  }
	| t_PC expr {set_org($2);}
	| t_RORG expr {printf("Begin Relocate\n"); reloc=1; pc = $2;}
	| t_REND {printf("End Relocate\n");reloc=0; pc = realpc;}
	;

bytelist : expr { emit_byte($1);} bytelist 
	 | expr { emit_byte($1);}
	 | t_STRING { emit_string($1); free($1); } bytelist 
	 | t_STRING { emit_string($1); free($1); }
	 ;

bytelist2 	: expr { emit_byte($1);} t_COMMA bytelist2 
		| expr { emit_byte($1);}
		| t_STRING { emit_string($1); free($1); } t_COMMA bytelist2
		| t_STRING { emit_string($1); free($1); }	  
		;

wordlist2	: expr {emit_word($1);} t_COMMA wordlist2
		| expr {emit_word($1);}
		;

include		: t_INCBIN t_STRING {
			FILE *inp;
			
			if(!(inp = fopen($2,"r"))) {
				perror($2);
				throw("I/O error");
			}

			while(!feof(inp)) {
				char c;
				if(!fread(&c, 1, 1, inp)) {
					fclose(inp);
					break;
				}
				emit(c); 

			}
		}
	| t_INCPROG t_STRING {
			FILE *inp;
			char c;
			
			if(!(inp = fopen($2,"r"))) {
				perror($2);
				throw("I/O error");
			}
			/* discard load address
			*/
			if(!fread(&c,1,1,inp) || 
				!fread(&c,1,1,inp)) {
				throw_op($2, "short file: %s");
				}

			while(!feof(inp)) {
				if(!fread(&c, 1, 1, inp)) {
					fclose(inp);
					break;
				}
				emit(c); 

			}
		}
		;

assignment	: t_IDENTIFIER t_EQUAL expr {create_label($1,$3);} 
		| t_LABEL  t_EQUAL expr {create_label($1,$3);} 
		;

mnemonic	:	t_MNEM { emit_single($1); }  
		|	t_MNEM t_LPAR expr1 t_X t_RPAR { emit_pzpx($1,$3);}
		|	t_MNEM t_HASH expr1 { emit_imm($1,$3);}
		|	t_MNEM expr1 { emit_abs($1,$2);}
		|	t_MNEM t_LPAR expr1 t_RPAR t_Y { emit_zpy($1,$3);}
		|	t_MNEM expr1 t_X {emit_absx($1,$2);}
		|	t_MNEM expr1 t_Y {emit_absy($1,$2);}
		|	t_JMP t_LPAR expr1 t_RPAR {emit(0x6c); emit_word($3);}
		|	t_JMP expr1 {emit(0x4c); emit_word($2);}
		;

/* we allow expressions to contain parentheses, but not at the outer
 * level when used with instructions. This reduces confusion about indirection
 */

expr	:	t_LPAR expr t_RPAR {$$=$2;}
	|	expr1
	;

expr1	:	expr t_PLUS expr {$$=$1+$3;}
	|	expr t_MINUS expr {$$=$1-$3;}
	|	t_MINUS expr %prec NEG {$$=-$2;}
	|	expr t_PC expr {$$=$1*$3;}
	|	expr t_DIV expr {$$=$1/$3;}
	|	expr t_MOD expr {$$=$1 % $3;}
	|	t_LT expr {$$=($2) & 0xff;}
	|	t_GT expr {$$=((($2)&0xff00)>>8);}
     	|	integer {$$=$1;}
	|	t_DOT {$$ = pc;}
	;

integer	:	t_INTEGER {$$=$1;}
	|	t_IDENTIFIER  {$$ = return_label($1); free($1);}
	|	t_PC {$$ = pc;}
	;

%%


void throw(char *s) {
  FILE *tin;
  int i;
  char buf[1024];
  
  if(tin = fopen(tmpfn, "r")) {
    for(i=0;i<lexline;i++)
      fgets(buf, 1024, tin);
  }
  fclose(tin);
  fprintf(stderr, buf);
  fprintf(stderr, "%d: %s\n", lexline, s);
  unlink(tmpfn);
  exit(1);
}

int yyerror(char *s)
{
  FILE *tin;
  int i;
  char buf[1024];

  if(tin = fopen(tmpfn, "r")) {
    for(i=0;i<lexline;i++)
      fgets(buf, 1024, tin);
  }
  fclose(tin);

  fprintf(stderr, "%s -> %s", s, buf);
}	


void throw_op(char *op, char *s) {
	char buf[1024];
	sprintf(buf, s, op);
	throw(buf);
}



void set_org(int newpc) {
  char buf[1024];
  
  org_is_set = 1;

  pc = newpc;
  realpc = pc;	
  if(pass) {
    if(out != 0) {
				/* a segment already exists, finish it
				 * before starting the new one	
				 */
      fclose(out);
    }
    
    sprintf(buf, "%x.seg", pc);
    printf("Starting segment at $%x\n", pc);
    if(!(out = fopen(buf, "w"))) {
      perror(buf);
      throw("I/O error");
    }
    
    fprintf(out, "%c", (pc & 0xff));	
    fprintf(out, "%c", ((pc & 0xff00) >> 8));
  }
}






