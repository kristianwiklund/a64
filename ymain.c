/* to test yaccer */

/*
Copyright (C) Kristian Wiklund 2006

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

    */

#include <stdio.h>
#include "mnemonics.h"

#define GASP "/usr/local/bin/gasp -u -a -I/usr/local/lib/a64/include -c ';' -o %s %s"
#define USE_GASP

int pass=0;
extern FILE *yyin;
extern lexline;
extern int pc;
extern FILE *out;
char *tmpfn;
extern struct opcode mnemonics[];

int main(int argc, char **argv)
{

  char buf[1024];

#ifdef USE_GASP
  tmpfn = (char *)strdup("a64XXXXXX");

  printf("Preprocessing: %s\n", argv[1]);

  if(!(mkstemp(tmpfn))) {
    fprintf(stderr, "Internal error generating temporary file\n");
    exit(1);
  }

  sprintf(buf, GASP, tmpfn, argv[1]);
  if(system(buf))
    exit(1);

#else
  tmpfn = (char *)argv[1];
#endif

  printf("Pass 1: %s\n", tmpfn);
  

  yyin = fopen(tmpfn,"r");

  if(!yyin)
    {
      perror(tmpfn);
      throw("I/O error");
    }

  yyrestart(yyin);


  yyparse();

  fclose(yyin);

  printf("Pass 2: %s\n", tmpfn);

  pass=1;

  yyin = fopen(tmpfn,"r");
  yyrestart(yyin);

  if(!yyin)
    {
      perror(tmpfn);
      throw("I/O error");
    }
  lexline=1;
  pc=-1;
  yyparse();

  fclose(yyin);
  fclose(out);

  unlink(tmpfn);

  /* dump statistics
   */

#if 0 

  {
    int i=0;

    while(mnemonics[i].mnemonic) {
      printf("%4d: %s\n", mnemonics[i].u, mnemonics[i].mnemonic);
      i++;
    }
  }
  
#endif

}


