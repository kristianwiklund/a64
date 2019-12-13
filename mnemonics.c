
#include "mnemonics.h"


struct opcode mnemonics[] =
{
  {"ADC", 0x61,},
  {"AND", 0x21,},
  {"ASL", 0x02, N_IM},
  {"BCC", 0x90,},
  {"BCS", 0xB0,},
  {"BEQ", 0xF0,},
  {"BIT", 0x20,},
  {"BMI", 0x30,},
  {"BNE", 0xD0,},
  {"BPL", 0x10,},
  {"BRK", 0x00,N_ALL},
  {"BVC", 0x50,},
  {"BVS", 0x70,},
  {"CLC", 0x18,N_ALL},
  {"CLD", 0xD8,N_ALL},
  {"CLI", 0x58,N_ALL},
  {"CLV", 0xB8,N_ALL},
  {"CMP", 0xC1,},
  {"CPX", 0xE0,},
  {"CPY", 0xC0,},
  {"DEC", 0xC2, N_IM|N_A},
  {"DEX", 0xCA,N_ALL},
  {"DEY", 0x88,N_ALL},
  {"EOR", 0x41,},
  {"INC", 0xE2, N_IM|N_A},
  {"INX", 0xE8, N_ALL},
  {"INY", 0xC8, N_ALL},
  {"JMP", 0,},    /* special case */
  {"JSR", 0,},    /* special case */
  {"LDA", 0xA1,},
  {"LDX", 0xA2, N_A|N_ABX|N_ZPX},
  {"LDY", 0xA0,},
  {"LSR", 0x42,},
  {"NOP", 0xEA,N_ALL},
  {"ORA", 0x01,},
  {"PHA", 0x48,N_ALL},
  {"PHP", 0x08,N_ALL},
  {"PLA", 0x68,N_ALL},
  {"PLP", 0x28,N_ALL},
  {"ROL", 0x22, N_IM},
  {"ROR", 0x62, N_IM},
  {"RTI", 0x40, N_ALL},
  {"RTS", 0x60, N_ALL},
  {"SBC", 0xE1,},
  {"SEC", 0x38, N_ALL},
  {"SED", 0xF8,},
  {"SEI", 0x78, N_ALL},
  {"STA", 0x81, N_IM},
  {"STX", 0x82, N_IM|N_A||N_ABX|N_ABY|N_ZPX},
  {"STY", 0x80,},
  {"TAX", 0xAA,N_ALL},
  {"TAY", 0xA8,N_ALL},
  {"TSX", 0xBA,N_ALL},
  {"TXA", 0x8A,N_ALL},
  {"TXS", 0x9A,N_ALL},
  {"TYA", 0x98,N_ALL},
  {0,0,} /* end of table */
};

struct opcode *find_mnemonic(char *s)
{
  int i=0;

  while(mnemonics[i].mnemonic)
    {
      if(!strcasecmp(mnemonics[i].mnemonic, s))
	return &mnemonics[i];
      i++;
    }
 
  return 0;
}
