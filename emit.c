/*

Copyright (C) 2006 Kristian Wiklund

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

    */

#include <stdio.h>

#include "emit.h"
#include "mnemonics.h"

extern int pc;
extern int realpc;
extern FILE *out;
extern int org_is_set;
extern int reloc;
extern int pass;

void emit(char x)
{
  static int count=0;
	pc++;
        realpc++;

	if(!org_is_set) {
		throw("cannot generate code unless initial PC is set");
	}


	if(!reloc && (pc != realpc)) {
	  char buf[1024];
	  sprintf(buf, "Internal error: PC (%x) and REALPC (%x) differs outside relocated scope", pc, realpc);
	  throw(buf);
	}
	
	

	if(!pass)
		return;
	
	/*	fprintf(stderr, "%x: %2x\n", pc, x);*/
        fwrite(&x,1,1,out);

}

void emit_word(int arg)
{
	emit(arg & 0xff);
	emit((arg & 0xff00) >> 8);
}

void emit_byte(int arg)
{
	emit(arg & 0xff);
}

void emit_string(char *s) {
	int l = strlen(s);
	int i;

	for(i=0; i<l; i++) {
		emit_byte(s[i]);
	}

}

void emit_single(struct opcode *oc) {
	if(oc->not != N_ALL) {
		
		switch(oc->code) {
		/* accumulator mode
		*/
		case 0x02:
		case 0x22:
		case 0x42:
		case 0x62:
			emit(oc->code|0x08);
			return;
		default:
			throw_op(oc->mnemonic,
			"Cannot use %s as single-byte instruction");
		}
	}

	emit(oc->code);
}

void emit_absy(struct opcode *oc, int arg) {
	
	/* handle zp,y or abs,y */
	
	/* zp,y is only available for STX/LDX
	 * abs,y is not available for STX
	 * neither is available for 00 instructions
	 */
	
	if((oc->not == N_ALL) || ((oc->code & 3) == 0)) {
		throw_op(oc->mnemonic, "Cannot adress %s addr,X");
	}

	/* check if 10 instruction and STX/LDX
	*/

	if((oc->code & 3) == 2) {
		if((oc->code != 0x82) && (oc->code != 0xA2))
			throw_op(oc->mnemonic, "Cannot adress %s addr,X");
	
	/* must be stx/ldx, try to put it in zero page
	*/

		if((arg >= 0) && (arg <= 255)) {
			emit(oc->code | 0x14);
			emit_byte(arg);
			return;
		}

	/* else use ordinary addr mode, which is 111 countrary to
	 * 01 instructions where it is 110
	 */

		emit(oc->code | 0x1C);
		emit_word(arg);
		return;
	}

	emit(oc->code | 0x18);
	emit_word(arg);



}

void emit_absx(struct opcode *oc, int arg) {

	/* handle zp,x or abs,x
	 * all 01 instructions are ok
	 * all 10 instructions except stx/ldx are ok
	 * STY need zp,x
	 * LDY need both
	 */

	if(oc->not == N_ALL) {
		throw_op(oc->mnemonic, "Cannot adress %s addr,X");
	}

	/* check for stx/ldx
	*/
	if((oc->code == 0x82) || (oc->code == 0xA2)) {
		throw_op(oc->mnemonic, 
			"Cannot adress %s addr,X");	
	}

	/* check for sty/ldy 
	*/	
	if((oc->code & 3) == 0) {
		if((oc->code != 0x80) && (oc->code != 0xa0)) 
			throw_op(oc->mnemonic, 
				"Cannot adress %s addr,X (00 group)");	
	}		 
	
	if((oc->code == 0x80) && ((arg<0) || (arg>255))) {
		throw("Address not in zero page");
	}

	/* put it in zero page
	*/

	if((arg >= 0) && (arg <= 255)) {
		/* 0x14 in all modes
		*/
		emit(oc->code | 0x14);
		emit_byte(arg);
		return;
	}

	/* not in zp, 111 in all modes
	*/
	emit(oc->code | 0x1C);
	emit_word(arg);

}

void emit_zpy(struct opcode *oc, int arg) {
	/* (zp),y only usable with 01 ops
	*/
	if((oc->code & 3) != 1) {
		throw_op(oc->mnemonic,
		"Cannot adress %s (zp), Y)");
	}

	if(arg < 0 || arg > 255) {
		throw("Argument not in zero page");
	}

	emit(oc->code|0x10);
	emit_byte(arg); 
}

void emit_pzpx(struct opcode *oc, int arg) {
	
	/* (zp,x) only usable with 01 ops
	*/
	if((oc->code & 3) != 1) {
		throw_op(oc->mnemonic,
		"Cannot adress %s (zp, X)");
	}

	if(arg < 0 || arg > 255) {
		throw("Argument not in zero page");
	}

	emit(oc->code);
	emit_byte(arg); 
}

void emit_abs(struct opcode *oc, int arg) {
	
	/* "absolute"  is 011 for all variants of cc
	 * "zero page" is 001 for all variants of cc
	 *
	 * branch instructions must be handled separately
	 *
	 * then, there are exceptions for JMP and JSR, handle them
	 * then, just check the size of arg and emit suitable opcode
	 */

	if((oc->not == N_ALL))
	   throw_op(oc->mnemonic, "Cannot use %s in absolute or zp mode");


	/* branch instructions. Offset must be within range.
	 */
	switch(oc->code) {
		case 0x10: 
		case 0x30:
		case 0x50:
		case 0x70:
		case 0x90:
		case 0xB0:
		case 0xD0:
		case 0xF0:	
		{
		int off = (arg-(pc+2));
		emit(oc->code);
	
		if(off < -127 || off > 127) {
			char buf[1024];
			sprintf(buf, 
			"%s: Branch out of range (from %x to %x), use JMP",
			oc->mnemonic, pc, arg);
			throw(buf);
		}

		emit_byte(off);
		return;
		}
	}
	
	/* handle JMP and JSR
	*/

	if(!strcasecmp(oc->mnemonic, "JMP")) {
		emit(0x4C);
		emit_word(arg);
		return;
	}

	if(!strcasecmp(oc->mnemonic, "JSR")) {
		emit(0x20);
		emit_word(arg);
		return;
	}
	
	/* for remaining opcodes, zp and ab is available for all cc 
	*/
	if(!(oc->not & N_ZP))
        	if((arg >= 0) && (arg <= 255)) {
			/* zero page */	
			emit(oc->code | 0x04);
			emit_byte(arg); 
			return;	
		}

	/* ordinary absolute
	*/

	emit(oc->code | 0x0C);
	emit_word(arg);
	return;
}

void emit_imm(struct opcode *oc, int arg)
{
	/* immediate mode.
	 */

	if(oc->not & N_IM)
		throw_op(oc->mnemonic, "Cannot use %s in immediate mode");
	
	if((oc->code & 3) == 1) {
		emit(oc->code | 0x8);
	} else {
		emit(oc->code);
	}

	emit_byte(arg);
	return;
}
