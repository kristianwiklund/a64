/*
Copyright (C) Kristian Wiklund 2006

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

    */

#ifndef _EMIT_H
#define _EMIT_H

#include "mnemonics.h"

void emit(char);
void emit_single(struct opcode *oc);
void emit_word(int arg);
void emit_byte(int arg);
void emit_string(char *);
void emit_absy(struct opcode *oc, int arg);
void emit_absx(struct opcode *oc, int arg);
void emit_zpy(struct opcode *oc, int arg);
void emit_pzpx(struct opcode *oc, int arg);
void emit_abs(struct opcode *oc, int arg);
void emit_imm(struct opcode *oc, int arg);
void throw(char *);
void throw_op(char *, char *);
void set_org(int);

#endif
