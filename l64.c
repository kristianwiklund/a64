#include <stdio.h>
#include <stdlib.h>
/*
Copyright (C) 2006 Kristian Wiklund

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

    */

int main(int argc, char **argv) {

  int i;
  FILE *out;
  int flag=0;
  unsigned short int pc,oldpc;

  out = fopen("prog.bin","w");

  /* really stupid linker
   */

  for(i=1;i<argc;i++)
    {
      FILE *f;
      unsigned char x,y;

      f = fopen(argv[i],"r");
      
      fread(&x, 1,1,f);
      fread(&y, 1,1,f);

      pc = y*256+x;

      fprintf(stderr, "Loading segment %s at %x\n", argv[i], pc);

      if(!flag)
	{
	  /* load address
	   */
	  fwrite(&x,1,1,out);
	  fwrite(&y,1,1,out);
	  flag=1;
	} else {
	  /* fill out with zero
	   */
	  x=0;
	  for(;oldpc<pc;oldpc++)
	    fwrite(&x,1,1,out);
	    
	}

      while(!feof(f)) {
	if(!fread(&x,1,1,f))
	  {
	    fclose(f);
	    oldpc=pc;
	    break;
	  }
	fwrite(&x,1,1,out);
	pc++;
      }

    }

  exit(0);
}
