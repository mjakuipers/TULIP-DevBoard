/*
 * tracer.c
 *
 * This file is part of the TULIP4041 project.
 * Copyright (C) 2024 Meindert Kuipers
 *
 * This is free software: you are free to change and redistribute it.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * USE AT YOUR OWN RISK
 *
 */

#include "tracer.h"

const char __in_flash()*mnemonics[] = 
// const char *mnemonics[] = 
// list of HP41 mnemonics, JDA style + HP style
// forced in FLASH to maximize SRAM
{
{"NOP "},     // 0x000
{"GO/XQ "},     // 0x001
{"A=0 @R"},     // 0x002
{"JNC 0"},     // 0x003
{"CLRF 3"},     // 0x004
{"GO/XQ "},     // 0x005
{"A=0 S&X"},     // 0x006
{"JC 0"},     // 0x007
{"SETF 3"},     // 0x008
{"GO/XQ "},     // 0x009
{"A=0 R<-"},     // 0x00A
{"JNC +1"},     // 0x00B
{"?FSET 3"},     // 0x00C
{"GO/XQ "},     // 0x00D
{"A=0 ALL"},     // 0x00E
{"JC +1"},     // 0x00F
{"LD@R 0"},     // 0x010
{"GO/XQ "},     // 0x011
{"A=0 P-Q"},     // 0x012
{"JNC +2"},     // 0x013
{"?R= 3"},     // 0x014
{"GO/XQ "},     // 0x015
{"A=0 XS"},     // 0x016
{"JC +2"},     // 0x017
{"UNUSED "},     // 0x018
{"GO/XQ "},     // 0x019
{"A=0 M"},     // 0x01A
{"JNC +3"},     // 0x01B
{"R= 3"},     // 0x01C
{"GO/XQ "},     // 0x01D
{"A=0 MS"},     // 0x01E
{"JC +3"},     // 0x01F
{"XQ>GO "},     // 0x020
{"GO/XQ "},     // 0x021
{"B=0 @R"},     // 0x022
{"JNC +4"},     // 0x023
{"SELP 0"},     // 0x024
{"GO/XQ "},     // 0x025
{"B=0 S&X"},     // 0x026
{"JC +4"},     // 0x027
{"WRIT 0(T)"},     // 0x028
{"GO/XQ "},     // 0x029
{"B=0 R<-"},     // 0x02A
{"JNC +5"},     // 0x02B
{"?FI 3"},     // 0x02C
{"GO/XQ "},     // 0x02D
{"B=0 ALL"},     // 0x02E
{"JC +5"},     // 0x02F
{"ROMBLK (HEPAX)"},     // 0x030
{"GO/XQ "},     // 0x031
{"B=0 P-Q"},     // 0x032
{"JNC +6"},     // 0x033
{"UNUSED "},     // 0x034
{"GO/XQ "},     // 0x035
{"B=0 XS"},     // 0x036
{"JC +6"},     // 0x037
{"READDATA "},     // 0x038
{"GO/XQ "},     // 0x039
{"B=0 M"},     // 0x03A
{"JNC +7"},     // 0x03B
{"RCR 3"},     // 0x03C
{"GO/XQ "},     // 0x03D
{"B=0 MS"},     // 0x03E
{"JC +7"},     // 0x03F
{"WROM (MLDL)"},     // 0x040
{"GO/XQ "},     // 0x041
{"C=0 @R"},     // 0x042
{"JNC +8"},     // 0x043
{"CLRF 4"},     // 0x044
{"GO/XQ "},     // 0x045
{"C=0 S&X"},     // 0x046
{"JC +8"},     // 0x047
{"SETF 4"},     // 0x048
{"GO/XQ "},     // 0x049
{"C=0 R<-"},     // 0x04A
{"JNC +9"},     // 0x04B
{"?FSET 4"},     // 0x04C
{"GO/XQ "},     // 0x04D
{"C=0 ALL"},     // 0x04E
{"JC +9"},     // 0x04F
{"LD@R 1"},     // 0x050
{"GO/XQ "},     // 0x051
{"C=0 P-Q"},     // 0x052
{"JNC +10"},     // 0x053
{"?R= 4"},     // 0x054
{"GO/XQ "},     // 0x055
{"C=0 XS"},     // 0x056
{"JC +10"},     // 0x057
{"G=C "},     // 0x058
{"GO/XQ "},     // 0x059
{"C=0 M"},     // 0x05A
{"JNC +11"},     // 0x05B
{"R= 4"},     // 0x05C
{"GO/XQ "},     // 0x05D
{"C=0 MS"},     // 0x05E
{"JC +11"},     // 0x05F
{"POWOFF "},     // 0x060
{"GO/XQ "},     // 0x061
{"A<>B @R"},     // 0x062
{"JNC +12"},     // 0x063
{"SELP 1"},     // 0x064
{"GO/XQ "},     // 0x065
{"A<>B S&X"},     // 0x066
{"JC +12"},     // 0x067
{"WRIT 1(Z)"},     // 0x068
{"GO/XQ "},     // 0x069
{"A<>B R<-"},     // 0x06A
{"JNC +13"},     // 0x06B
{"?FI 4"},     // 0x06C
{"GO/XQ "},     // 0x06D
{"A<>B ALL"},     // 0x06E
{"JC +13"},     // 0x06F
{"N=C "},     // 0x070
{"GO/XQ "},     // 0x071
{"A<>B P-Q"},     // 0x072
{"JNC +14"},     // 0x073
{"UNUSED "},     // 0x074
{"GO/XQ "},     // 0x075
{"A<>B XS"},     // 0x076
{"JC +14"},     // 0x077
{"READ 1(Z)"},     // 0x078
{"GO/XQ "},     // 0x079
{"A<>B M"},     // 0x07A
{"JNC +15"},     // 0x07B
{"RCR 4"},     // 0x07C
{"GO/XQ "},     // 0x07D
{"A<>B MS"},     // 0x07E
{"JC +15"},     // 0x07F
{"UNUSED "},     // 0x080
{"GO/XQ "},     // 0x081
{"B=A @R"},     // 0x082
{"JNC +16"},     // 0x083
{"CLRF 5"},     // 0x084
{"GO/XQ "},     // 0x085
{"B=A S&X"},     // 0x086
{"JC +16"},     // 0x087
{"SETF 5"},     // 0x088
{"GO/XQ "},     // 0x089
{"B=A R<-"},     // 0x08A
{"JNC +17"},     // 0x08B
{"?FSET 5"},     // 0x08C
{"GO/XQ "},     // 0x08D
{"B=A ALL"},     // 0x08E
{"JC +17"},     // 0x08F
{"LD@R 2"},     // 0x090
{"GO/XQ "},     // 0x091
{"B=A P-Q"},     // 0x092
{"JNC +18"},     // 0x093
{"?R= 5"},     // 0x094
{"GO/XQ "},     // 0x095
{"B=A XS"},     // 0x096
{"JC +18"},     // 0x097
{"C=G "},     // 0x098
{"GO/XQ "},     // 0x099
{"B=A M"},     // 0x09A
{"JNC +19"},     // 0x09B
{"R= 5"},     // 0x09C
{"GO/XQ "},     // 0x09D
{"B=A MS"},     // 0x09E
{"JC +19"},     // 0x09F
{"SLCTP "},     // 0x0A0
{"GO/XQ "},     // 0x0A1
{"A<>C @R"},     // 0x0A2
{"JNC +20"},     // 0x0A3
{"SELP 2"},     // 0x0A4
{"GO/XQ "},     // 0x0A5
{"A<>C S&X"},     // 0x0A6
{"JC +20"},     // 0x0A7
{"WRIT 2(Y)"},     // 0x0A8
{"GO/XQ "},     // 0x0A9
{"A<>C R<-"},     // 0x0AA
{"JNC +21"},     // 0x0AB
{"?FI 5 ?EDAV"},     // 0x0AC
{"GO/XQ "},     // 0x0AD
{"A<>C ALL"},     // 0x0AE
{"JC +21"},     // 0x0AF
{"C=N "},     // 0x0B0
{"GO/XQ "},     // 0x0B1
{"A<>C P-Q"},     // 0x0B2
{"JNC +22"},     // 0x0B3
{"UNUSED "},     // 0x0B4
{"GO/XQ "},     // 0x0B5
{"A<>C XS"},     // 0x0B6
{"JC +22"},     // 0x0B7
{"READ 2(Y)"},     // 0x0B8
{"GO/XQ "},     // 0x0B9
{"A<>C M"},     // 0x0BA
{"JNC +23"},     // 0x0BB
{"RCR 5"},     // 0x0BC
{"GO/XQ "},     // 0x0BD
{"A<>C MS"},     // 0x0BE
{"JC +23"},     // 0x0BF
{"EADD=C (MAXX)"},     // 0x0C0
{"GO/XQ "},     // 0x0C1
{"C=B @R"},     // 0x0C2
{"JNC +24"},     // 0x0C3
{"CLRF 10"},     // 0x0C4
{"GO/XQ "},     // 0x0C5
{"C=B S&X"},     // 0x0C6
{"JC +24"},     // 0x0C7
{"SETF 10"},     // 0x0C8
{"GO/XQ "},     // 0x0C9
{"C=B R<-"},     // 0x0CA
{"JNC +25"},     // 0x0CB
{"?FSET 10"},     // 0x0CC
{"GO/XQ "},     // 0x0CD
{"C=B ALL"},     // 0x0CE
{"JC +25"},     // 0x0CF
{"LD@R 3"},     // 0x0D0
{"GO/XQ "},     // 0x0D1
{"C=B P-Q"},     // 0x0D2
{"JNC +26"},     // 0x0D3
{"?R= 10"},     // 0x0D4
{"GO/XQ "},     // 0x0D5
{"C=B XS"},     // 0x0D6
{"JC +26"},     // 0x0D7
{"C<>G "},     // 0x0D8
{"GO/XQ "},     // 0x0D9
{"C=B M"},     // 0x0DA
{"JNC +27"},     // 0x0DB
{"R= 10"},     // 0x0DC
{"GO/XQ "},     // 0x0DD
{"C=B MS"},     // 0x0DE
{"JC +27"},     // 0x0DF
{"SLCTQ "},     // 0x0E0
{"GO/XQ "},     // 0x0E1
{"B<>C @R"},     // 0x0E2
{"JNC +28"},     // 0x0E3
{"SELP 3"},     // 0x0E4
{"GO/XQ "},     // 0x0E5
{"B<>C S&X"},     // 0x0E6
{"JC +28"},     // 0x0E7
{"WRIT 3(X)"},     // 0x0E8
{"GO/XQ "},     // 0x0E9
{"B<>C R<-"},     // 0x0EA
{"JNC +29"},     // 0x0EB
{"?FI 10 ?ORAV"},     // 0x0EC
{"GO/XQ "},     // 0x0ED
{"B<>C ALL"},     // 0x0EE
{"JC +29"},     // 0x0EF
{"C<>N "},     // 0x0F0
{"GO/XQ "},     // 0x0F1
{"B<>C P-Q"},     // 0x0F2
{"JNC +30"},     // 0x0F3
{"UNUSED "},     // 0x0F4
{"GO/XQ "},     // 0x0F5
{"B<>C XS"},     // 0x0F6
{"JC +30"},     // 0x0F7
{"READ 3(X)"},     // 0x0F8
{"GO/XQ "},     // 0x0F9
{"B<>C M"},     // 0x0FA
{"JNC +31"},     // 0x0FB
{"RCR 10"},     // 0x0FC
{"GO/XQ "},     // 0x0FD
{"B<>C MS"},     // 0x0FE
{"JC +31"},     // 0x0FF
{"ENBANK1 "},     // 0x100
{"GO/XQ "},     // 0x101
{"A=C @R"},     // 0x102
{"JNC +32"},     // 0x103
{"CLRF 8"},     // 0x104
{"GO/XQ "},     // 0x105
{"A=C S&X"},     // 0x106
{"JC +32"},     // 0x107
{"SETF 8"},     // 0x108
{"GO/XQ "},     // 0x109
{"A=C R<-"},     // 0x10A
{"JNC +33"},     // 0x10B
{"?FSET 8"},     // 0x10C
{"GO/XQ "},     // 0x10D
{"A=C ALL"},     // 0x10E
{"JC +33"},     // 0x10F
{"LD@R 4"},     // 0x110
{"GO/XQ "},     // 0x111
{"A=C P-Q"},     // 0x112
{"JNC +34"},     // 0x113
{"?R= 8"},     // 0x114
{"GO/XQ "},     // 0x115
{"A=C XS"},     // 0x116
{"JC +34"},     // 0x117
{"UNUSED "},     // 0x118
{"GO/XQ "},     // 0x119
{"A=C M"},     // 0x11A
{"JNC +35"},     // 0x11B
{"R= 8"},     // 0x11C
{"GO/XQ "},     // 0x11D
{"A=C MS"},     // 0x11E
{"JC +35"},     // 0x11F
{"?P=Q "},     // 0x120
{"GO/XQ "},     // 0x121
{"A=A+B @R"},     // 0x122
{"JNC +36"},     // 0x123
{"SELP 4"},     // 0x124
{"GO/XQ "},     // 0x125
{"A=A+B S&X"},     // 0x126
{"JC +36"},     // 0x127
{"WRIT 4(L)"},     // 0x128
{"GO/XQ "},     // 0x129
{"A=A+B R<-"},     // 0x12A
{"JNC +37"},     // 0x12B
{"?FI 8 ?FRAV"},     // 0x12C
{"GO/XQ "},     // 0x12D
{"A=A+B ALL"},     // 0x12E
{"JC +37"},     // 0x12F
{"LDI "},     // 0x130
{"GO/XQ "},     // 0x131
{"A=A+B P-Q"},     // 0x132
{"JNC +38"},     // 0x133
{"UNUSED "},     // 0x134
{"GO/XQ "},     // 0x135
{"A=A+B XS"},     // 0x136
{"JC +38"},     // 0x137
{"READ 4(L)"},     // 0x138
{"GO/XQ "},     // 0x139
{"A=A+B M"},     // 0x13A
{"JNC +39"},     // 0x13B
{"RCR 8"},     // 0x13C
{"GO/XQ "},     // 0x13D
{"A=A+B MS"},     // 0x13E
{"JC +39"},     // 0x13F
{"ENBANK2 "},     // 0x140
{"GO/XQ "},     // 0x141
{"A=A+C @R"},     // 0x142
{"JNC +40"},     // 0x143
{"CLRF 6"},     // 0x144
{"GO/XQ "},     // 0x145
{"A=A+C S&X"},     // 0x146
{"JC +40"},     // 0x147
{"SETF 6"},     // 0x148
{"GO/XQ "},     // 0x149
{"A=A+C R<-"},     // 0x14A
{"JNC +41"},     // 0x14B
{"?FSET 6"},     // 0x14C
{"GO/XQ "},     // 0x14D
{"A=A+C ALL"},     // 0x14E
{"JC +41"},     // 0x14F
{"LD@R 5"},     // 0x150
{"GO/XQ "},     // 0x151
{"A=A+C P-Q"},     // 0x152
{"JNC +42"},     // 0x153
{"?R= 6"},     // 0x154
{"GO/XQ "},     // 0x155
{"A=A+C XS"},     // 0x156
{"JC +42"},     // 0x157
{"M=C "},     // 0x158
{"GO/XQ "},     // 0x159
{"A=A+C M"},     // 0x15A
{"JNC +43"},     // 0x15B
{"R= 6"},     // 0x15C
{"GO/XQ "},     // 0x15D
{"A=A+C MS"},     // 0x15E
{"JC +43"},     // 0x15F
{"?LOWBAT "},     // 0x160
{"GO/XQ "},     // 0x161
{"A=A+1 @R"},     // 0x162
{"JNC +44"},     // 0x163
{"SELP 5"},     // 0x164
{"GO/XQ "},     // 0x165
{"A=A+1 S&X"},     // 0x166
{"JC +44"},     // 0x167
{"WRIT 5(M)"},     // 0x168
{"GO/XQ "},     // 0x169
{"A=A+1 R<-"},     // 0x16A
{"JNC +45"},     // 0x16B
{"?FI 6 ?IFCR"},     // 0x16C
{"GO/XQ "},     // 0x16D
{"A=A+1 ALL"},     // 0x16E
{"JC +45"},     // 0x16F
{"PUSHADR "},     // 0x170
{"GO/XQ "},     // 0x171
{"A=A+1 P-Q"},     // 0x172
{"JNC +46"},     // 0x173
{"UNUSED "},     // 0x174
{"GO/XQ "},     // 0x175
{"A=A+1 XS"},     // 0x176
{"JC +46"},     // 0x177
{"READ 5(M)"},     // 0x178
{"GO/XQ "},     // 0x179
{"A=A+1 M"},     // 0x17A
{"JNC +47"},     // 0x17B
{"RCR 6"},     // 0x17C
{"GO/XQ "},     // 0x17D
{"A=A+1 MS"},     // 0x17E
{"JC +47"},     // 0x17F
{"ENBANK3 "},     // 0x180
{"GO/XQ "},     // 0x181
{"A=A-B @R"},     // 0x182
{"JNC +48"},     // 0x183
{"CLRF 11"},     // 0x184
{"GO/XQ "},     // 0x185
{"A=A-B S&X"},     // 0x186
{"JC +48"},     // 0x187
{"SETF 11"},     // 0x188
{"GO/XQ "},     // 0x189
{"A=A-B R<-"},     // 0x18A
{"JNC +49"},     // 0x18B
{"?FSET 11"},     // 0x18C
{"GO/XQ "},     // 0x18D
{"A=A-B ALL"},     // 0x18E
{"JC +49"},     // 0x18F
{"LD@R 6"},     // 0x190
{"GO/XQ "},     // 0x191
{"A=A-B P-Q"},     // 0x192
{"JNC +50"},     // 0x193
{"?R= 11"},     // 0x194
{"GO/XQ "},     // 0x195
{"A=A-B XS"},     // 0x196
{"JC +50"},     // 0x197
{"C=M "},     // 0x198
{"GO/XQ "},     // 0x199
{"A=A-B M"},     // 0x19A
{"JNC +51"},     // 0x19B
{"R= 11"},     // 0x19C
{"GO/XQ "},     // 0x19D
{"A=A-B MS"},     // 0x19E
{"JC +51"},     // 0x19F
{"A=B=C=0 "},     // 0x1A0
{"GO/XQ "},     // 0x1A1
{"A=A-1 @R"},     // 0x1A2
{"JNC +52"},     // 0x1A3
{"SELP 6"},     // 0x1A4
{"GO/XQ "},     // 0x1A5
{"A=A-1 S&X"},     // 0x1A6
{"JC +52"},     // 0x1A7
{"WRIT 6(N)"},     // 0x1A8
{"GO/XQ "},     // 0x1A9
{"A=A-1 R<-"},     // 0x1AA
{"JNC +53"},     // 0x1AB
{"?FI 11 ?TFAIL"},     // 0x1AC
{"GO/XQ "},     // 0x1AD
{"A=A-1 ALL"},     // 0x1AE
{"JC +53"},     // 0x1AF
{"POPADR "},     // 0x1B0
{"GO/XQ "},     // 0x1B1
{"A=A-1 P-Q"},     // 0x1B2
{"JNC +54"},     // 0x1B3
{"UNUSED "},     // 0x1B4
{"GO/XQ "},     // 0x1B5
{"A=A-1 XS"},     // 0x1B6
{"JC +54"},     // 0x1B7
{"READ 6(N)"},     // 0x1B8
{"GO/XQ "},     // 0x1B9
{"A=A-1 M"},     // 0x1BA
{"JNC +55"},     // 0x1BB
{"RCR 11"},     // 0x1BC
{"GO/XQ "},     // 0x1BD
{"A=A-1 MS"},     // 0x1BE
{"JC +55"},     // 0x1BF
{"ENBANK4 "},     // 0x1C0
{"GO/XQ "},     // 0x1C1
{"A=A-C @R"},     // 0x1C2
{"JNC +56"},     // 0x1C3
{"UNUSED "},     // 0x1C4
{"GO/XQ "},     // 0x1C5
{"A=A-C S&X"},     // 0x1C6
{"JC +56"},     // 0x1C7
{"UNUSED "},     // 0x1C8
{"GO/XQ "},     // 0x1C9
{"A=A-C R<-"},     // 0x1CA
{"JNC +57"},     // 0x1CB
{"UNUSED "},     // 0x1CC
{"GO/XQ "},     // 0x1CD
{"A=A-C ALL"},     // 0x1CE
{"JC +57"},     // 0x1CF
{"LD@R 7"},     // 0x1D0
{"GO/XQ "},     // 0x1D1
{"A=A-C P-Q"},     // 0x1D2
{"JNC +58"},     // 0x1D3
{"UNUSED "},     // 0x1D4
{"GO/XQ "},     // 0x1D5
{"A=A-C XS"},     // 0x1D6
{"JC +58"},     // 0x1D7
{"C<>M "},     // 0x1D8
{"GO/XQ "},     // 0x1D9
{"A=A-C M"},     // 0x1DA
{"JNC +59"},     // 0x1DB
{"UNUSED "},     // 0x1DC
{"GO/XQ "},     // 0x1DD
{"A=A-C MS"},     // 0x1DE
{"JC +59"},     // 0x1DF
{"GOTOADR "},     // 0x1E0
{"GO/XQ "},     // 0x1E1
{"C=C+C @R"},     // 0x1E2
{"JNC +60"},     // 0x1E3
{"SELP 7"},     // 0x1E4
{"GO/XQ "},     // 0x1E5
{"C=C+C S&X"},     // 0x1E6
{"JC +60"},     // 0x1E7
{"WRIT 7(O)"},     // 0x1E8
{"GO/XQ "},     // 0x1E9
{"C=C+C R<-"},     // 0x1EA
{"JNC +61"},     // 0x1EB
{"UNUSED "},     // 0x1EC
{"GO/XQ "},     // 0x1ED
{"C=C+C ALL"},     // 0x1EE
{"JC +61"},     // 0x1EF
{"WPTOG (HEPAX)"},     // 0x1F0
{"GO/XQ "},     // 0x1F1
{"C=C+C P-Q"},     // 0x1F2
{"JNC +62"},     // 0x1F3
{"UNUSED "},     // 0x1F4
{"GO/XQ "},     // 0x1F5
{"C=C+C XS"},     // 0x1F6
{"JC +62"},     // 0x1F7
{"READ 7(O)"},     // 0x1F8
{"GO/XQ "},     // 0x1F9
{"C=C+C M"},     // 0x1FA
{"JNC +63"},     // 0x1FB
{"WCMD (41CL)"},     // 0x1FC
{"GO/XQ "},     // 0x1FD
{"C=C+C MS"},     // 0x1FE
{"JC +63"},     // 0x1FF
{"HPIL=C 0"},     // 0x200
{"GO/XQ "},     // 0x201
{"C=C+A @R"},     // 0x202
{"JNC -64"},     // 0x203
{"CLRF 2"},     // 0x204
{"GO/XQ "},     // 0x205
{"C=C+A S&X"},     // 0x206
{"JC -64"},     // 0x207
{"SETF 2"},     // 0x208
{"GO/XQ "},     // 0x209
{"C=C+A R<-"},     // 0x20A
{"JNC -63"},     // 0x20B
{"?FSET 2"},     // 0x20C
{"GO/XQ "},     // 0x20D
{"C=C+A ALL"},     // 0x20E
{"JC -63"},     // 0x20F
{"LD@R 8"},     // 0x210
{"GO/XQ "},     // 0x211
{"C=C+A P-Q"},     // 0x212
{"JNC -62"},     // 0x213
{"?R= 2"},     // 0x214
{"GO/XQ "},     // 0x215
{"C=C+A XS"},     // 0x216
{"JC -62"},     // 0x217
{"UNUSED "},     // 0x218
{"GO/XQ "},     // 0x219
{"C=C+A M"},     // 0x21A
{"JNC -61"},     // 0x21B
{"R= 2"},     // 0x21C
{"GO/XQ "},     // 0x21D
{"C=C+A MS"},     // 0x21E
{"JC -61"},     // 0x21F
{"C=KEY "},     // 0x220
{"GO/XQ "},     // 0x221
{"C=C+1 @R"},     // 0x222
{"JNC -60"},     // 0x223
{"SELP 8"},     // 0x224
{"GO/XQ "},     // 0x225
{"C=C+1 S&X"},     // 0x226
{"JC -60"},     // 0x227
{"WRIT 8(P)"},     // 0x228
{"GO/XQ "},     // 0x229
{"C=C+1 R<-"},     // 0x22A
{"JNC -59"},     // 0x22B
{"?FI 2 ?WNDB"},     // 0x22C
{"GO/XQ "},     // 0x22D
{"C=C+1 ALL"},     // 0x22E
{"JC -59"},     // 0x22F
{"GTOKEY "},     // 0x230
{"GO/XQ "},     // 0x231
{"C=C+1 P-Q"},     // 0x232
{"JNC -58"},     // 0x233
{"UNUSED "},     // 0x234
{"GO/XQ "},     // 0x235
{"C=C+1 XS"},     // 0x236
{"JC -58"},     // 0x237
{"READ 8(P)"},     // 0x238
{"GO/XQ "},     // 0x239
{"C=C+1 M"},     // 0x23A
{"JNC -57"},     // 0x23B
{"RCR 2"},     // 0x23C
{"GO/XQ "},     // 0x23D
{"C=C+1 MS"},     // 0x23E
{"JC -57"},     // 0x23F
{"HPIL=C 1"},     // 0x240
{"GO/XQ "},     // 0x241
{"C=A-C @R"},     // 0x242
{"JNC -56"},     // 0x243
{"CLRF 9"},     // 0x244
{"GO/XQ "},     // 0x245
{"C=A-C S&X"},     // 0x246
{"JC -56"},     // 0x247
{"SETF 9"},     // 0x248
{"GO/XQ "},     // 0x249
{"C=A-C R<-"},     // 0x24A
{"JNC -55"},     // 0x24B
{"?FSET 9"},     // 0x24C
{"GO/XQ "},     // 0x24D
{"C=A-C ALL"},     // 0x24E
{"JC -55"},     // 0x24F
{"LD@R 9"},     // 0x250
{"GO/XQ "},     // 0x251
{"C=A-C P-Q"},     // 0x252
{"JNC -54"},     // 0x253
{"?R= 9"},     // 0x254
{"GO/XQ "},     // 0x255
{"C=A-C XS"},     // 0x256
{"JC -54"},     // 0x257
{"T=ST "},     // 0x258
{"GO/XQ "},     // 0x259
{"C=A-C M"},     // 0x25A
{"JNC -53"},     // 0x25B
{"R= 9"},     // 0x25C
{"GO/XQ "},     // 0x25D
{"C=A-C MS"},     // 0x25E
{"JC -53"},     // 0x25F
{"SETHEX "},     // 0x260
{"GO/XQ "},     // 0x261
{"C=C-1 @R"},     // 0x262
{"JNC -52"},     // 0x263
{"SELP 9"},     // 0x264
{"GO/XQ "},     // 0x265
{"C=C-1 S&X"},     // 0x266
{"JC -52"},     // 0x267
{"WRIT 9(Q)"},     // 0x268
{"GO/XQ "},     // 0x269
{"C=C-1 R<-"},     // 0x26A
{"JNC -51"},     // 0x26B
{"?FI 9 ?FRNS"},     // 0x26C
{"GO/XQ "},     // 0x26D
{"C=C-1 ALL"},     // 0x26E
{"JC -51"},     // 0x26F
{"RAMSLCT "},     // 0x270
{"GO/XQ "},     // 0x271
{"C=C-1 P-Q"},     // 0x272
{"JNC -50"},     // 0x273
{"UNUSED "},     // 0x274
{"GO/XQ "},     // 0x275
{"C=C-1 XS"},     // 0x276
{"JC -50"},     // 0x277
{"READ 9(Q)"},     // 0x278
{"GO/XQ "},     // 0x279
{"C=C-1 M"},     // 0x27A
{"JNC -49"},     // 0x27B
{"RCR 9"},     // 0x27C
{"GO/XQ "},     // 0x27D
{"C=C-1 MS"},     // 0x27E
{"JC -49"},     // 0x27F
{"HPIL=C 2"},     // 0x280
{"GO/XQ "},     // 0x281
{"C=0-C @R"},     // 0x282
{"JNC -48"},     // 0x283
{"CLRF 7"},     // 0x284
{"GO/XQ "},     // 0x285
{"C=0-C S&X"},     // 0x286
{"JC -48"},     // 0x287
{"SETF 7"},     // 0x288
{"GO/XQ "},     // 0x289
{"C=0-C R<-"},     // 0x28A
{"JNC -47"},     // 0x28B
{"?FSET 7"},     // 0x28C
{"GO/XQ "},     // 0x28D
{"C=0-C ALL"},     // 0x28E
{"JC -47"},     // 0x28F
{"LD@R A"},     // 0x290
{"GO/XQ "},     // 0x291
{"C=0-C P-Q"},     // 0x292
{"JNC -46"},     // 0x293
{"?R= 7"},     // 0x294
{"GO/XQ "},     // 0x295
{"C=0-C XS"},     // 0x296
{"JC -46"},     // 0x297
{"ST=T "},     // 0x298
{"GO/XQ "},     // 0x299
{"C=0-C M"},     // 0x29A
{"JNC -45"},     // 0x29B
{"R= 7"},     // 0x29C
{"GO/XQ "},     // 0x29D
{"C=0-C MS"},     // 0x29E
{"JC -45"},     // 0x29F
{"SETDEC "},     // 0x2A0
{"GO/XQ "},     // 0x2A1
{"C=-C-1 @R"},     // 0x2A2
{"JNC -44"},     // 0x2A3
{"SELP A"},     // 0x2A4
{"GO/XQ "},     // 0x2A5
{"C=-C-1 S&X"},     // 0x2A6
{"JC -44"},     // 0x2A7
{"WRIT 10(+)"},     // 0x2A8
{"GO/XQ "},     // 0x2A9
{"C=-C-1 R<-"},     // 0x2AA
{"JNC -43"},     // 0x2AB
{"?FI 7 ?SRQR"},     // 0x2AC
{"GO/XQ "},     // 0x2AD
{"C=-C-1 ALL"},     // 0x2AE
{"JC -43"},     // 0x2AF
{"UNUSED "},     // 0x2B0
{"GO/XQ "},     // 0x2B1
{"C=-C-1 P-Q"},     // 0x2B2
{"JNC -42"},     // 0x2B3
{"UNUSED "},     // 0x2B4
{"GO/XQ "},     // 0x2B5
{"C=-C-1 XS"},     // 0x2B6
{"JC -42"},     // 0x2B7
{"READ 10(+)"},     // 0x2B8
{"GO/XQ "},     // 0x2B9
{"C=-C-1 M"},     // 0x2BA
{"JNC -41"},     // 0x2BB
{"RCR 7"},     // 0x2BC
{"GO/XQ "},     // 0x2BD
{"C=-C-1 MS"},     // 0x2BE
{"JC -41"},     // 0x2BF
{"HPIL=C 3"},     // 0x2C0
{"GO/XQ "},     // 0x2C1
{"?B#0 @R"},     // 0x2C2
{"JNC -40"},     // 0x2C3
{"CLRF 13"},     // 0x2C4
{"GO/XQ "},     // 0x2C5
{"?B#0 S&X"},     // 0x2C6
{"JC -40"},     // 0x2C7
{"SETF 13"},     // 0x2C8
{"GO/XQ "},     // 0x2C9
{"?B#0 R<-"},     // 0x2CA
{"JNC -39"},     // 0x2CB
{"?FSET 13"},     // 0x2CC
{"GO/XQ "},     // 0x2CD
{"?B#0 ALL"},     // 0x2CE
{"JC -39"},     // 0x2CF
{"LD@R B"},     // 0x2D0
{"GO/XQ "},     // 0x2D1
{"?B#0 P-Q"},     // 0x2D2
{"JNC -38"},     // 0x2D3
{"?R= 13"},     // 0x2D4
{"GO/XQ "},     // 0x2D5
{"?B#0 XS"},     // 0x2D6
{"JC -38"},     // 0x2D7
{"ST<>T "},     // 0x2D8
{"GO/XQ "},     // 0x2D9
{"?B#0 M"},     // 0x2DA
{"JNC -37"},     // 0x2DB
{"R= 13"},     // 0x2DC
{"GO/XQ "},     // 0x2DD
{"?B#0 MS"},     // 0x2DE
{"JC -37"},     // 0x2DF
{"DSPOFF "},     // 0x2E0
{"GO/XQ "},     // 0x2E1
{"?C#0 @R"},     // 0x2E2
{"JNC -36"},     // 0x2E3
{"SELP B"},     // 0x2E4
{"GO/XQ "},     // 0x2E5
{"?C#0 S&X"},     // 0x2E6
{"JC -36"},     // 0x2E7
{"WRIT 11(a)"},     // 0x2E8
{"GO/XQ "},     // 0x2E9
{"?C#0 R<-"},     // 0x2EA
{"JNC -35"},     // 0x2EB
{"?FI 13 ?SERV"},     // 0x2EC
{"GO/XQ "},     // 0x2ED
{"?C#0 ALL"},     // 0x2EE
{"JC -35"},     // 0x2EF
{"WRITDAT "},     // 0x2F0
{"GO/XQ "},     // 0x2F1
{"?C#0 P-Q"},     // 0x2F2
{"JNC -34"},     // 0x2F3
{"UNUSED "},     // 0x2F4
{"GO/XQ "},     // 0x2F5
{"?C#0 XS"},     // 0x2F6
{"JC -34"},     // 0x2F7
{"READ 11(a)"},     // 0x2F8
{"GO/XQ "},     // 0x2F9
{"?C#0 M"},     // 0x2FA
{"JNC -33"},     // 0x2FB
{"RCR 13"},     // 0x2FC
{"GO/XQ "},     // 0x2FD
{"?C#0 MS"},     // 0x2FE
{"JC -33"},     // 0x2FF
{"HPIL=C 4"},     // 0x300
{"GO/XQ "},     // 0x301
{"?A<C @R"},     // 0x302
{"JNC -32"},     // 0x303
{"CLRF 1"},     // 0x304
{"GO/XQ "},     // 0x305
{"?A<C S&X"},     // 0x306
{"JC -32"},     // 0x307
{"SETF 1"},     // 0x308
{"GO/XQ "},     // 0x309
{"?A<C R<-"},     // 0x30A
{"JNC -31"},     // 0x30B
{"?FSET 1"},     // 0x30C
{"GO/XQ "},     // 0x30D
{"?A<C ALL"},     // 0x30E
{"JC -31"},     // 0x30F
{"LD@R C"},     // 0x310
{"GO/XQ "},     // 0x311
{"?A<C P-Q"},     // 0x312
{"JNC -30"},     // 0x313
{"?R= 1"},     // 0x314
{"GO/XQ "},     // 0x315
{"?A<C XS"},     // 0x316
{"JC -30"},     // 0x317
{"UNUSED "},     // 0x318
{"GO/XQ "},     // 0x319
{"?A<C M"},     // 0x31A
{"JNC -29"},     // 0x31B
{"R= 1"},     // 0x31C
{"GO/XQ "},     // 0x31D
{"?A<C MS"},     // 0x31E
{"JC -29"},     // 0x31F
{"DSPTOG "},     // 0x320
{"GO/XQ "},     // 0x321
{"?A<B @R"},     // 0x322
{"JNC -28"},     // 0x323
{"SELP C"},     // 0x324
{"GO/XQ "},     // 0x325
{"?A<B S&X"},     // 0x326
{"JC -28"},     // 0x327
{"WRIT 12(b)"},     // 0x328
{"GO/XQ "},     // 0x329
{"?A<B R<-"},     // 0x32A
{"JNC -27"},     // 0x32B
{"?FI 1 ?CRDR"},     // 0x32C
{"GO/XQ "},     // 0x32D
{"?A<B ALL"},     // 0x32E
{"JC -27"},     // 0x32F
{"FETCH S&X "},     // 0x330
{"GO/XQ "},     // 0x331
{"?A<B P-Q"},     // 0x332
{"JNC -26"},     // 0x333
{"UNUSED "},     // 0x334
{"GO/XQ "},     // 0x335
{"?A<B XS"},     // 0x336
{"JC -26"},     // 0x337
{"READ 12(b)"},     // 0x338
{"GO/XQ "},     // 0x339
{"?A<B M"},     // 0x33A
{"JNC -25"},     // 0x33B
{"RCR 1"},     // 0x33C
{"GO/XQ "},     // 0x33D
{"?A<B MS"},     // 0x33E
{"JC -25"},     // 0x33F
{"HPIL=C 5"},     // 0x340
{"GO/XQ "},     // 0x341
{"?A#0 @R"},     // 0x342
{"JNC -24"},     // 0x343
{"CLRF 12"},     // 0x344
{"GO/XQ "},     // 0x345
{"?A#0 S&X"},     // 0x346
{"JC -24"},     // 0x347
{"SETF 12"},     // 0x348
{"GO/XQ "},     // 0x349
{"?A#0 R<-"},     // 0x34A
{"JNC -23"},     // 0x34B
{"?FSET 12"},     // 0x34C
{"GO/XQ "},     // 0x34D
{"?A#0 ALL"},     // 0x34E
{"JC -23"},     // 0x34F
{"LD@R D"},     // 0x350
{"GO/XQ "},     // 0x351
{"?A#0 P-Q"},     // 0x352
{"JNC -22"},     // 0x353
{"?R= 12"},     // 0x354
{"GO/XQ "},     // 0x355
{"?A#0 XS"},     // 0x356
{"JC -22"},     // 0x357
{"ST=C "},     // 0x358
{"GO/XQ "},     // 0x359
{"?A#0 M"},     // 0x35A
{"JNC -21"},     // 0x35B
{"R= 12"},     // 0x35C
{"GO/XQ "},     // 0x35D
{"?A#0 MS"},     // 0x35E
{"JC -21"},     // 0x35F
{"?C RTN "},     // 0x360
{"GO/XQ "},     // 0x361
{"?A#C @R"},     // 0x362
{"JNC -20"},     // 0x363
{"SELP D"},     // 0x364
{"GO/XQ "},     // 0x365
{"?A#C S&X"},     // 0x366
{"JC -20"},     // 0x367
{"WRIT 13(c)"},     // 0x368
{"GO/XQ "},     // 0x369
{"?A#C R<-"},     // 0x36A
{"JNC -19"},     // 0x36B
{"?FI 12 ?ALM"},     // 0x36C
{"GO/XQ "},     // 0x36D
{"?A#C ALL"},     // 0x36E
{"JC -19"},     // 0x36F
{"C=C OR A "},     // 0x370
{"GO/XQ "},     // 0x371
{"?A#C P-Q"},     // 0x372
{"JNC -18"},     // 0x373
{"UNUSED "},     // 0x374
{"GO/XQ "},     // 0x375
{"?A#C XS"},     // 0x376
{"JC -18"},     // 0x377
{"READ 13(c)"},     // 0x378
{"GO/XQ "},     // 0x379
{"?A#C M"},     // 0x37A
{"JNC -17"},     // 0x37B
{"RCR 12"},     // 0x37C
{"GO/XQ "},     // 0x37D
{"?A#C MS"},     // 0x37E
{"JC -17"},     // 0x37F
{"HPIL=C 6"},     // 0x380
{"GO/XQ "},     // 0x381
{"RSHFA @R"},     // 0x382
{"JNC -16"},     // 0x383
{"CLRF 0"},     // 0x384
{"GO/XQ "},     // 0x385
{"RSHFA S&X"},     // 0x386
{"JC -16"},     // 0x387
{"SETF 0"},     // 0x388
{"GO/XQ "},     // 0x389
{"RSHFA R<-"},     // 0x38A
{"JNC -15"},     // 0x38B
{"?FSET 0"},     // 0x38C
{"GO/XQ "},     // 0x38D
{"RSHFA ALL"},     // 0x38E
{"JC -15"},     // 0x38F
{"LD@R E"},     // 0x390
{"GO/XQ "},     // 0x391
{"RSHFA P-Q"},     // 0x392
{"JNC -14"},     // 0x393
{"?R= 0"},     // 0x394
{"GO/XQ "},     // 0x395
{"RSHFA XS"},     // 0x396
{"JC -14"},     // 0x397
{"C=ST "},     // 0x398
{"GO/XQ "},     // 0x399
{"RSHFA M"},     // 0x39A
{"JNC -13"},     // 0x39B
{"R= 0"},     // 0x39C
{"GO/XQ "},     // 0x39D
{"RSHFA MS"},     // 0x39E
{"JC -13"},     // 0x39F
{"?NC RTN "},     // 0x3A0
{"GO/XQ "},     // 0x3A1
{"RSHFB @R"},     // 0x3A2
{"JNC -12"},     // 0x3A3
{"SELP E"},     // 0x3A4
{"GO/XQ "},     // 0x3A5
{"RSHFB S&X"},     // 0x3A6
{"JC -12"},     // 0x3A7
{"WRIT 14(d)"},     // 0x3A8
{"GO/XQ "},     // 0x3A9
{"RSHFB R<-"},     // 0x3AA
{"JNC -11"},     // 0x3AB
{"?FI 0 ?PBSY"},     // 0x3AC
{"GO/XQ "},     // 0x3AD
{"RSHFB ALL"},     // 0x3AE
{"JC -11"},     // 0x3AF
{"C=C AND A "},     // 0x3B0
{"GO/XQ "},     // 0x3B1
{"RSHFB P-Q"},     // 0x3B2
{"JNC -10"},     // 0x3B3
{"UNUSED "},     // 0x3B4
{"GO/XQ "},     // 0x3B5
{"RSHFB XS"},     // 0x3B6
{"JC -10"},     // 0x3B7
{"READ 14(d)"},     // 0x3B8
{"GO/XQ "},     // 0x3B9
{"RSHFB M"},     // 0x3BA
{"JNC -9"},     // 0x3BB
{"RCR 0"},     // 0x3BC
{"GO/XQ "},     // 0x3BD
{"RSHFB MS"},     // 0x3BE
{"JC -9"},     // 0x3BF
{"HPIL=C 7"},     // 0x3C0
{"GO/XQ "},     // 0x3C1
{"RSHFC @R"},     // 0x3C2
{"JNC -8"},     // 0x3C3
{"ST=0 "},     // 0x3C4
{"GO/XQ "},     // 0x3C5
{"RSHFC S&X"},     // 0x3C6
{"JC -8"},     // 0x3C7
{"CLRKEY "},     // 0x3C8
{"GO/XQ "},     // 0x3C9
{"RSHFC R<-"},     // 0x3CA
{"JNC -7"},     // 0x3CB
{"?KEY "},     // 0x3CC
{"GO/XQ "},     // 0x3CD
{"RSHFC ALL"},     // 0x3CE
{"JC -7"},     // 0x3CF
{"LD@R F"},     // 0x3D0
{"GO/XQ "},     // 0x3D1
{"RSHFC P-Q"},     // 0x3D2
{"JNC -6"},     // 0x3D3
{"R=R-1 "},     // 0x3D4
{"GO/XQ "},     // 0x3D5
{"RSHFC XS"},     // 0x3D6
{"JC -6"},     // 0x3D7
{"C<>ST "},     // 0x3D8
{"GO/XQ "},     // 0x3D9
{"RSHFC M"},     // 0x3DA
{"JNC -5"},     // 0x3DB
{"R=R+1 "},     // 0x3DC
{"GO/XQ "},     // 0x3DD
{"RSHFC MS"},     // 0x3DE
{"JC -5"},     // 0x3DF
{"RTN "},     // 0x3E0
{"GO/XQ "},     // 0x3E1
{"LSHFA @R"},     // 0x3E2
{"JNC -4"},     // 0x3E3
{"SELP F"},     // 0x3E4
{"GO/XQ "},     // 0x3E5
{"LSHFA S&X"},     // 0x3E6
{"JC -4"},     // 0x3E7
{"WRIT 15(e)"},     // 0x3E8
{"GO/XQ "},     // 0x3E9
{"LSHFA R<-"},     // 0x3EA
{"JNC -3"},     // 0x3EB
{"UNUSED "},     // 0x3EC
{"GO/XQ "},     // 0x3ED
{"LSHFA ALL"},     // 0x3EE
{"JC -3"},     // 0x3EF
{"PRPHSLCT "},     // 0x3F0
{"GO/XQ "},     // 0x3F1
{"LSHFA P-Q"},     // 0x3F2
{"JNC -2"},     // 0x3F3
{"UNUSED "},     // 0x3F4
{"GO/XQ "},     // 0x3F5
{"LSHFA XS"},     // 0x3F6
{"JC -2"},     // 0x3F7
{"READ 15(e)"},     // 0x3F8
{"GO/XQ "},     // 0x3F9
{"LSHFA M"},     // 0x3FA
{"JNC -1"},     // 0x3FB
{"UNUSED "},     // 0x3FC
{"GO/XQ "},     // 0x3FD
{"LSHFA MS"},     // 0x3FE
{"JC -1"},     // 0x3FF



// *******************************************************
// After this line are the HP type mnemonics
//********************************************************



{"NOP "},     // 0x000
{"GO/GSUB "},     // 0x001
{"A=0 PT"},     // 0x002
{"GONC 0"},     // 0x003
{"ST=0 3"},     // 0x004
{"GO/GSUB "},     // 0x005
{"A=0 X"},     // 0x006
{"GOC 0"},     // 0x007
{"ST=1 3"},     // 0x008
{"GO/GSUB "},     // 0x009
{"A=0 WPT"},     // 0x00A
{"GONC +1"},     // 0x00B
{"ST=1? 3"},     // 0x00C
{"GO/GSUB "},     // 0x00D
{"A=0 W"},     // 0x00E
{"GOC +1"},     // 0x00F
{"LC 0"},     // 0x010
{"GO/GSUB "},     // 0x011
{"A=0 PQ"},     // 0x012
{"GONC +2"},     // 0x013
{"?PT= 3"},     // 0x014
{"GO/GSUB "},     // 0x015
{"A=0 XS"},     // 0x016
{"GOC +2"},     // 0x017
{"UNUSED "},     // 0x018
{"GO/GSUB "},     // 0x019
{"A=0 M"},     // 0x01A
{"GONC +3"},     // 0x01B
{"?PT= 3"},     // 0x01C
{"GO/GSUB "},     // 0x01D
{"A=0 S"},     // 0x01E
{"GOC +3"},     // 0x01F
{"SPOPND "},     // 0x020
{"GO/GSUB "},     // 0x021
{"B=0 PT"},     // 0x022
{"GONC +4"},     // 0x023
{"SELPF 0"},     // 0x024
{"GO/GSUB "},     // 0x025
{"B=0 X"},     // 0x026
{"GOC +4"},     // 0x027
{"REGN=C 0(T)"},     // 0x028
{"GO/GSUB "},     // 0x029
{"B=0 WPT"},     // 0x02A
{"GONC +5"},     // 0x02B
{"?PFLG3=1 "},     // 0x02C
{"GO/GSUB "},     // 0x02D
{"B=0 W"},     // 0x02E
{"GOC +5"},     // 0x02F
{"ROMBLK (HEPAX)"},     // 0x030
{"GO/GSUB "},     // 0x031
{"B=0 PQ"},     // 0x032
{"GONC +6"},     // 0x033
{"UNUSED "},     // 0x034
{"GO/GSUB "},     // 0x035
{"B=0 XS"},     // 0x036
{"GOC +6"},     // 0x037
{"C=DATA "},     // 0x038
{"GO/GSUB "},     // 0x039
{"B=0 M"},     // 0x03A
{"GONC +7"},     // 0x03B
{"RCR 3"},     // 0x03C
{"GO/GSUB "},     // 0x03D
{"B=0 S"},     // 0x03E
{"GOC +7"},     // 0x03F
{"WMLDL "},     // 0x040
{"GO/GSUB "},     // 0x041
{"C=0 PT"},     // 0x042
{"GONC +8"},     // 0x043
{"ST=0 4"},     // 0x044
{"GO/GSUB "},     // 0x045
{"C=0 X"},     // 0x046
{"GOC +8"},     // 0x047
{"ST=1 4"},     // 0x048
{"GO/GSUB "},     // 0x049
{"C=0 WPT"},     // 0x04A
{"GONC +9"},     // 0x04B
{"ST=1? 4"},     // 0x04C
{"GO/GSUB "},     // 0x04D
{"C=0 W"},     // 0x04E
{"GOC +9"},     // 0x04F
{"LC 1"},     // 0x050
{"GO/GSUB "},     // 0x051
{"C=0 PQ"},     // 0x052
{"GONC +10"},     // 0x053
{"?PT= 4"},     // 0x054
{"GO/GSUB "},     // 0x055
{"C=0 XS"},     // 0x056
{"GOC +10"},     // 0x057
{"G=C "},     // 0x058
{"GO/GSUB "},     // 0x059
{"C=0 M"},     // 0x05A
{"GONC +11"},     // 0x05B
{"?PT= 4"},     // 0x05C
{"GO/GSUB "},     // 0x05D
{"C=0 S"},     // 0x05E
{"GOC +11"},     // 0x05F
{"POWOFF "},     // 0x060
{"GO/GSUB "},     // 0x061
{"ABEX PT"},     // 0x062
{"GONC +12"},     // 0x063
{"SELPF 1"},     // 0x064
{"GO/GSUB "},     // 0x065
{"ABEX X"},     // 0x066
{"GOC +12"},     // 0x067
{"REGN=C 1(Z)"},     // 0x068
{"GO/GSUB "},     // 0x069
{"ABEX WPT"},     // 0x06A
{"GONC +13"},     // 0x06B
{"?PFLG4=1 "},     // 0x06C
{"GO/GSUB "},     // 0x06D
{"ABEX W"},     // 0x06E
{"GOC +13"},     // 0x06F
{"N=C "},     // 0x070
{"GO/GSUB "},     // 0x071
{"ABEX PQ"},     // 0x072
{"GONC +14"},     // 0x073
{"UNUSED "},     // 0x074
{"GO/GSUB "},     // 0x075
{"ABEX XS"},     // 0x076
{"GOC +14"},     // 0x077
{"C=REGN 1(Z)"},     // 0x078
{"GO/GSUB "},     // 0x079
{"ABEX M"},     // 0x07A
{"GONC +15"},     // 0x07B
{"RCR 4"},     // 0x07C
{"GO/GSUB "},     // 0x07D
{"ABEX S"},     // 0x07E
{"GOC +15"},     // 0x07F
{"UNUSED "},     // 0x080
{"GO/GSUB "},     // 0x081
{"B=A PT"},     // 0x082
{"GONC +16"},     // 0x083
{"ST=0 5"},     // 0x084
{"GO/GSUB "},     // 0x085
{"B=A X"},     // 0x086
{"GOC +16"},     // 0x087
{"ST=1 5"},     // 0x088
{"GO/GSUB "},     // 0x089
{"B=A WPT"},     // 0x08A
{"GONC +17"},     // 0x08B
{"ST=1? 5"},     // 0x08C
{"GO/GSUB "},     // 0x08D
{"B=A W"},     // 0x08E
{"GOC +17"},     // 0x08F
{"LC 2"},     // 0x090
{"GO/GSUB "},     // 0x091
{"B=A PQ"},     // 0x092
{"GONC +18"},     // 0x093
{"?PT= 5"},     // 0x094
{"GO/GSUB "},     // 0x095
{"B=A XS"},     // 0x096
{"GOC +18"},     // 0x097
{"C=G "},     // 0x098
{"GO/GSUB "},     // 0x099
{"B=A M"},     // 0x09A
{"GONC +19"},     // 0x09B
{"?PT= 5"},     // 0x09C
{"GO/GSUB "},     // 0x09D
{"B=A S"},     // 0x09E
{"GOC +19"},     // 0x09F
{"SELP "},     // 0x0A0
{"GO/GSUB "},     // 0x0A1
{"ACEX PT"},     // 0x0A2
{"GONC +20"},     // 0x0A3
{"SELPF 2"},     // 0x0A4
{"GO/GSUB "},     // 0x0A5
{"ACEX X"},     // 0x0A6
{"GOC +20"},     // 0x0A7
{"REGN=C 2(Y)"},     // 0x0A8
{"GO/GSUB "},     // 0x0A9
{"ACEX WPT"},     // 0x0AA
{"GONC +21"},     // 0x0AB
{"?PFLG5=1 ?EDAV"},     // 0x0AC
{"GO/GSUB "},     // 0x0AD
{"ACEX W"},     // 0x0AE
{"GOC +21"},     // 0x0AF
{"C=N "},     // 0x0B0
{"GO/GSUB "},     // 0x0B1
{"ACEX PQ"},     // 0x0B2
{"GONC +22"},     // 0x0B3
{"UNUSED "},     // 0x0B4
{"GO/GSUB "},     // 0x0B5
{"ACEX XS"},     // 0x0B6
{"GOC +22"},     // 0x0B7
{"C=REGN 2(Y)"},     // 0x0B8
{"GO/GSUB "},     // 0x0B9
{"ACEX M"},     // 0x0BA
{"GONC +23"},     // 0x0BB
{"RCR 5"},     // 0x0BC
{"GO/GSUB "},     // 0x0BD
{"ACEX S"},     // 0x0BE
{"GOC +23"},     // 0x0BF
{"EADD=C (MAXX)"},     // 0x0C0
{"GO/GSUB "},     // 0x0C1
{"C=B PT"},     // 0x0C2
{"GONC +24"},     // 0x0C3
{"ST=0 10"},     // 0x0C4
{"GO/GSUB "},     // 0x0C5
{"C=B X"},     // 0x0C6
{"GOC +24"},     // 0x0C7
{"ST=1 10"},     // 0x0C8
{"GO/GSUB "},     // 0x0C9
{"C=B WPT"},     // 0x0CA
{"GONC +25"},     // 0x0CB
{"ST=1? 10"},     // 0x0CC
{"GO/GSUB "},     // 0x0CD
{"C=B W"},     // 0x0CE
{"GOC +25"},     // 0x0CF
{"LC 3"},     // 0x0D0
{"GO/GSUB "},     // 0x0D1
{"C=B PQ"},     // 0x0D2
{"GONC +26"},     // 0x0D3
{"?PT= 10"},     // 0x0D4
{"GO/GSUB "},     // 0x0D5
{"C=B XS"},     // 0x0D6
{"GOC +26"},     // 0x0D7
{"CGEX "},     // 0x0D8
{"GO/GSUB "},     // 0x0D9
{"C=B M"},     // 0x0DA
{"GONC +27"},     // 0x0DB
{"?PT= 10"},     // 0x0DC
{"GO/GSUB "},     // 0x0DD
{"C=B S"},     // 0x0DE
{"GOC +27"},     // 0x0DF
{"SELQ "},     // 0x0E0
{"GO/GSUB "},     // 0x0E1
{"BCEX PT"},     // 0x0E2
{"GONC +28"},     // 0x0E3
{"SELPF 3"},     // 0x0E4
{"GO/GSUB "},     // 0x0E5
{"BCEX X"},     // 0x0E6
{"GOC +28"},     // 0x0E7
{"REGN=C 3(X)"},     // 0x0E8
{"GO/GSUB "},     // 0x0E9
{"BCEX WPT"},     // 0x0EA
{"GONC +29"},     // 0x0EB
{"?PFLG10=1 ?ORAV"},     // 0x0EC
{"GO/GSUB "},     // 0x0ED
{"BCEX W"},     // 0x0EE
{"GOC +29"},     // 0x0EF
{"CNEX "},     // 0x0F0
{"GO/GSUB "},     // 0x0F1
{"BCEX PQ"},     // 0x0F2
{"GONC +30"},     // 0x0F3
{"UNUSED "},     // 0x0F4
{"GO/GSUB "},     // 0x0F5
{"BCEX XS"},     // 0x0F6
{"GOC +30"},     // 0x0F7
{"C=REGN 3(X)"},     // 0x0F8
{"GO/GSUB "},     // 0x0F9
{"BCEX M"},     // 0x0FA
{"GONC +31"},     // 0x0FB
{"RCR 10"},     // 0x0FC
{"GO/GSUB "},     // 0x0FD
{"BCEX S"},     // 0x0FE
{"GOC +31"},     // 0x0FF
{"ENROM1 "},     // 0x100
{"GO/GSUB "},     // 0x101
{"A=C PT"},     // 0x102
{"GONC +32"},     // 0x103
{"ST=0 8"},     // 0x104
{"GO/GSUB "},     // 0x105
{"A=C X"},     // 0x106
{"GOC +32"},     // 0x107
{"ST=1 8"},     // 0x108
{"GO/GSUB "},     // 0x109
{"A=C WPT"},     // 0x10A
{"GONC +33"},     // 0x10B
{"ST=1? 8"},     // 0x10C
{"GO/GSUB "},     // 0x10D
{"A=C W"},     // 0x10E
{"GOC +33"},     // 0x10F
{"LC 4"},     // 0x110
{"GO/GSUB "},     // 0x111
{"A=C PQ"},     // 0x112
{"GONC +34"},     // 0x113
{"?PT= 8"},     // 0x114
{"GO/GSUB "},     // 0x115
{"A=C XS"},     // 0x116
{"GOC +34"},     // 0x117
{"UNUSED "},     // 0x118
{"GO/GSUB "},     // 0x119
{"A=C M"},     // 0x11A
{"GONC +35"},     // 0x11B
{"?PT= 8"},     // 0x11C
{"GO/GSUB "},     // 0x11D
{"A=C S"},     // 0x11E
{"GOC +35"},     // 0x11F
{"?P=Q "},     // 0x120
{"GO/GSUB "},     // 0x121
{"A=A+B PT"},     // 0x122
{"GONC +36"},     // 0x123
{"SELPF 4"},     // 0x124
{"GO/GSUB "},     // 0x125
{"A=A+B X"},     // 0x126
{"GOC +36"},     // 0x127
{"REGN=C 4(L)"},     // 0x128
{"GO/GSUB "},     // 0x129
{"A=A+B WPT"},     // 0x12A
{"GONC +37"},     // 0x12B
{"?PFLG8=1 ?FRAV"},     // 0x12C
{"GO/GSUB "},     // 0x12D
{"A=A+B W"},     // 0x12E
{"GOC +37"},     // 0x12F
{"LDI "},     // 0x130
{"GO/GSUB "},     // 0x131
{"A=A+B PQ"},     // 0x132
{"GONC +38"},     // 0x133
{"UNUSED "},     // 0x134
{"GO/GSUB "},     // 0x135
{"A=A+B XS"},     // 0x136
{"GOC +38"},     // 0x137
{"C=REGN 4(L)"},     // 0x138
{"GO/GSUB "},     // 0x139
{"A=A+B M"},     // 0x13A
{"GONC +39"},     // 0x13B
{"RCR 8"},     // 0x13C
{"GO/GSUB "},     // 0x13D
{"A=A+B S"},     // 0x13E
{"GOC +39"},     // 0x13F
{"ENROM2 "},     // 0x140
{"GO/GSUB "},     // 0x141
{"A=A+C PT"},     // 0x142
{"GONC +40"},     // 0x143
{"ST=0 6"},     // 0x144
{"GO/GSUB "},     // 0x145
{"A=A+C X"},     // 0x146
{"GOC +40"},     // 0x147
{"ST=1 6"},     // 0x148
{"GO/GSUB "},     // 0x149
{"A=A+C WPT"},     // 0x14A
{"GONC +41"},     // 0x14B
{"ST=1? 6"},     // 0x14C
{"GO/GSUB "},     // 0x14D
{"A=A+C W"},     // 0x14E
{"GOC +41"},     // 0x14F
{"LC 5"},     // 0x150
{"GO/GSUB "},     // 0x151
{"A=A+C PQ"},     // 0x152
{"GONC +42"},     // 0x153
{"?PT= 6"},     // 0x154
{"GO/GSUB "},     // 0x155
{"A=A+C XS"},     // 0x156
{"GOC +42"},     // 0x157
{"M=C "},     // 0x158
{"GO/GSUB "},     // 0x159
{"A=A+C M"},     // 0x15A
{"GONC +43"},     // 0x15B
{"?PT= 6"},     // 0x15C
{"GO/GSUB "},     // 0x15D
{"A=A+C S"},     // 0x15E
{"GOC +43"},     // 0x15F
{"?LLD "},     // 0x160
{"GO/GSUB "},     // 0x161
{"A=A+1 PT"},     // 0x162
{"GONC +44"},     // 0x163
{"SELPF 5"},     // 0x164
{"GO/GSUB "},     // 0x165
{"A=A+1 X"},     // 0x166
{"GOC +44"},     // 0x167
{"REGN=C 5(M)"},     // 0x168
{"GO/GSUB "},     // 0x169
{"A=A+1 WPT"},     // 0x16A
{"GONC +45"},     // 0x16B
{"?PFLG6=1 ?IFCR"},     // 0x16C
{"GO/GSUB "},     // 0x16D
{"A=A+1 W"},     // 0x16E
{"GOC +45"},     // 0x16F
{"STK=C "},     // 0x170
{"GO/GSUB "},     // 0x171
{"A=A+1 PQ"},     // 0x172
{"GONC +46"},     // 0x173
{"UNUSED "},     // 0x174
{"GO/GSUB "},     // 0x175
{"A=A+1 XS"},     // 0x176
{"GOC +46"},     // 0x177
{"C=REGN 5(M)"},     // 0x178
{"GO/GSUB "},     // 0x179
{"A=A+1 M"},     // 0x17A
{"GONC +47"},     // 0x17B
{"RCR 6"},     // 0x17C
{"GO/GSUB "},     // 0x17D
{"A=A+1 S"},     // 0x17E
{"GOC +47"},     // 0x17F
{"ENROM3 "},     // 0x180
{"GO/GSUB "},     // 0x181
{"A=A-B PT"},     // 0x182
{"GONC +48"},     // 0x183
{"ST=0 11"},     // 0x184
{"GO/GSUB "},     // 0x185
{"A=A-B X"},     // 0x186
{"GOC +48"},     // 0x187
{"ST=1 11"},     // 0x188
{"GO/GSUB "},     // 0x189
{"A=A-B WPT"},     // 0x18A
{"GONC +49"},     // 0x18B
{"ST=1? 11"},     // 0x18C
{"GO/GSUB "},     // 0x18D
{"A=A-B W"},     // 0x18E
{"GOC +49"},     // 0x18F
{"LC 6"},     // 0x190
{"GO/GSUB "},     // 0x191
{"A=A-B PQ"},     // 0x192
{"GONC +50"},     // 0x193
{"?PT= 11"},     // 0x194
{"GO/GSUB "},     // 0x195
{"A=A-B XS"},     // 0x196
{"GOC +50"},     // 0x197
{"C=M "},     // 0x198
{"GO/GSUB "},     // 0x199
{"A=A-B M"},     // 0x19A
{"GONC +51"},     // 0x19B
{"?PT= 11"},     // 0x19C
{"GO/GSUB "},     // 0x19D
{"A=A-B S"},     // 0x19E
{"GOC +51"},     // 0x19F
{"CLRABC "},     // 0x1A0
{"GO/GSUB "},     // 0x1A1
{"A=A-1 PT"},     // 0x1A2
{"GONC +52"},     // 0x1A3
{"SELPF 6"},     // 0x1A4
{"GO/GSUB "},     // 0x1A5
{"A=A-1 X"},     // 0x1A6
{"GOC +52"},     // 0x1A7
{"REGN=C 6(N)"},     // 0x1A8
{"GO/GSUB "},     // 0x1A9
{"A=A-1 WPT"},     // 0x1AA
{"GONC +53"},     // 0x1AB
{"?PFLG11=1 ?TFAIL"},     // 0x1AC
{"GO/GSUB "},     // 0x1AD
{"A=A-1 W"},     // 0x1AE
{"GOC +53"},     // 0x1AF
{"C=STK "},     // 0x1B0
{"GO/GSUB "},     // 0x1B1
{"A=A-1 PQ"},     // 0x1B2
{"GONC +54"},     // 0x1B3
{"UNUSED "},     // 0x1B4
{"GO/GSUB "},     // 0x1B5
{"A=A-1 XS"},     // 0x1B6
{"GOC +54"},     // 0x1B7
{"C=REGN 6(N)"},     // 0x1B8
{"GO/GSUB "},     // 0x1B9
{"A=A-1 M"},     // 0x1BA
{"GONC +55"},     // 0x1BB
{"RCR 11"},     // 0x1BC
{"GO/GSUB "},     // 0x1BD
{"A=A-1 S"},     // 0x1BE
{"GOC +55"},     // 0x1BF
{"ENROM4 "},     // 0x1C0
{"GO/GSUB "},     // 0x1C1
{"A=A-C PT"},     // 0x1C2
{"GONC +56"},     // 0x1C3
{"UNUSED "},     // 0x1C4
{"GO/GSUB "},     // 0x1C5
{"A=A-C X"},     // 0x1C6
{"GOC +56"},     // 0x1C7
{"UNUSED "},     // 0x1C8
{"GO/GSUB "},     // 0x1C9
{"A=A-C WPT"},     // 0x1CA
{"GONC +57"},     // 0x1CB
{"UNUSED "},     // 0x1CC
{"GO/GSUB "},     // 0x1CD
{"A=A-C W"},     // 0x1CE
{"GOC +57"},     // 0x1CF
{"LC 7"},     // 0x1D0
{"GO/GSUB "},     // 0x1D1
{"A=A-C PQ"},     // 0x1D2
{"GONC +58"},     // 0x1D3
{"UNUSED "},     // 0x1D4
{"GO/GSUB "},     // 0x1D5
{"A=A-C XS"},     // 0x1D6
{"GOC +58"},     // 0x1D7
{"CMEX "},     // 0x1D8
{"GO/GSUB "},     // 0x1D9
{"A=A-C M"},     // 0x1DA
{"GONC +59"},     // 0x1DB
{"UNUSED "},     // 0x1DC
{"GO/GSUB "},     // 0x1DD
{"A=A-C S"},     // 0x1DE
{"GOC +59"},     // 0x1DF
{"GOTOC "},     // 0x1E0
{"GO/GSUB "},     // 0x1E1
{"C=C+C PT"},     // 0x1E2
{"GONC +60"},     // 0x1E3
{"SELPF 7"},     // 0x1E4
{"GO/GSUB "},     // 0x1E5
{"C=C+C X"},     // 0x1E6
{"GOC +60"},     // 0x1E7
{"REGN=C 7(O)"},     // 0x1E8
{"GO/GSUB "},     // 0x1E9
{"C=C+C WPT"},     // 0x1EA
{"GONC +61"},     // 0x1EB
{"UNUSED "},     // 0x1EC
{"GO/GSUB "},     // 0x1ED
{"C=C+C W"},     // 0x1EE
{"GOC +61"},     // 0x1EF
{"WPTOG (HEPAX)"},     // 0x1F0
{"GO/GSUB "},     // 0x1F1
{"C=C+C PQ"},     // 0x1F2
{"GONC +62"},     // 0x1F3
{"UNUSED "},     // 0x1F4
{"GO/GSUB "},     // 0x1F5
{"C=C+C XS"},     // 0x1F6
{"GOC +62"},     // 0x1F7
{"C=REGN 7(O)"},     // 0x1F8
{"GO/GSUB "},     // 0x1F9
{"C=C+C M"},     // 0x1FA
{"GONC +63"},     // 0x1FB
{"WCMD (41CL)"},     // 0x1FC
{"GO/GSUB "},     // 0x1FD
{"C=C+C S"},     // 0x1FE
{"GOC +63"},     // 0x1FF
{"HPIL=C 0"},     // 0x200
{"GO/GSUB "},     // 0x201
{"C=C+A PT"},     // 0x202
{"GONC -64"},     // 0x203
{"ST=0 2"},     // 0x204
{"GO/GSUB "},     // 0x205
{"C=C+A X"},     // 0x206
{"GOC -64"},     // 0x207
{"ST=1 2"},     // 0x208
{"GO/GSUB "},     // 0x209
{"C=C+A WPT"},     // 0x20A
{"GONC -63"},     // 0x20B
{"ST=1? 2"},     // 0x20C
{"GO/GSUB "},     // 0x20D
{"C=C+A W"},     // 0x20E
{"GOC -63"},     // 0x20F
{"LC 8"},     // 0x210
{"GO/GSUB "},     // 0x211
{"C=C+A PQ"},     // 0x212
{"GONC -62"},     // 0x213
{"?PT= 2"},     // 0x214
{"GO/GSUB "},     // 0x215
{"C=C+A XS"},     // 0x216
{"GOC -62"},     // 0x217
{"UNUSED "},     // 0x218
{"GO/GSUB "},     // 0x219
{"C=C+A M"},     // 0x21A
{"GONC -61"},     // 0x21B
{"?PT= 2"},     // 0x21C
{"GO/GSUB "},     // 0x21D
{"C=C+A S"},     // 0x21E
{"GOC -61"},     // 0x21F
{"C=KEYS "},     // 0x220
{"GO/GSUB "},     // 0x221
{"C=C+1 PT"},     // 0x222
{"GONC -60"},     // 0x223
{"SELPF 8"},     // 0x224
{"GO/GSUB "},     // 0x225
{"C=C+1 X"},     // 0x226
{"GOC -60"},     // 0x227
{"REGN=C 8(P)"},     // 0x228
{"GO/GSUB "},     // 0x229
{"C=C+1 WPT"},     // 0x22A
{"GONC -59"},     // 0x22B
{"?PFLG2=1 ?WNDB"},     // 0x22C
{"GO/GSUB "},     // 0x22D
{"C=C+1 W"},     // 0x22E
{"GOC -59"},     // 0x22F
{"GOKEYS "},     // 0x230
{"GO/GSUB "},     // 0x231
{"C=C+1 PQ"},     // 0x232
{"GONC -58"},     // 0x233
{"UNUSED "},     // 0x234
{"GO/GSUB "},     // 0x235
{"C=C+1 XS"},     // 0x236
{"GOC -58"},     // 0x237
{"C=REGN 8(P)"},     // 0x238
{"GO/GSUB "},     // 0x239
{"C=C+1 M"},     // 0x23A
{"GONC -57"},     // 0x23B
{"RCR 2"},     // 0x23C
{"GO/GSUB "},     // 0x23D
{"C=C+1 S"},     // 0x23E
{"GOC -57"},     // 0x23F
{"HPIL=C 1"},     // 0x240
{"GO/GSUB "},     // 0x241
{"C=A-C PT"},     // 0x242
{"GONC -56"},     // 0x243
{"ST=0 9"},     // 0x244
{"GO/GSUB "},     // 0x245
{"C=A-C X"},     // 0x246
{"GOC -56"},     // 0x247
{"ST=1 9"},     // 0x248
{"GO/GSUB "},     // 0x249
{"C=A-C WPT"},     // 0x24A
{"GONC -55"},     // 0x24B
{"ST=1? 9"},     // 0x24C
{"GO/GSUB "},     // 0x24D
{"C=A-C W"},     // 0x24E
{"GOC -55"},     // 0x24F
{"LC 9"},     // 0x250
{"GO/GSUB "},     // 0x251
{"C=A-C PQ"},     // 0x252
{"GONC -54"},     // 0x253
{"?PT= 9"},     // 0x254
{"GO/GSUB "},     // 0x255
{"C=A-C XS"},     // 0x256
{"GOC -54"},     // 0x257
{"F=SB "},     // 0x258
{"GO/GSUB "},     // 0x259
{"C=A-C M"},     // 0x25A
{"GONC -53"},     // 0x25B
{"?PT= 9"},     // 0x25C
{"GO/GSUB "},     // 0x25D
{"C=A-C S"},     // 0x25E
{"GOC -53"},     // 0x25F
{"SETHEX "},     // 0x260
{"GO/GSUB "},     // 0x261
{"C=C-1 PT"},     // 0x262
{"GONC -52"},     // 0x263
{"SELPF 9"},     // 0x264
{"GO/GSUB "},     // 0x265
{"C=C-1 X"},     // 0x266
{"GOC -52"},     // 0x267
{"REGN=C 9(Q)"},     // 0x268
{"GO/GSUB "},     // 0x269
{"C=C-1 WPT"},     // 0x26A
{"GONC -51"},     // 0x26B
{"?PFLG9=1 ?FRNS"},     // 0x26C
{"GO/GSUB "},     // 0x26D
{"C=C-1 W"},     // 0x26E
{"GOC -51"},     // 0x26F
{"DADD=C "},     // 0x270
{"GO/GSUB "},     // 0x271
{"C=C-1 PQ"},     // 0x272
{"GONC -50"},     // 0x273
{"UNUSED "},     // 0x274
{"GO/GSUB "},     // 0x275
{"C=C-1 XS"},     // 0x276
{"GOC -50"},     // 0x277
{"C=REGN 9(Q)"},     // 0x278
{"GO/GSUB "},     // 0x279
{"C=C-1 M"},     // 0x27A
{"GONC -49"},     // 0x27B
{"RCR 9"},     // 0x27C
{"GO/GSUB "},     // 0x27D
{"C=C-1 S"},     // 0x27E
{"GOC -49"},     // 0x27F
{"HPIL=C 2"},     // 0x280
{"GO/GSUB "},     // 0x281
{"C=0-C PT"},     // 0x282
{"GONC -48"},     // 0x283
{"ST=0 7"},     // 0x284
{"GO/GSUB "},     // 0x285
{"C=0-C X"},     // 0x286
{"GOC -48"},     // 0x287
{"ST=1 7"},     // 0x288
{"GO/GSUB "},     // 0x289
{"C=0-C WPT"},     // 0x28A
{"GONC -47"},     // 0x28B
{"ST=1? 7"},     // 0x28C
{"GO/GSUB "},     // 0x28D
{"C=0-C W"},     // 0x28E
{"GOC -47"},     // 0x28F
{"LC A"},     // 0x290
{"GO/GSUB "},     // 0x291
{"C=0-C PQ"},     // 0x292
{"GONC -46"},     // 0x293
{"?PT= 7"},     // 0x294
{"GO/GSUB "},     // 0x295
{"C=0-C XS"},     // 0x296
{"GOC -46"},     // 0x297
{"SB=F "},     // 0x298
{"GO/GSUB "},     // 0x299
{"C=0-C M"},     // 0x29A
{"GONC -45"},     // 0x29B
{"?PT= 7"},     // 0x29C
{"GO/GSUB "},     // 0x29D
{"C=0-C S"},     // 0x29E
{"GOC -45"},     // 0x29F
{"SETDEC "},     // 0x2A0
{"GO/GSUB "},     // 0x2A1
{"C=-C-1 PT"},     // 0x2A2
{"GONC -44"},     // 0x2A3
{"SELPF A"},     // 0x2A4
{"GO/GSUB "},     // 0x2A5
{"C=-C-1 X"},     // 0x2A6
{"GOC -44"},     // 0x2A7
{"REGN=C 10(+)"},     // 0x2A8
{"GO/GSUB "},     // 0x2A9
{"C=-C-1 WPT"},     // 0x2AA
{"GONC -43"},     // 0x2AB
{"?PFLG7=1 ?SRQR"},     // 0x2AC
{"GO/GSUB "},     // 0x2AD
{"C=-C-1 W"},     // 0x2AE
{"GOC -43"},     // 0x2AF
{"UNUSED "},     // 0x2B0
{"GO/GSUB "},     // 0x2B1
{"C=-C-1 PQ"},     // 0x2B2
{"GONC -42"},     // 0x2B3
{"UNUSED "},     // 0x2B4
{"GO/GSUB "},     // 0x2B5
{"C=-C-1 XS"},     // 0x2B6
{"GOC -42"},     // 0x2B7
{"C=REGN 10(+)"},     // 0x2B8
{"GO/GSUB "},     // 0x2B9
{"C=-C-1 M"},     // 0x2BA
{"GONC -41"},     // 0x2BB
{"RCR 7"},     // 0x2BC
{"GO/GSUB "},     // 0x2BD
{"C=-C-1 S"},     // 0x2BE
{"GOC -41"},     // 0x2BF
{"HPIL=C 3"},     // 0x2C0
{"GO/GSUB "},     // 0x2C1
{"?B#0 PT"},     // 0x2C2
{"GONC -40"},     // 0x2C3
{"ST=0 13"},     // 0x2C4
{"GO/GSUB "},     // 0x2C5
{"?B#0 X"},     // 0x2C6
{"GOC -40"},     // 0x2C7
{"ST=1 13"},     // 0x2C8
{"GO/GSUB "},     // 0x2C9
{"?B#0 WPT"},     // 0x2CA
{"GONC -39"},     // 0x2CB
{"ST=1? 13"},     // 0x2CC
{"GO/GSUB "},     // 0x2CD
{"?B#0 W"},     // 0x2CE
{"GOC -39"},     // 0x2CF
{"LC B"},     // 0x2D0
{"GO/GSUB "},     // 0x2D1
{"?B#0 PQ"},     // 0x2D2
{"GONC -38"},     // 0x2D3
{"?PT= 13"},     // 0x2D4
{"GO/GSUB "},     // 0x2D5
{"?B#0 XS"},     // 0x2D6
{"GOC -38"},     // 0x2D7
{"FEXSB "},     // 0x2D8
{"GO/GSUB "},     // 0x2D9
{"?B#0 M"},     // 0x2DA
{"GONC -37"},     // 0x2DB
{"?PT= 13"},     // 0x2DC
{"GO/GSUB "},     // 0x2DD
{"?B#0 S"},     // 0x2DE
{"GOC -37"},     // 0x2DF
{"DISOFF "},     // 0x2E0
{"GO/GSUB "},     // 0x2E1
{"?C#0 PT"},     // 0x2E2
{"GONC -36"},     // 0x2E3
{"SELPF B"},     // 0x2E4
{"GO/GSUB "},     // 0x2E5
{"?C#0 X"},     // 0x2E6
{"GOC -36"},     // 0x2E7
{"REGN=C 11(a)"},     // 0x2E8
{"GO/GSUB "},     // 0x2E9
{"?C#0 WPT"},     // 0x2EA
{"GONC -35"},     // 0x2EB
{"?PFLG13=1 ?SERV"},     // 0x2EC
{"GO/GSUB "},     // 0x2ED
{"?C#0 W"},     // 0x2EE
{"GOC -35"},     // 0x2EF
{"DATA=C "},     // 0x2F0
{"GO/GSUB "},     // 0x2F1
{"?C#0 PQ"},     // 0x2F2
{"GONC -34"},     // 0x2F3
{"UNUSED "},     // 0x2F4
{"GO/GSUB "},     // 0x2F5
{"?C#0 XS"},     // 0x2F6
{"GOC -34"},     // 0x2F7
{"C=REGN 11(a)"},     // 0x2F8
{"GO/GSUB "},     // 0x2F9
{"?C#0 M"},     // 0x2FA
{"GONC -33"},     // 0x2FB
{"RCR 13"},     // 0x2FC
{"GO/GSUB "},     // 0x2FD
{"?C#0 S"},     // 0x2FE
{"GOC -33"},     // 0x2FF
{"HPIL=C 4"},     // 0x300
{"GO/GSUB "},     // 0x301
{"?A<C PT"},     // 0x302
{"GONC -32"},     // 0x303
{"ST=0 1"},     // 0x304
{"GO/GSUB "},     // 0x305
{"?A<C X"},     // 0x306
{"GOC -32"},     // 0x307
{"ST=1 1"},     // 0x308
{"GO/GSUB "},     // 0x309
{"?A<C WPT"},     // 0x30A
{"GONC -31"},     // 0x30B
{"ST=1? 1"},     // 0x30C
{"GO/GSUB "},     // 0x30D
{"?A<C W"},     // 0x30E
{"GOC -31"},     // 0x30F
{"LC C"},     // 0x310
{"GO/GSUB "},     // 0x311
{"?A<C PQ"},     // 0x312
{"GONC -30"},     // 0x313
{"?PT= 1"},     // 0x314
{"GO/GSUB "},     // 0x315
{"?A<C XS"},     // 0x316
{"GOC -30"},     // 0x317
{"UNUSED "},     // 0x318
{"GO/GSUB "},     // 0x319
{"?A<C M"},     // 0x31A
{"GONC -29"},     // 0x31B
{"?PT= 1"},     // 0x31C
{"GO/GSUB "},     // 0x31D
{"?A<C S"},     // 0x31E
{"GOC -29"},     // 0x31F
{"DISTOG "},     // 0x320
{"GO/GSUB "},     // 0x321
{"?A<B PT"},     // 0x322
{"GONC -28"},     // 0x323
{"SELPF C"},     // 0x324
{"GO/GSUB "},     // 0x325
{"?A<B X"},     // 0x326
{"GOC -28"},     // 0x327
{"REGN=C 12(b)"},     // 0x328
{"GO/GSUB "},     // 0x329
{"?A<B WPT"},     // 0x32A
{"GONC -27"},     // 0x32B
{"?PFLG1=1 ?CRDR"},     // 0x32C
{"GO/GSUB "},     // 0x32D
{"?A<B W"},     // 0x32E
{"GOC -27"},     // 0x32F
{"CXISA "},     // 0x330
{"GO/GSUB "},     // 0x331
{"?A<B PQ"},     // 0x332
{"GONC -26"},     // 0x333
{"UNUSED "},     // 0x334
{"GO/GSUB "},     // 0x335
{"?A<B XS"},     // 0x336
{"GOC -26"},     // 0x337
{"C=REGN 12(b)"},     // 0x338
{"GO/GSUB "},     // 0x339
{"?A<B M"},     // 0x33A
{"GONC -25"},     // 0x33B
{"RCR 1"},     // 0x33C
{"GO/GSUB "},     // 0x33D
{"?A<B S"},     // 0x33E
{"GOC -25"},     // 0x33F
{"HPIL=C 5"},     // 0x340
{"GO/GSUB "},     // 0x341
{"?A#0 PT"},     // 0x342
{"GONC -24"},     // 0x343
{"ST=0 12"},     // 0x344
{"GO/GSUB "},     // 0x345
{"?A#0 X"},     // 0x346
{"GOC -24"},     // 0x347
{"ST=1 12"},     // 0x348
{"GO/GSUB "},     // 0x349
{"?A#0 WPT"},     // 0x34A
{"GONC -23"},     // 0x34B
{"ST=1? 12"},     // 0x34C
{"GO/GSUB "},     // 0x34D
{"?A#0 W"},     // 0x34E
{"GOC -23"},     // 0x34F
{"LC D"},     // 0x350
{"GO/GSUB "},     // 0x351
{"?A#0 PQ"},     // 0x352
{"GONC -22"},     // 0x353
{"?PT= 12"},     // 0x354
{"GO/GSUB "},     // 0x355
{"?A#0 XS"},     // 0x356
{"GOC -22"},     // 0x357
{"ST=C "},     // 0x358
{"GO/GSUB "},     // 0x359
{"?A#0 M"},     // 0x35A
{"GONC -21"},     // 0x35B
{"?PT= 12"},     // 0x35C
{"GO/GSUB "},     // 0x35D
{"?A#0 S"},     // 0x35E
{"GOC -21"},     // 0x35F
{" RTNC"},     // 0x360
{"GO/GSUB "},     // 0x361
{"?A#C PT"},     // 0x362
{"GONC -20"},     // 0x363
{"SELPF D"},     // 0x364
{"GO/GSUB "},     // 0x365
{"?A#C X"},     // 0x366
{"GOC -20"},     // 0x367
{"REGN=C 13(c)"},     // 0x368
{"GO/GSUB "},     // 0x369
{"?A#C WPT"},     // 0x36A
{"GONC -19"},     // 0x36B
{"?PFLG12=1 ?ALM"},     // 0x36C
{"GO/GSUB "},     // 0x36D
{"?A#C W"},     // 0x36E
{"GOC -19"},     // 0x36F
{"C=CORA "},     // 0x370
{"GO/GSUB "},     // 0x371
{"?A#C PQ"},     // 0x372
{"GONC -18"},     // 0x373
{"UNUSED "},     // 0x374
{"GO/GSUB "},     // 0x375
{"?A#C XS"},     // 0x376
{"GOC -18"},     // 0x377
{"C=REGN 13(c)"},     // 0x378
{"GO/GSUB "},     // 0x379
{"?A#C M"},     // 0x37A
{"GONC -17"},     // 0x37B
{"RCR 12"},     // 0x37C
{"GO/GSUB "},     // 0x37D
{"?A#C S"},     // 0x37E
{"GOC -17"},     // 0x37F
{"HPIL=C 6"},     // 0x380
{"GO/GSUB "},     // 0x381
{"ASR PT"},     // 0x382
{"GONC -16"},     // 0x383
{"ST=0 0"},     // 0x384
{"GO/GSUB "},     // 0x385
{"ASR X"},     // 0x386
{"GOC -16"},     // 0x387
{"ST=1 0"},     // 0x388
{"GO/GSUB "},     // 0x389
{"ASR WPT"},     // 0x38A
{"GONC -15"},     // 0x38B
{"ST=1? 0"},     // 0x38C
{"GO/GSUB "},     // 0x38D
{"ASR W"},     // 0x38E
{"GOC -15"},     // 0x38F
{"LC E"},     // 0x390
{"GO/GSUB "},     // 0x391
{"ASR PQ"},     // 0x392
{"GONC -14"},     // 0x393
{"?PT= 0"},     // 0x394
{"GO/GSUB "},     // 0x395
{"ASR XS"},     // 0x396
{"GOC -14"},     // 0x397
{"C=ST "},     // 0x398
{"GO/GSUB "},     // 0x399
{"ASR M"},     // 0x39A
{"GONC -13"},     // 0x39B
{"?PT= 0"},     // 0x39C
{"GO/GSUB "},     // 0x39D
{"ASR S"},     // 0x39E
{"GOC -13"},     // 0x39F
{"RTNNC "},     // 0x3A0
{"GO/GSUB "},     // 0x3A1
{"BSR PT"},     // 0x3A2
{"GONC -12"},     // 0x3A3
{"SELPF E"},     // 0x3A4
{"GO/GSUB "},     // 0x3A5
{"BSR X"},     // 0x3A6
{"GOC -12"},     // 0x3A7
{"REGN=C 14(d)"},     // 0x3A8
{"GO/GSUB "},     // 0x3A9
{"BSR WPT"},     // 0x3AA
{"GONC -11"},     // 0x3AB
{"?PFLG0=1 ?PBSY"},     // 0x3AC
{"GO/GSUB "},     // 0x3AD
{"BSR W"},     // 0x3AE
{"GOC -11"},     // 0x3AF
{"C=CANDA "},     // 0x3B0
{"GO/GSUB "},     // 0x3B1
{"BSR PQ"},     // 0x3B2
{"GONC -10"},     // 0x3B3
{"UNUSED "},     // 0x3B4
{"GO/GSUB "},     // 0x3B5
{"BSR XS"},     // 0x3B6
{"GOC -10"},     // 0x3B7
{"C=REGN 14(d)"},     // 0x3B8
{"GO/GSUB "},     // 0x3B9
{"BSR M"},     // 0x3BA
{"GONC -9"},     // 0x3BB
{"RCR 0"},     // 0x3BC
{"GO/GSUB "},     // 0x3BD
{"BSR S"},     // 0x3BE
{"GOC -9"},     // 0x3BF
{"HPIL=C 7"},     // 0x3C0
{"GO/GSUB "},     // 0x3C1
{"CSR PT"},     // 0x3C2
{"GONC -8"},     // 0x3C3
{"CLRST "},     // 0x3C4
{"GO/GSUB "},     // 0x3C5
{"CSR X"},     // 0x3C6
{"GOC -8"},     // 0x3C7
{"RSTKB "},     // 0x3C8
{"GO/GSUB "},     // 0x3C9
{"CSR WPT"},     // 0x3CA
{"GONC -7"},     // 0x3CB
{"CHKKB "},     // 0x3CC
{"GO/GSUB "},     // 0x3CD
{"CSR W"},     // 0x3CE
{"GOC -7"},     // 0x3CF
{"LC F"},     // 0x3D0
{"GO/GSUB "},     // 0x3D1
{"CSR PQ"},     // 0x3D2
{"GONC -6"},     // 0x3D3
{"DECPT "},     // 0x3D4
{"GO/GSUB "},     // 0x3D5
{"CSR XS"},     // 0x3D6
{"GOC -6"},     // 0x3D7
{"CSTEX "},     // 0x3D8
{"GO/GSUB "},     // 0x3D9
{"CSR M"},     // 0x3DA
{"GONC -5"},     // 0x3DB
{"INCPT "},     // 0x3DC
{"GO/GSUB "},     // 0x3DD
{"CSR S"},     // 0x3DE
{"GOC -5"},     // 0x3DF
{"RTN "},     // 0x3E0
{"GO/GSUB "},     // 0x3E1
{"ASL PT"},     // 0x3E2
{"GONC -4"},     // 0x3E3
{"SELPF F"},     // 0x3E4
{"GO/GSUB "},     // 0x3E5
{"ASL X"},     // 0x3E6
{"GOC -4"},     // 0x3E7
{"REGN=C 15(e)"},     // 0x3E8
{"GO/GSUB "},     // 0x3E9
{"ASL WPT"},     // 0x3EA
{"GONC -3"},     // 0x3EB
{"UNUSED "},     // 0x3EC
{"GO/GSUB "},     // 0x3ED
{"ASL W"},     // 0x3EE
{"GOC -3"},     // 0x3EF
{"PFAD=C "},     // 0x3F0
{"GO/GSUB "},     // 0x3F1
{"ASL PQ"},     // 0x3F2
{"GONC -2"},     // 0x3F3
{"UNUSED "},     // 0x3F4
{"GO/GSUB "},     // 0x3F5
{"ASL XS"},     // 0x3F6
{"GOC -2"},     // 0x3F7
{"C=REGN 15(e)"},     // 0x3F8
{"GO/GSUB "},     // 0x3F9
{"ASL M"},     // 0x3FA
{"GONC -1"},     // 0x3FB
{"UNUSED "},     // 0x3FC
{"GO/GSUB "},     // 0x3FD
{"ASL S"},     // 0x3FE
{"GOC -1"},     // 0x3FF


};


struct TLine TraceSample;                   // Trace Buffer definition, default (maximum info)

struct TLine_FI TraceSample_FI;             // Trace Buffer definition, no HP-IL , with FI
struct TLine_basic Tracesample_basic;       // basic Tracebuffer
struct PLine PowerSample;                   // Power event buffer definition

uint32_t ISAsample;
uint64_t DATAsample;
uint32_t DATAsample1;
uint32_t DATAsample2;
uint32_t xq1 = 0;
uint32_t xq2 = 0;

static volatile uint32_t fi1 = 0;
static volatile uint32_t fi2 = 0;
static volatile uint64_t fi = 0;
static volatile uint16_t fi_compressed = 0;
 
uint32_t addr;
uint32_t instr, sinstr;
uint32_t delayed_dis;
uint32_t sync;
uint32_t cycle;
uint32_t cycle_prev;
uint8_t  bank;

bool drive_isa   = false;   // driven by TULIP
bool drive_carry = false;   // carry driven by TULIP
bool drive_data  = false;   // data driven by TULIP
bool drive_FI    = false;   // FI driven by TULIP
bool xeq_tulip   = false;   // instruction hit by TULIP
bool dat_driven  = false;   // data driven by TULIP


uint16_t type_GOXQ;

char overflow = '.';
char prev_overflow = ' ';
bool sample_skipped = false;

uint32_t data0;
uint32_t data1;

uint32_t data_x;        // DATA exponent, 2 digits
uint32_t data_xs;       // DATA exponent sign, 1 digit
uint32_t data_m1;       // DATA mantissa, 5 digits, D12..D31
uint32_t data_m2;       // DATA mantissa, 5 digits, D28..D51
uint32_t data_s;        // DATA sign, D52..D55

int16_t activeSELP = -1;    // for disassembling peripheral instructions

bool Tracer_firstconnect = false;   // to detect the first connect to a CDC host
                                    // to display a welcome message

uint16_t ILframe_out = 0;
uint16_t ILframe_in = 0;
uint8_t HPIL_REG_copy[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
bool ILchanged = false;
uint16_t wframe;
char ILmnem[10];


queue_t TraceBuffer;
queue_t PreTrigBuffer;          // buffer for pre-trigger samples
queue_t PowerBuffer;

int64_t us_elapsed;
float ms_elapsed;
float secs_elapsed;
int calc_speed;

queue_t PowerEventBuffer;           // buffer for power events

char TracePrint[250];
int TracePrintLen = 0;
int dis_len;

volatile int level;
volatile int prev_level;

int ALD_range_lo;
int ALD_range_hi;
bool ALD_block = false;
int test = 0;
bool blocking = false;

bool prev_block = false;
bool block = false;
    
// extern queue_t TraceBuffer;
extern struct TLine TraceLine;             // the variable with the TraceLine

extern int sample_break;

bool something_in_the_queue = false;       

Filter TraceFilter;        // the trace filter settings

const char *const mode_str[] = {
    "RUNNING    ",
    "LIGHT SLEEP",
    "DEEP SLEEP " };

// initialize the trace buffer
void TraceBuffer_init()
{
    int TraceLength = globsetting.get(tracer_mainbuffer);           // get the trace length from the global settings
    queue_init(&TraceBuffer, sizeof(TraceLine), TraceLength);       // define trace buffer, default 5000 samples

    int PreTrigLength = globsetting.get(tracer_pretrig);            // get the pre-trigger length from the global settings
    queue_init(&PreTrigBuffer, sizeof(TraceLine), PreTrigLength);   // define pre-trigger buffer, default 32 samples

    queue_init(&PowerBuffer, sizeof(PLine), 32 );              // buffer for power mode events
}

bool SetPreTrigBuffer(int pretrig)
{
    // change the pre-trigger buffer size
    if (pretrig < 1) return false;          // negative pre-trigger size is not allowed
    if (pretrig > 256) return false;       // maximum pre-trigger size is 1000 samples

    queue_free(&PreTrigBuffer);                                 // free the old pre-trigger buffer
    queue_init(&PreTrigBuffer, sizeof(TraceLine), pretrig);     // define pre-trigger buffer
    return true;
}


void HPIL_instr(uint16_t instr)
// disassemble HP-IL specific instruction for SELP = 0..7
// activeSELP indicates the selected register
{
    uint16_t i_type = instr & 0x003;         // isolate last two bits

    switch (i_type)
    {
    case 1:         // copy literal to selected HP-IL register
        TracePrintLen += sprintf(TracePrint + TracePrintLen, "  reg %d=%02X", activeSELP, (instr & 0x3FC) >> 2); 
        break;
    case 2:         // copies HP-IL register n to C[0..1]
        TracePrintLen += sprintf(TracePrint + TracePrintLen, "  C[0.1]=reg %d", (instr & 0x1C0) >> 6); 
        break;
    case 3:         // return control to CPU - ?PFSET
        TracePrintLen += sprintf(TracePrint + TracePrintLen, "  ?PFSET"); 
        break;
    default:
        TracePrintLen += sprintf(TracePrint + TracePrintLen, "  oops, unknown?"); 
        // should never get here
        break;
    }
}


// this is the main tracer function, called constantly from the main() loop in core0


void Trace_task()
{

    // check if a CDC port is connected, and if it was the first connection
    // to display a welcome message to identify the port

    if (cdc_connected(ITF_TRACE)) {
        // only if the Tracer CDC interface is connected
        if (!Tracer_firstconnect) {
            // Tracer_firstconnect was false, so this is now a new CDC connection
            Tracer_firstconnect = true;
            TracePrintLen = 0;
            cli_printf("  CDC Port 2 [tracer] connected");
            trace_enabled = true;
            TracePrintLen += sprintf(TracePrint + TracePrintLen, "TRACER CDC PORT connected, trace is %s\n\r", trace_enabled ? "enabled":"disabled");
            cdc_sendbuf(ITF_TRACE, TracePrint, TracePrintLen);
            cdc_flush(ITF_TRACE);
        } else {
            if (cdc_read_char(ITF_TRACE) != 0) {
                TracePrintLen = 0;
                trace_enabled = !trace_enabled;
                globsetting.set(tracer_enabled, !globsetting.get(tracer_enabled));
                TracePrintLen += sprintf(TracePrint + TracePrintLen, "Trace is %s\n\r", trace_enabled ? "enabled":"disabled");
                cdc_sendbuf(ITF_TRACE, TracePrint, TracePrintLen);
                cdc_flush(ITF_TRACE);
            }
        }
    }

    // check for disconnection of the tracer CDC
    if ((Tracer_firstconnect) && (!cdc_connected(ITF_TRACE))) {
        // CDC interface is disconnected
        cli_printf("  CDC Port 2 [tracer] disconnected");
        trace_enabled = false;          // disable the tracer
        Tracer_firstconnect = false;
    }

    // TODO: to be checked: if the tracer CDC port is not connected we can simply empty the buffer
    // and not do anything
    // also if the tracer is disabled we can skip the rest of the function
    // but we must keep emptying the buffer
    /*
    if ((!cdc_connected(ITF_TRACE)) || (!trace_enabled)) {
        if (!queue_is_empty(&TraceBuffer)) {
            // only do something if there is something in the tracebuffer
            tud_task();             // must keep the USB port updated
            queue_remove_blocking(&TraceBuffer, &TraceSample); // read from the Trace Buffer
        } 
        return;
    } 
        */


        // powermode reporting is disabled for now
/*
        us_elapsed = absolute_time_diff_us(t_start, t_end);

        ms_elapsed = (us_elapsed % (1000 * 1000)) / 1000 ;
        secs_elapsed = us_elapsed / (1000 * 1000);
        TPrintLen = 0;
        TPrintLen += sprintf(TPrint + TPrintLen,"** HP41 Powermode: %s - previous mode %5d.%03d secs %s", 
            mode_str[HP41_powermode], secs_elapsed, ms_elapsed, mode_str[prev_mode]);

        if (prev_mode == PowerMode_Running) {
            TPrintLen += sprintf(TPrint + TPrintLen," %9d bus cycles", cycle_counter);
        }
        else {
            TPrintLen += sprintf(TPrint + TPrintLen, "");
        }
        puts(TPrint);
*/
/*
    if (!queue_is_empty(&PowerBuffer) && queue_is_empty(&TraceBuffer)) {
        // only do something if there is something in the power event buffer
        // and the trace buffer is empty,
        tud_task();             // must keep the USB port updated
        queue_remove_blocking(&PowerBuffer, &PowerSample); // read from the Power Event Buffer

        TracePrintLen = 0;

        us_elapsed = absolute_time_diff_us(PowerSample.t_start, PowerSample.t_end); 
        secs_elapsed = (float)us_elapsed / (1000 * 1000);

        TracePrintLen += sprintf(TracePrint + TracePrintLen,"\n** Powermode: %s - from %s -  %7.3f secs", 
            mode_str[PowerSample.new_mode], mode_str[PowerSample.prev_mode], secs_elapsed);

        calc_speed = PowerSample.cycle_number / secs_elapsed;

        if (PowerSample.prev_mode == PowerMode_Running) {
            TracePrintLen += sprintf(TracePrint + TracePrintLen," - %9d bus cycles - %d cycles/sec\n\r", PowerSample.cycle_number, calc_speed);
        }
        else {
            TracePrintLen += sprintf(TracePrint + TracePrintLen, "\n\r");
        }

        cdc_sendbuf(ITF_TRACE, TracePrint, TracePrintLen);
        cdc_flush(ITF_TRACE);
    }

    */

    if (!queue_is_empty(&TraceBuffer)) {
        // only do something if there is something in the tracebuffer
        tud_task();             // must keep the USB port updated

        // read from the Trace Buffer
        // queue_remove_blocking(&TraceBuffer, &TraceSample);

        something_in_the_queue = queue_try_remove(&TraceBuffer, &TraceSample);   // also try to remove from the pre-trigger buffer, to keep it updated

        if (!globsetting.get(tracer_enabled)) return;        // tracer is disabled
        if (!trace_enabled) return;

        // we could get out here if there is no CDC port connected for the tracer
        if (!cdc_connected(ITF_TRACE)) {

            // cdc_sendbuf(ITF_TRACE, TracePrint, TracePrintLen);
            // cdc_flush(ITF_TRACE);  // why flush if it is not connected?
            tud_task();             // must keep the USB port updated
            return;
        }

        if (&TraceSample == NULL) return;

        // now start analyzing the trace sample

        sinstr = TraceSample.isa_instruction;       // sync is still in here
        instr = sinstr & 0x03FF;
        sync  = (TraceSample.isa_instruction >> 11) & 0x01;     // sync is bit 11 of the instruction

        addr  = TraceSample.isa_address;
        cycle_prev = cycle;
        cycle = TraceSample.cycle_number;
        
        DATAsample1 = TraceSample.data1;        // D31..D00
        DATAsample2 = TraceSample.data2;        // D55..D32, right justified 0xfeffffff

        // bank = TraceSample.bank;
        // bank is now in the instruction, bits 12 and 13, to be able to trace bank changes
        bank = (TraceSample.isa_instruction >> 12) & 0x03;

        // get this status bit from bit 14 of the instruction
        drive_isa =(TraceSample.isa_instruction & 0x4000) != 0;   // driven by TULIP

        // get the carry status from bit 15 of the instruction, to be able to trace carry changes
        drive_carry = (TraceSample.isa_instruction & 0x8000) != 0;   // carry driven by TULIP

        xeq_tulip = (DATAsample2 & 0x40000000) != 0;   // instruction hit by TULIP if xq_instr is not 0
        dat_driven = (DATAsample2 & 0x80000000) != 0;   // data driven by TULIP if bit 31 of data2 is 1


        data_x  =  DATAsample1 & 0x000000FF;                // DATA exponent, 2 digits
        data_xs = (DATAsample1 & 0x00000F00) >>  8;         // DATA exponent sign, 1 digit
        data_m1 = (DATAsample1 & 0xFFFFF000) >> 12;         // DATA mantissa, 5 digits, D12..D31
        data_m2 =  DATAsample2 & 0x000FFFFF;                // DATA mantissa, 5 digits, D28..D51
        data_s  = (DATAsample2 & 0x00F00000) >> 20;         // DATA sign, D52..D55

        // a traceline starting with O marks a buffer overflow
        if (cycle != (cycle_prev + 1)) {
            overflow = 'O' ;
        } else {
            overflow = ' ';
        }

        // first check for all ranges to be filtered out

        // SYSTEM ROM, pages 0..5
        bool sysrom_trace = (addr >= 0x0000) && (addr < 0x6000);

        // IL ROMs, pages 6, 7
        bool ilrom_trace = (addr >= 0x6000) && (addr < 0x8000);

        // block some known system loops
        //   0x0098 - 0x00A1       RSTKB and RST05
        //   0x0177 - 0x0178       delay for debounce
        //   0x089C - 0x089D       BLINK01
        //   0x0E9A - 0x0E9E       NLT10 wait for key to NULL
        //   0x0EC9 - 0x0ECE       NULTST NULL timer
        bool sysloop_trace = ((addr >= 0x0098) && (addr <= 0x00A1)) || 
                             ((addr >= 0x0177) && (addr <= 0x0178)) || 
                             ((addr >= 0x089C) && (addr <= 0x089D)) ||
                             ((addr >= 0x0E9A) && (addr <= 0x0E9E)) ||
                             ((addr >= 0x0EC9) && (addr <= 0x0ECE));

        // now check if sample should be blocked 
        block = !globsetting.get(tracer_sysloop_on) && sysloop_trace;
        block |= block || !globsetting.get(tracer_sysrom_on) && sysrom_trace;
        block |= block || !globsetting.get(tracer_ilroms_on) && ilrom_trace;

        // if block is true then we have to block this sample, also set marker for previous block
        if (block) prev_block = true;

        if (!block) {
            // sample not to be blocked, now build the trace string ;

            if (prev_block) {
                // there was a previous block, indicate skipped lines
                prev_block = false;
                overflow = '=' ;
            } 

            // build the trace/disassembly string
            TracePrintLen = 0;

            // add a + to the address if drive_isa is true, to indicate that the instruction was driven by TULIP
            // add a + to the instruction if it was a tulip hit, to indicate that the instruction was decoded
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"%c  %6d  %04X-%1d%c  %01X  %03X%c  ", overflow, cycle, addr, bank, 
                                                                                                       drive_isa ? '+' : ' ', sync, 
                                                                                                       instr, xeq_tulip ? '+' : ' ');

            TracePrintLen += sprintf(TracePrint + TracePrintLen,"%01X.%05X%05X.%01X.%02X", data_s, data_m2, data_m1, data_xs, data_x);

            // add a + tp the data if driven by the TULIP
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"%c  ", dat_driven ? '+' : ' ', DATAsample2);  // data driven by TULIP if bit 31 of data2 is 1

            /* code below can be skipped, now indicated by +
            if (TraceSample.xq_instr == 0) {
               // print .... if no instruction was executed by TULIP
               TracePrintLen += sprintf(TracePrint + TracePrintLen,"...  ", TraceSample.xq_instr);  // instruction decoded by TULIP
            } else {
                // decoded instruction, print it
                TracePrintLen += sprintf(TracePrint + TracePrintLen,"%03X  ", TraceSample.xq_instr & 0x3FF);  // instruction decoded by TULIP
            }

            */

            if (TraceSample.ramslct & 0x8000) {
                // print ... if our RAM access
                TracePrintLen += sprintf(TracePrint + TracePrintLen,"R%03X+  ", TraceSample.ramslct & 0x0FFF);  // RAM select, bit 15 is 1 if our RAM access
            } else {
                TracePrintLen += sprintf(TracePrint + TracePrintLen,"R%03X   ", TraceSample.ramslct & 0x0FFF);  // RAM select
            }
            

            // show the selected peripherl in the same way
            if (TraceSample.prphslct & 0x8000) {
                // print ... if our peripheral access, not supported yet but will come
                TracePrintLen += sprintf(TracePrint + TracePrintLen,"P%03X+  ", TraceSample.prphslct & 0x0FFF);  // Peripheral select, bit 15 is 1 if our peripheral access
            } else {
                TracePrintLen += sprintf(TracePrint + TracePrintLen,"P%03X   ", TraceSample.prphslct & 0x0FFF);  // Peripheral select
            }

            // if there was a carry only print a C, otherwise just a space
            // use drive_carry to indicate that the carry was driven by TULIP, to be able to track carry changes
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"%c  ", drive_carry ? 'C' : ' ');      

            // work out the FI bits
            fi_compressed = TraceSample.fi;
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"FI"); 

            // process the compress FI bits
            for (int i = 0; i < 14; i++) {      // all 14 digits
                if (((fi_compressed >> i) & 0b1) == 0b1 ) {
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"%01X", i); 
                } else {
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"-"); 
                }
            }

            drive_FI = (TraceSample.fi & 0x8000) != 0;   // FI driven by TULIP if bit 14 of fi is 1

            // add a + to the FI if driven by the TULIP
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"%c  ", drive_FI ? '+' : ' ');


            // printf(" %08x", TraceSample.xq_data1) in case emulator internals are needed

            if (globsetting.get(tracer_dis_type) != 0) {
                // disassembly is enabled, so disassemble the instruction

                // keep track of the length of the disassembly line for alignment of any text after it
                dis_len = TracePrintLen + 20;       // allow 20 chars for the disassembly text

                // disassembly of the traceline
                if (sync == 1) {
                    // valid instruction
                    if ((instr & 0x003) == 0x001) {     // class 1 instruction
                        // Class 1 is 2-word GO/XQ, so handle in the next disassembly line
                        delayed_dis = instr;
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"  ..."); 
                    } else {
                        if (globsetting.get(tracer_dis_type) == tracer_distype_JDA) {
                            // full disassembly, so use the mnemonics
                            TracePrintLen += sprintf(TracePrint + TracePrintLen,"  %s", mnemonics[instr]); 
                        } else {
                            // then it is HP
                            TracePrintLen += sprintf(TracePrint + TracePrintLen,"  %s", mnemonics[instr + 0x400]); 
                        }
                        delayed_dis = 0;
                        // isolate SELPn instruction, this is a CLASS 0 instruction
                        // bit pattern PPPPIIII00,. where I = 1001 (0x9) and P is the peripheral selected
                        // mask is 0b0000111111 (0x03F), test is 0b0000100100 (0x024)
                        if ((instr & 0x03F) == 0x024) 
                        {
                            // SELPF Found
                            activeSELP = (instr & 0x3C0) >> 6;
                        }
                        else activeSELP = -1;           // reset after any instruction with a SYNC
                    }
                } else {
                    // no SYNC, so the 2nd word of a multi-byte instruction
                    // or under peripheral control, or FETCH S&X
                    if ((delayed_dis & 0x003) == 0x001){
                        // class 1 instruction XQ/GO
                        type_GOXQ = instr & 0x003;
                        char goxq_str[10];

                        if (globsetting.get(tracer_dis_type) == tracer_distype_JDA) {
                            // full disassembly, so use the mnemonics
                            switch (type_GOXQ) {
                                case 0x000: sprintf(goxq_str, "?NC XQ"); break;
                                case 0x001: sprintf(goxq_str, "?C XQ "); break;
                                case 0x002: sprintf(goxq_str, "?NC GO"); break;
                                case 0x003: sprintf(goxq_str, "?C GO "); break;
                                default: break;
                            }
                        } else {
                            // then it is HP                            
                            switch (type_GOXQ) {
                                case 0x000: sprintf(goxq_str, "GSUBNC"); break;
                                case 0x001: sprintf(goxq_str, "GSUBC "); break;
                                case 0x002: sprintf(goxq_str, "GOLNC "); break;
                                case 0x003: sprintf(goxq_str, "GOLC  "); break;
                                default: break;
                            }
                        }
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"  %s", goxq_str); 
                        TracePrintLen += sprintf(TracePrint + TracePrintLen," %02X%02X", (instr & 0x3FC) >>2, (delayed_dis & 0x3FC) >> 2);
                        delayed_dis = 0;
                    } else {
                        // not a class 1 instruction so treat as literal
                        // could be from LDI, under peripheral control or FETCH S&X
                        // implement in disassembler (maybe) later
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"  %03X", instr);
                        if ((activeSELP >= 0) && (activeSELP <= 7)) {
                            // there is an active peripheral, decode the literal
                            // for now for HP-IL
                            HPIL_instr(instr);
                        }
                    }
                }

                // file the traceline with spaces until the disassembly text is 20 chars long
                // for alligment of the IL frame and registers after this
                //  1BA  C[0.1]=reg 6 is 17 chars   
                while (TracePrintLen < dis_len) {
                    TracePrintLen += sprintf(TracePrint + TracePrintLen," ");
                }

            } else {
                // disassembly is not enabled, so just add some spaces for alignment of any text after it
                TracePrintLen += sprintf(TracePrint + TracePrintLen,"   "); 
            }

            // add tracing for the HP-IL frames and registers if enabled and the HP-IL module is plugged            
            if (globsetting.get(tracer_ilregs_on) && globsetting.get(HP82160A_enabled)) {

                // list the HP-IL registers and the frames
                // only list when a frame is sent or received
                wframe = TraceSample.frame_out;

                // find out the frame if any and print if there was an incoming or outgoing frame
                if ( wframe != 0xFFFF) {
                    // 0xFFFF is the value in the tracebuffer is nothing was sent
                    getIL_mnemonic(wframe, ILmnem);
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  IL> %03X %s", wframe, ILmnem); 
                } else {
                    wframe = TraceSample.frame_in;
                    if (ILframe_in != wframe) {
                        // packet received
                        getIL_mnemonic(wframe, ILmnem);
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"  IL< %03X %s", wframe, ILmnem); 
                        ILframe_in = wframe;
                    } else {
                        // no packet received of sent
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"             "); 
                    }
                }
                              
                // now print the HP-IL registers only if there was a change in one of the registers
                ILchanged = false;
                for (int i = 0; i < 9; i++) {
                    if (HPIL_REG_copy[i] != TraceSample.HPILregs[i]) {
                        ILchanged = true;
                    }
                }

                if (ILchanged) {
                     // only print the registers if there was a change
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  Reg "); 
                    for (int i = 0; i < 9; i++) {   
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"%02X", TraceSample.HPILregs[i]);

                        // a changed register is indicated with a *
                        if (HPIL_REG_copy[i] != TraceSample.HPILregs[i]) {
                            TracePrintLen += sprintf(TracePrint + TracePrintLen,"* ");
                        } else {
                            TracePrintLen += sprintf(TracePrint + TracePrintLen,"  ");
                        }
                        HPIL_REG_copy[i] = TraceSample.HPILregs[i];
                    }
                }
            }

            // end of the traceline, finish it
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"\n\r");

            if (cdc_connected(ITF_TRACE)) {
                // only send if something is connected, otherwise this will stall
                // better move this test to earlier in the tracer task for performance
                cdc_sendbuf(ITF_TRACE, TracePrint, TracePrintLen);
                // cdc_flush(ITF_TRACE);
                // tud_task();             // must keep the USB port updated
            }
             
        } else{
            // sample skipped
            sample_skipped = true;
        }
    }
}
