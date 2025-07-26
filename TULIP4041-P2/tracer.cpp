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
// list of HP41 mnemonics, JDA style
// forced in FLASH to maximize SRAM
{
"NOP",                  // 000
"GO/XQ",        
"A=0 @R",       
"JNC +0",       
"CLRF 3",       
"GO/XQ",        
"A=0 S&X",      
"JC 0",     
"SETF 3",       
"GO/XQ",        
"A=0 R<-",      
"JNC +1",       
"?FSET 3",      
"GO/XQ",        
"A=0 ALL",      
"JC 1",         
"LD@R 0",               // 010
"GO/XQ",        
"A=0 P-Q",      
"JNC +2",       
"?R= 3",        
"GO/XQ ",       
"A=0 XS",       
"JC 2",
"UNUSED ",      
"GO/XQ ",       
"A=0 M",        
"JNC +3",       
"R= 3",         
"GO/XQ ",       
"A=0 MS",       
"JC 3",
"XQ>GO ",       
"GO/XQ ",       
"B=0 @R",       
"JNC +4",       
"SELP 0",       
"GO/XQ ",       
"B=0 S&X",      
"JC 4",
"WRIT 0(T)",    
"GO/XQ ",       
"B=0 R<-",      
"JNC +5",       
"?FI 3",        
"GO/XQ ",       
"B=0 ALL",      
"JC 5",
"ROMBLK ",      
"GO/XQ ",       
"B=0 P-Q",      
"JNC +6",       
"UNUSED ",      
"GO/XQ ",       
"B=0 XS",       
"JC 6",
"READ 0(T)",   
"GO/XQ ",       
"B=0 M",        
"JNC +7",       
"RCR 3",        
"GO/XQ ",       
"B=0 MS",       
"JC 7",
"WROM ",        // 040   
"GO/XQ ",       
"C=0 @R",       
"JNC +8",       
"CLRF 4",       
"GO/XQ ",       
"C=0 S&X",      
"JC 8",
"SETF 4",       
"GO/XQ ",       
"C=0 R<-",      
"JNC +9",       
"?FSET 4",      
"GO/XQ ",       
"C=0 ALL",      
"JC 9",
"LD@R 1",       
"GO/XQ ",       
"C=0 P-Q",      
"JNC +10",      
"?R= 4",        
"GO/XQ ",       
"C=0 XS",       
"JC 10",
"G=C ",         
"GO/XQ ",       
"C=0 M",        
"JNC +11",      
"R= 4",         
"GO/XQ ",       
"C=0 MS",       
"JC 11",        
"POWOFF ",      
"GO/XQ ",       
"A<>B @R",      
"JNC +12",      
"SELP 1",       
"GO/XQ ",       
"A<>B S&X",     
"JC 12",
"WRIT 1(Z)",    
"GO/XQ ",       
"A<>B R<-",     
"JNC +13",      
"?FI 4",        
"GO/XQ ",       
"A<>B ALL",     
"JC 13",
"N=C ",         
"GO/XQ ",       
"A<>B P-Q",     
"JNC +14",      
"UNUSED ",      
"GO/XQ ",       
"A<>B XS",      
"JC 14",
"READ 1(Z)",    
"GO/XQ ",       
"A<>B M",       
"JNC +15",      
"RCR 4",        
"GO/XQ ",       
"A<>B MS",      
"JC 15",
"UNUSED ",      
"GO/XQ ",       
"B=A @R",       
"JNC +16",      
"CLRF 5",       
"GO/XQ ",       
"B=A S&X",      
"JC 16",
"SETF 5",       
"GO/XQ ",       
"B=A R<-",      
"JNC +17",      
"?FSET 5",      
"GO/XQ ",       
"B=A ALL",      
"JC 17",
"LD@R 2",       
"GO/XQ ",       
"B=A P-Q",      
"JNC +18",      
"?R= 5",        
"GO/XQ ",       
"B=A XS",       
"JC 18",
"C=G ",         
"GO/XQ ",       
"B=A M",        
"JNC +19",      
"R= 5",         
"GO/XQ ",       
"B=A MS",       
"JC 19",
"SLCTP ",       
"GO/XQ ",       
"A<>C @R",      
"JNC +20",      
"SELP 2",       
"GO/XQ ",       
"A<>C S&X",     
"JC 20",
"WRIT 2(Y)",    
"GO/XQ ",       
"A<>C R<-",     
"JNC +21",      
"?FI 5 ?EDAV",  
"GO/XQ ",       
"A<>C ALL",     
"JC 21",
"C=N ",         
"GO/XQ ",       
"A<>C P-Q",     
"JNC +22",      
"UNUSED ",      
"GO/XQ ",       
"A<>C XS",      
"JC 22",
"READ 2(Y)",    
"GO/XQ ",       
"A<>C M",       
"JNC +23",      
"RCR 5",        
"GO/XQ ",       
"A<>C MS",      
"JC 23",
"EADD=C MAXX",      // 0C0, MAXX Expanded Memory Select
"GO/XQ ",       
"C=B @R",       
"JNC +24",      
"CLRF 10",      
"GO/XQ ",       
"C=B S&X",      
"JC 24",
"SETF 10",      
"GO/XQ ",       
"C=B R<-",      
"JNC +25",      
"?FSET 10",     
"GO/XQ ",       
"C=B ALL",      
"JC 25",
"LD@R 3",       
"GO/XQ ",       
"C=B P-Q",      
"JNC +26",      
"?R= 10",       
"GO/XQ ",       
"C=B XS",       
"JC 26",
"C<>G ",        
"GO/XQ ",       
"C=B M",        
"JNC +27",      
"R= 10",        
"GO/XQ ",       
"C=B MS",       
"JC 27",
"SLCTQ ",       
"GO/XQ ",       
"B<>C @R",      
"JNC +28",      
"SELP 3",       
"GO/XQ ",       
"B<>C S&X",     
"JC 28",
"WRIT 3(X)",    
"GO/XQ ",       
"B<>C R<-",     
"JNC +29",      
"?FI 10 ?ORAV", 
"GO/XQ ",       
"B<>C ALL",     
"JC 29",
"C<>N ",        
"GO/XQ ",       
"B<>C P-Q",     
"JNC +30",      
"UNUSED ",      
"GO/XQ ",       
"B<>C XS",      
"JC 30",
"READ 3(X)",    
"GO/XQ ",       
"B<>C M",       
"JNC +31",      
"RCR 10",       
"GO/XQ ",       
"B<>C MS",      
"JC 31",
"ENBANK1 ",     
"GO/XQ ",       
"A=C @R",       
"JNC +32",      
"CLRF 8",       
"GO/XQ ",       
"A=C S&X",      
"JC 32",
"SETF 8",       
"GO/XQ ",       
"A=C R<-",      
"JNC +33",      
"?FSET 8",      
"GO/XQ ",       
"A=C ALL",      
"JC 33",
"LD@R 4",       
"GO/XQ ",       
"A=C P-Q",      
"JNC +34",      
"?R= 8",        
"GO/XQ ",       
"A=C XS",       
"JC 34",
"UNUSED ",      
"GO/XQ ",       
"A=C M",        
"JNC +35",      
"R= 8",         
"GO/XQ ",       
"A=C MS",       
"JC 35",
"?P=Q ",        
"GO/XQ ",       
"A=A+B @R",     
"JNC +36",      
"SELP 4",       
"GO/XQ ",       
"A=A+B S&X",    
"JC 36",
"WRIT 4(L)",    
"GO/XQ ",       
"A=A+B R<-",    
"JNC +37",      
"?FI 8 ?FRAV",  
"GO/XQ ",       
"A=A+B ALL",    
"JC 37",
"LDI ",         
"GO/XQ ",       
"A=A+B P-Q",    
"JNC +38",      
"UNUSED ",      
"GO/XQ ",       
"A=A+B XS",     
"JC 38",
"READ 4(L)",    
"GO/XQ ",       
"A=A+B M",      
"JNC +39",      
"RCR 8",        
"GO/XQ ",       
"A=A+B MS",     
"JC 39",
"ENBANK3 ",     
"GO/XQ ",       
"A=A+C @R",     
"JNC +40",      
"CLRF 6",       
"GO/XQ ",       
"A=A+C S&X",    
"JC 40",
"SETF 6",       
"GO/XQ ",       
"A=A+C R<-",    
"JNC +41",      
"?FSET 6",      
"GO/XQ ",       
"A=A+C ALL",    
"JC 41",
"LD@R 5",       
"GO/XQ ",       
"A=A+C P-Q",    
"JNC +42",      
"?R= 6",        
"GO/XQ ",       
"A=A+C XS",     
"JC 42",
"M=C ",         
"GO/XQ ",       
"A=A+C M",      
"JNC +43",      
"R= 6",         
"GO/XQ ",       
"A=A+C MS",     
"JC 43",
"?LOWBAT ",     
"GO/XQ ",       
"A=A+1 @R",     
"JNC +44",      
"SELP 5",       
"GO/XQ ",       
"A=A+1 S&X",    
"JC 44",
"WRIT 5(M)",    
"GO/XQ ",       
"A=A+1 R<-",    
"JNC +45",      
"?FI 6 ?IFCR",  
"GO/XQ ",       
"A=A+1 ALL",    
"JC 45",
"PUSHADR ",
"GO/XQ ",
"A=A+1 P-Q",
"JNC +46",
"UNUSED ",
"GO/XQ ",
"A=A+1 XS",
"JC 46",
"READ 5(M)",
"GO/XQ ",
"A=A+1 M",
"JNC +47",
"RCR 6",
"GO/XQ ",
"A=A+1 MS",
"JC 47",
"ENBANK2 ",
"GO/XQ ",
"A=A-B @R",
"JNC +48",
"CLRF 11",
"GO/XQ ",
"A=A-B S&X",
"JC 48",
"SETF 11",
"GO/XQ ",
"A=A-B R<-",
"JNC +49",
"?FSET 11",
"GO/XQ ",
"A=A-B ALL",
"JC 49",
"LD@R 6",
"GO/XQ ",
"A=A-B P-Q",
"JNC +50",
"?R= 11",
"GO/XQ ",
"A=A-B XS",
"JC 50",
"C=M ",
"GO/XQ ",
"A=A-B M",
"JNC +51",
"R= 11",
"GO/XQ ",
"A=A-B MS",
"JC 51",
"A=B=C=0 ",
"GO/XQ ",
"A=A-1 @R",
"JNC +52",
"SELP 6",
"GO/XQ ",
"A=A-1 S&X",
"JC 52",
"WRIT 6(N)",
"GO/XQ ",
"A=A-1 R<-",
"JNC +53",
"?FI 11 ?TFAIL",
"GO/XQ ",
"A=A-1 ALL",
"JC 53",
"POPADR ",
"GO/XQ ",
"A=A-1 P-Q",
"JNC +54",
"UNUSED ",
"GO/XQ ",
"A=A-1 XS",
"JC 54",
"READ 6(N)",
"GO/XQ ",
"A=A-1 M",
"JNC +55",
"RCR 11",
"GO/XQ ",
"A=A-1 MS",
"JC 55",
"ENBANK4 ",
"GO/XQ ",
"A=A-C @R",
"JNC +56",
"UNUSED ",
"GO/XQ ",
"A=A-C S&X",
"JC 56",
"UNUSED ",
"GO/XQ ",
"A=A-C R<-",
"JNC +57",
"UNUSED ",
"GO/XQ ",
"A=A-C ALL",
"JC 57",
"LD@R 7",
"GO/XQ ",
"A=A-C P-Q",
"JNC +58",
"UNUSED ",
"GO/XQ ",
"A=A-C XS",
"JC 58",
"C<>M ",
"GO/XQ ",
"A=A-C M",
"JNC +59",
"UNUSED ",
"GO/XQ ",
"A=A-C MS",
"JC 59",
"GOTOADR ",
"GO/XQ ",
"C=C+C @R",
"JNC +60",
"SELP 7",
"GO/XQ ",
"C=C+C S&X",
"JC 60",
"WRIT 7(O)",
"GO/XQ ",
"C=C+C R<-",
"JNC +61",
"UNUSED ",
"GO/XQ ",
"C=C+C ALL",
"JC 61",
"WPTOG ",
"GO/XQ ",
"C=C+C P-Q",
"JNC +62",
"UNUSED ",
"GO/XQ ",
"C=C+C XS",
"JC 62",
"READ 7(O)",
"GO/XQ ",
"C=C+C M",
"JNC +63",
"WCMD ",
"GO/XQ ",
"C=C+C MS",
"JC 63",
"HPIL=C 0",
"GO/XQ ",
"C=C+A @R",
"JNC -64",
"CLRF 2",
"GO/XQ ",
"C=C+A S&X",
"JC -64",
"SETF 2",
"GO/XQ ",
"C=C+A R<-",
"JNC -63",
"?FSET 2",
"GO/XQ ",
"C=C+A ALL",
"JC -63",
"LD@R 8",
"GO/XQ ",
"C=C+A P-Q",
"JNC -62",
"?R= 2",
"GO/XQ ",
"C=C+A XS",
"JC -62",
"UNUSED ",
"GO/XQ ",
"C=C+A M",
"JNC -61",
"R= 2",
"GO/XQ ",
"C=C+A MS",
"JC -61",
"C=KEY ",
"GO/XQ ",
"C=C+1 @R",
"JNC -60",
"SELP 8",
"GO/XQ ",
"C=C+1 S&X",
"JC -60",
"WRIT 8(P)",
"GO/XQ ",
"C=C+1 R<-",
"JNC -59",
"?FI 2 ?WNDB",
"GO/XQ ",
"C=C+1 ALL",
"JC -59",
"GTOKEY ",
"GO/XQ ",
"C=C+1 P-Q",
"JNC -58",
"UNUSED ",
"GO/XQ ",
"C=C+1 XS",
"JC -58",
"READ 8(P)",
"GO/XQ ",
"C=C+1 M",
"JNC -57",
"RCR 2",
"GO/XQ ",
"C=C+1 MS",
"JC -57",
"HPIL=C 1",
"GO/XQ ",
"C=A-C @R",
"JNC -56",
"CLRF 9",
"GO/XQ ",
"C=A-C S&X",
"JC -56",
"SETF 9",
"GO/XQ ",
"C=A-C R<-",
"JNC -55",
"?FSET 9",
"GO/XQ ",
"C=A-C ALL",
"JC -55",
"LD@R 9",
"GO/XQ ",
"C=A-C P-Q",
"JNC -54",
"?R= 9",
"GO/XQ ",
"C=A-C XS",
"JC -54",
"T=ST ",
"GO/XQ ",
"C=A-C M",
"JNC -53",
"R= 9",
"GO/XQ ",
"C=A-C MS",
"JC -53",
"SETHEX ",
"GO/XQ ",
"C=C-1 @R",
"JNC -52",
"SELP 9",
"GO/XQ ",
"C=C-1 S&X",
"JC -52",
"WRIT 9(Q)",
"GO/XQ ",
"C=C-1 R<-",
"JNC -51",
"?FI 9 ?FRNS",
"GO/XQ ",
"C=C-1 ALL",
"JC -51",
"RAMSLCT ",
"GO/XQ ",
"C=C-1 P-Q",
"JNC -50",
"UNUSED ",
"GO/XQ ",
"C=C-1 XS",
"JC -50",
"READ 9(Q)",
"GO/XQ ",
"C=C-1 M",
"JNC -49",
"RCR 9",
"GO/XQ ",
"C=C-1 MS",
"JC -49",
"HPIL=C 2",
"GO/XQ ",
"C=0-C @R",
"JNC -48",
"CLRF 7",
"GO/XQ ",
"C=0-C S&X",
"JC -48",
"SETF 7",
"GO/XQ ",
"C=0-C R<-",
"JNC -47",
"?FSET 7",
"GO/XQ ",
"C=0-C ALL",
"JC -47",
"LD@R A",
"GO/XQ ",
"C=0-C P-Q",
"JNC -46",
"?R= 7",
"GO/XQ ",
"C=0-C XS",
"JC -46",
"ST=T ",
"GO/XQ ",
"C=0-C M",
"JNC -45",
"R= 7",
"GO/XQ ",
"C=0-C MS",
"JC -45",
"SETDEC ",
"GO/XQ ",
"C=-C-1 @R",
"JNC -44",
"SELP A",
"GO/XQ ",
"C=-C-1 S&X",
"JC -44",
"WRIT 10(+)",
"GO/XQ ",
"C=-C-1 R<-",
"JNC -43",
"?FI 7 ?SRQR",
"GO/XQ ",
"C=-C-1 ALL",
"JC -43",
"UNUSED ",
"GO/XQ ",
"C=-C-1 P-Q",
"JNC -42",
"UNUSED ",
"GO/XQ ",
"C=-C-1 XS",
"JC -42",
"READ 10(+)",
"GO/XQ ",
"C=-C-1 M",
"JNC -41",
"RCR 7",
"GO/XQ ",
"C=-C-1 MS",
"JC -41",
"HPIL=C 3",
"GO/XQ ",
"?B#0 @R",
"JNC -40",
"CLRF 13",
"GO/XQ ",
"?B#0 S&X",
"JC -40",
"SETF 13",
"GO/XQ ",
"?B#0 R<-",
"JNC -39",
"?FSET 13",
"GO/XQ ",
"?B#0 ALL",
"JC -39",
"LD@R B",
"GO/XQ ",
"?B#0 P-Q",
"JNC -38",
"?R= 13",
"GO/XQ ",
"?B#0 XS",
"JC -38",
"ST<>T ",
"GO/XQ ",
"?B#0 M",
"JNC -37",
"R= 13",
"GO/XQ ",
"?B#0 MS",
"JC -37",
"DSPOFF ",
"GO/XQ ",
"?C#0 @R",
"JNC -36",
"SELP B",
"GO/XQ ",
"?C#0 S&X",
"JC -36",
"WRIT 11(a)",
"GO/XQ ",
"?C#0 R<-",
"JNC -35",
"?FI 13 ?SERV",
"GO/XQ ",
"?C#0 ALL",
"JC -35",
"WRITDAT ",
"GO/XQ ",
"?C#0 P-Q",
"JNC -34",
"UNUSED ",
"GO/XQ ",
"?C#0 XS",
"JC -34",
"READ 11(a)",
"GO/XQ ",
"?C#0 M",
"JNC -33",
"RCR 13",
"GO/XQ ",
"?C#0 MS",
"JC -33",
"HPIL=C 4",
"GO/XQ ",
"?A<C @R",
"JNC -32",
"CLRF 1",
"GO/XQ ",
"?A<C S&X",
"JC -32",
"SETF 1",
"GO/XQ ",
"?A<C R<-",
"JNC -31",
"?FSET 1",
"GO/XQ ",
"?A<C ALL",
"JC -31",
"LD@R C",
"GO/XQ ",
"?A<C P-Q",
"JNC -30",
"?R= 1",
"GO/XQ ",
"?A<C XS",
"JC -30",
"UNUSED ",
"GO/XQ ",
"?A<C M",
"JNC -29",
"R= 1",
"GO/XQ ",
"?A<C MS",
"JC -29",
"DSPTOG ",
"GO/XQ ",
"?A<B @R",
"JNC -28",
"SELP C",
"GO/XQ ",
"?A<B S&X",
"JC -28",
"WRIT 12(b)",
"GO/XQ ",
"?A<B R<-",
"JNC -27",
"?FI 1 ?CRDR",
"GO/XQ ",
"?A<B ALL",
"JC -27",
"FETCH S&X ",
"GO/XQ ",
"?A<B P-Q",
"JNC -26",
"UNUSED ",
"GO/XQ ",
"?A<B XS",
"JC -26",
"READ 12(b)",
"GO/XQ ",
"?A<B M",
"JNC -25",
"RCR 1",
"GO/XQ ",
"?A<B MS",
"JC -25",
"HPIL=C 5",
"GO/XQ ",
"?A#0 @R",
"JNC -24",
"CLRF 12",
"GO/XQ ",
"?A#0 S&X",
"JC -24",
"SETF 12",
"GO/XQ ",
"?A#0 R<-",
"JNC -23",
"?FSET 12",
"GO/XQ ",
"?A#0 ALL",
"JC -23",
"LD@R D",
"GO/XQ ",
"?A#0 P-Q",
"JNC -22",
"?R= 12",
"GO/XQ ",
"?A#0 XS",
"JC -22",
"ST=C ",
"GO/XQ ",
"?A#0 M",
"JNC -21",
"R= 12",
"GO/XQ ",
"?A#0 MS",
"JC -21",
"?C RTN ",
"GO/XQ ",
"?A#C @R",
"JNC -20",
"SELP D",
"GO/XQ ",
"?A#C S&X",
"JC -20",
"WRIT 13(c)",
"GO/XQ ",
"?A#C R<-",
"JNC -19",
"?FI 12 ?ALM",
"GO/XQ ",
"?A#C ALL",
"JC -19",
"C=C O RA ",
"GO/XQ ",
"?A#C P-Q",
"JNC -18",
"UNUSED ",
"GO/XQ ",
"?A#C XS",
"JC -18",
"READ 13(c)",
"GO/XQ ",
"?A#C M",
"JNC -17",
"RCR 12",
"GO/XQ ",
"?A#C MS",
"JC -17",
"HPIL=C 6",
"GO/XQ ",
"RSHFA @R",
"JNC -16",
"CLRF 0",
"GO/XQ ",
"RSHFA S&X",
"JC -16",
"SETF 0",
"GO/XQ ",
"RSHFA R<-",
"JNC -15",
"?FSET 0",
"GO/XQ ",
"RSHFA ALL",
"JC -15",
"LD@R E",
"GO/XQ ",
"RSHFA P-Q",
"JNC -14",
"?R= 0",
"GO/XQ ",
"RSHFA XS",
"JC -14",
"C=ST ",
"GO/XQ ",
"RSHFA M",
"JNC -13",
"R= 0",
"GO/XQ ",
"RSHFA MS",
"JC -13",
"?NC RTN ",
"GO/XQ ",
"RSHFB @R",
"JNC -12",
"SELP E",
"GO/XQ ",
"RSHFB S&X",
"JC -12",
"WRIT 14(d)",
"GO/XQ ",
"RSHFB R<-",
"JNC -11",
"?FI 0 ?PBSY",
"GO/XQ ",
"RSHFB ALL",
"JC -11",
"C=C AND A ",
"GO/XQ ",
"RSHFB P-Q",
"JNC -10",
"UNUSED ",
"GO/XQ ",
"RSHFB XS",
"JC -10",
"READ 14(d)",
"GO/XQ ",
"RSHFB M",
"JNC -9",
"RCR 0",
"GO/XQ ",
"RSHFB MS",
"JC -9",
"HPIL=C 7",
"GO/XQ ",
"RSHFC @R",
"JNC -8",
"ST=0 ",
"GO/XQ ",
"RSHFC S&X",
"JC -8",
"CLRKEY ",
"GO/XQ ",
"RSHFC R<-",
"JNC -7",
"?KEY ",
"GO/XQ ",
"RSHFC ALL",
"JC -7",
"LD@R F",
"GO/XQ ",
"RSHFC P-Q",
"JNC -6",
"R=R-1 ",
"GO/XQ ",
"RSHFC XS",
"JC -6",
"C<>ST ",
"GO/XQ ",
"RSHFC M",
"JNC -5",
"R=R+1 ",
"GO/XQ ",
"RSHFC MS",
"JC -5",
"RTN ",
"GO/XQ ",
"LSHFA @R",
"JNC -4",
"SELP F",
"GO/XQ ",
"LSHFA S&X",
"JC -4",
"WRIT 15(e)",
"GO/XQ ",
"LSHFA R<-",
"JNC -3",
"?FI ",
"GO/XQ ",
"LSHFA ALL",
"JC -3",
"PRPHSLCT ",
"GO/XQ ",
"LSHFA P-Q",
"JNC -2",
"UNUSED ",
"GO/XQ ",
"LSHFA XS",
"JC -2",
"READ 15(e)",
"GO/XQ ",
"LSHFA M",
"JNC -1",
"UNUSED ",
"GO/XQ ",
"LSHFA MS",
"JC -1",                    // 3FF
};



struct TLine TraceSample;                   // Trace Buffer definition, default (maximum info)

struct TLine_FI TraceSample_FI;             // Trace Buffer definition, no HP-IL , with FI
struct TLine_basic Tracesample_basic;       // basic Tracebuffer

uint32_t ISAsample;
uint64_t DATAsample;
uint32_t DATAsample1;
uint32_t DATAsample2;
uint32_t xq1 = 0;
uint32_t xq2 = 0;

static volatile uint32_t fi1 = 0;
static volatile uint32_t fi2 = 0;
static volatile uint64_t fi = 0;
 
uint32_t addr;
uint32_t instr, sinstr;
uint32_t delayed_dis;
uint32_t sync;
uint32_t cycle;
uint32_t cycle_prev;
uint8_t  bank;

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

queue_t PowerEventBuffer;           // buffer for power events

char  TracePrint[250];
int   TracePrintLen = 0;

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


// initialize the trace buffer
void TraceBuffer_init()
{
    int TraceLength = globsetting.get(tracer_mainbuffer);           // get the trace length from the global settings
    queue_init(&TraceBuffer, sizeof(TraceLine), TraceLength);       // define trace buffer, default 5000 samples

    int PreTrigLength = globsetting.get(tracer_pretrig);            // get the pre-trigger length from the global settings
    queue_init(&PreTrigBuffer, sizeof(TraceLine), PreTrigLength);   // define pre-trigger buffer, default 32 samples
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

    // to be checked: if the tracer CDC port is not connected we can simply empty the buffer
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

    if (!queue_is_empty(&TraceBuffer)) {
        // only do something if there is something in the tracebuffer
        tud_task();             // must keep the USB port updated

        // read from the Trace Buffer
        queue_remove_blocking(&TraceBuffer, &TraceSample);

        if (!globsetting.get(tracer_enabled)) return;        // tracer is disabled
        if (!trace_enabled) return;

        // we could get out here if there is no CDC port connected for the tracer
        if (!cdc_connected(ITF_TRACE)) return;

        // now start analyzing the trace sample

        sinstr = TraceSample.isa_instruction;       // sync is still in here
        instr = sinstr & 0x03FF;
        sync  = TraceSample.isa_instruction >> 11;

        addr  = TraceSample.isa_address;
        cycle_prev = cycle;
        cycle = TraceSample.cycle_number;
        
        DATAsample1 = TraceSample.data1;        // D31..D00
        DATAsample2 = TraceSample.data2;        // D55..D32, right justified 0xfeffffff

        bank = TraceSample.bank;

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
            // sample not to be blocked, now build the trace string 

            if (prev_block) {
                // there was a previous block, indicate skipped lines
                prev_block = false;
                overflow = '=' ;
            } 

            // build the trace/disassembly string
            TracePrintLen = 0;
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"%c  %6d  %04X-%1d  %01X  %03X  ", overflow, cycle, addr, bank, sync, instr);
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"%01X.%05X%05X.%01X.%02X  ", data_s, data_m2, data_m1, data_xs, data_x);
            if (TraceSample.xq_instr == 0) {
                // print .... if no instruction was executed by TULIP
                TracePrintLen += sprintf(TracePrint + TracePrintLen,"...  ", TraceSample.xq_instr);  // instruction decoded by TULIP
            } else {
                // decoded instruction, print it
                TracePrintLen += sprintf(TracePrint + TracePrintLen,"%03X  ", TraceSample.xq_instr & 0x3FF);  // instruction decoded by TULIP
            }
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"R%03X  ", TraceSample.ramslct);
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"C%01X  ", TraceSample.xq_carry);    

            // work out the FI bits
            // if one of the bits is low, we count this as a valid flag

            #if (TULIP_HARDWARE == T_DEVBOARD)
                // for the DevBoard we have real FI tracing
                // handle FI signal
                fi1 = TraceSample.fi1;
                fi2 = TraceSample.fi2;                  // need to shift
                fi = fi2;
                fi = fi << 24;
                fi = fi + fi1;
                // fi1 is D31..D00, fi2 is D55..D32

                TracePrintLen += sprintf(TracePrint + TracePrintLen,"FI"); 
                for (int i = 0; i < 14; i++) {      // all 14 digits
                    if (((fi >> (i * 4)) & 0b1111) != 0b1111 ) {
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"%01X", i); 
                    } else {
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"-"); 
                    }
                }

            #elif (TULIP_HARDWARE == T_MODULE)
            // the module does not have FI tracing, here we get only the flags as driven by the TULIP
            // and the pattern is that of FI_OUT1 and FI_OUT2
            /*
                    #define FI_00           0x00000007      // FI_PBSY, checked with ?PBSY (Wand, Printer(not used))
                    #define FI_01           0x00000070      // FI_CRDR, checked with ?CRDR (Cardreader)
                    #define FI_02           0x00000700      // FI_WNDB, checked with ?WNDB (Wand)
                    #define FI_03           0x00007000      // 
                    #define FI_04           0x00070000      // 
                    #define FI_05           0x00700000      // FI_EDAV, checked with ?EDAV, IR Emitter Diode Available (Blinky)
                    #define FI_06           0x07000000      // FI_IFCR, checked with ?IFCR, Interface Clear Received (HP-IL)
                    #define FI_07           0x70000000      // FI_SRQR, checked with ?SRQR, Service Request Received (HP-IL)

                    // sent on T32..T55
                    #define FI_08           0x00000007      // FI_FRAV, checked with ?FRAV, Frame Available (HP-IL)
                    #define FI_09           0x00000070      // FI_FRNS, checked with ?FRNS, Frame Received Not as Sent (HP-IL)
                    #define FI_10           0x00000700      // FI_ORAV, checked with ?ORAV, Output Register Available (HP-IL) 
                    #define FI_11           0x00007000      //
                    #define FI_12           0x00070000      // FI_ALM, checked with ?ALM, Alarm (Timer)
                    #define FI_13           0x00700000      // FI_SER, checked with ?SER, Service Request (all peripherals)
                */

                TracePrintLen += sprintf(TracePrint + TracePrintLen,"FI"); 
                // for the module we have the FI_OUT1 and FI_OUT2 bits as output by the TULIP
                fi1 = TraceSample.fi1;          // D31..D00
                fi2 = TraceSample.fi2;          // D55..D32
                fi = fi2;
                fi = fi << 32;
                fi = fi | fi1;                 // combine the two parts, shift to the left to get D55..D32

                // for testing print the complete fi word
                // TracePrintLen += sprintf(TracePrint + TracePrintLen," fi1:%08X fi2:%08X", fi1, fi2);

                // now print the FI bits
                 for (int i = 0; i < 14; i++) {   
                    if (((fi >> (i * 4)) & 0x7) == 0x7 ) {
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"%01X", i); 
                    } else {
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"-"); 
                    }
                }

            #endif

            // printf(" %08x", TraceSample.xq_data1) in case emulator internals are needed

            // keep track of the length of the disassembly line for alignment of any text after it
            int dis_len = TracePrintLen + 20;       // allow 20 chars for the disassembly text

            // disassembly of the traceline
            if (sync == 1) {
                // valid instruction
                if ((instr & 0x003) == 0x001) {     // class 1 instruction
                    // Class 1 is 2-word GO/XQ, so handle in the next disassembly line
                    delayed_dis = instr;
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  ..."); 
                } else 
                {
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  %s", mnemonics[instr]); 
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
            } else     
            {
                // no SYNC, so the 2nd word of a multi-byte instruction
                // or under peripheral control, or FETCH S&X
                if ((delayed_dis & 0x003) == 0x001)
                {
                    // class 1 instruction XQ/GO
                    type_GOXQ = instr & 0x003;
                    char goxq_str[10];
                    switch (type_GOXQ)
                    {
                        case 0x000: sprintf(goxq_str, "?NC XQ"); break;
                        case 0x001: sprintf(goxq_str, "?C XQ "); break;
                        case 0x002: sprintf(goxq_str, "?NC GO"); break;
                        case 0x003: sprintf(goxq_str, "?C GO ");  break;
                        default: break;
                    }
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  %s", goxq_str); 
                    TracePrintLen += sprintf(TracePrint + TracePrintLen," %02X%02X", (instr & 0x3FC) >>2, (delayed_dis & 0x3FC) >> 2);
                    delayed_dis = 0;
                } else {
                    // not a class 1 instruction so treat as literal
                    // could be from LDI, under peripheral control or FETCH S&X
                    // implement in disassembler (maybe) later
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  %03X", instr);
                    if ((activeSELP >= 0) && (activeSELP <= 7)) 
                    {
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

            // add tracing for the HP-IL frames and registers if enabled and the HP-IL module is plugged            
            if (globsetting.get(tracer_ilregs_on) && globsetting.get(HPIL_plugged)) {
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

            if (cdc_connected(ITF_TRACE))
            // only send if something is connected, otherwise this will stall
            // better move this test to earlier in the tracer task for performance
            {
                cdc_sendbuf(ITF_TRACE, TracePrint, TracePrintLen);
                cdc_flush(ITF_TRACE);
                tud_task();             // must keep the USB port updated
            }
             
        } 
        else
        {
            // sample skipped
            sample_skipped = true;
        }
    }
}
