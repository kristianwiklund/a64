#ifndef _MNEMONICS_H
#define _MNEMONICS_H

struct opcode 
{
  char *mnemonic;
  unsigned char code;
  int not;        /* flags for which adressing modes not allowed           */
  int u;          /* increased every time mnemonic is used during assembly */
};

struct opcode * find_mnemonic(char *s);

#define N_ALL   0xffff /* single-byte instruction */
#define N_IM    0x01
#define N_A     0x02
#define N_ABX  0x04
#define N_ABY  0x08
#define N_ZP    0x10
#define N_ZPX   0x20
#define N_ZPY   0x40

#endif

