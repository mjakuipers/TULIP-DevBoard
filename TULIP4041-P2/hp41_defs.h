/*
 * hp41defs.h
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

// HP41_defs.h
// header file with definitions for HP41 related stuff

// definition of HP41 instructions to be decoded

// instruction modifier with SYNC bit set and the msb copied in bit 10
// instruction as returned as the ISA instruction from the sync state machine
//      0x0xx   ->  0x8xx   bitwise 0000 -> 1000    0->8
//      0x1xx   ->  0x9xx   bitwise 0001 -> 1001    1->9
//      0x2xx   ->  0xExx   bitwise 0010 -> 1110    2->E
//      0x3xx   ->  0xFxx   bitwise 0011 -> 1111    3->F


#ifndef __HP41_DEFS_H__
#define __HP41_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif


#define SYNC_bit        0x800                   // position of SYNC status from SYNC/ISA state machine

// HP41 WROM or WRIT S&X
#define HP41_WROM       0x040
#define inst_WROM       0x840                   // 0x840, WROM instruction with SYNC status bit

// HP41 user memory
#define HP41_RAMSLCT    0x270                   // select user memory register in C[2..0]       10.0111.0000
#define inst_RAMSLCT    0xE70                   //   same, with SYNC bit set                  1110.0111.0000
#define HP41_READDATA   0x038                   // copy active user memory register to C        00.0011.1000
#define inst_READDATA   0x838                   //   same with SYNC bit set                   1000.0011.1000
#define HP41_WRITDATA   0x2F0                   // copy C to active user memory register        10.1111.0000
#define inst_WRITDATA   0xEF0                   //   same with SYNC bit set                   1110.1111.0000

#define inst_JNC        0xE07                   // JNC with SYNC for ALD

// #define HP41_WRIT    0x                      // class 0 write to selected user memory register
// #define HP41_READ    0x                      // class 0 read from selected user memory register
// definition of the HP41_WRIT instruction:
  // Class 0 instructions have the following form :
  //      -- -- -- -- -- -- -- -- 0 0
  //      P3 P2 P1 P0 I3 I2 I1 I0
  //          \/          \/
  //      parameter   instruction type  
//  for the WRITE instruction, I = 0xA, for the READ instruction I = 0xE
//  parameter is the targeted register 
//  READ 0 aliases to READDATA (0x038)
#define mask_CLASS0     0x03F                   // binary 00 0011 1111
#define mask_WRIT       0x028                   // binary 00 0010 1000
#define mask_READ       0x038                   // binary 00 0011 1000


// HEPAX instructions
#define HP41_HEPROMBLK  0x030                   // HEPAX ROMBLK
#define HP41_HEPWPTOG   0x1F0                   // HEPAX WPTOG

// HP41CL instructions
#define HP41_41CLWCMD   0x1FC                   // HP41CL WCMD
#define HP41_41CLEADD   0x0C0                   // HP41CL MAXX EADD=C, Extended RAMSLCT

// HP41 Wand instructions
#define HP41_PBSY      0x3AC                   // set carry if FI 0 set
#define HP41_WNDB      0x22C                   // set carry if DATA in Wand Buffer (FI 2 set)
#define inst_PBSY      0xFAC                   
#define inst_WNDB      0xE2C

// HP41 Bankswitching instructions
#define HP41_ENBANK1    0x100
#define HP41_ENBANK2    0x180
#define HP41_ENBANK3    0x140
#define HP41_ENBANK4    0x1C0

#define inst_ENBANK1    0x900
#define inst_ENBANK2    0x980
#define inst_ENBANK3    0x940
#define inst_ENBANK4    0x9C0

// HP-IL instructions
// instruction type = 0
// parameter is register
#define HP41_IFCR      0x16C                 // set carry if interface ready 
#define HP41_SRQR      0x2AC                 // set carry if interface request service 
#define HP41_FRAV      0x12C                 // set carry if a frame is available from the loop
#define HP41_FRNS      0x26C                 // set carry if frame not returned as it was sent 
#define HP41_ORAV      0x0EC                 // set carry if an output register is available

#define HP41_HPIL_C0    0x200                 // copies C[1..0] to HP-IL register 0
#define HP41_HPIL_C1    0x240                 // copies C[1..0] to HP-IL register 1
#define HP41_HPIL_C2    0x280                 // copies C[1..0] to HP-IL register 2
#define HP41_HPIL_C3    0x2C0                 // copies C[1..0] to HP-IL register 3
#define HP41_HPIL_C4    0x300                 // copies C[1..0] to HP-IL register 4
#define HP41_HPIL_C5    0x340                 // copies C[1..0] to HP-IL register 5
#define HP41_HPIL_C6    0x380                 // copies C[1..0] to HP-IL register 6
#define HP41_HPIL_C7    0x3C0                 // copies C[1..0] to HP-IL register 7

#define HP41_SELP0      0x024                 // select HP-IL peripheral register 0
#define HP41_SELP1      0x064                 // select HP-IL peripheral register 1
#define HP41_SELP2      0x0A4                 // select HP-IL peripheral register 2
#define HP41_SELP3      0x0E4                 // select HP-IL peripheral register 3
#define HP41_SELP4      0x124                 // select HP-IL peripheral register 4
#define HP41_SELP5      0x164                 // select HP-IL peripheral register 5
#define HP41_SELP6      0x1A4                 // select HP-IL peripheral register 6
#define HP41_SELP7      0x1E4                 // select HP-IL peripheral register 7

#define inst_SELP0      0x824                 // select HP-IL peripheral register 0, SYNC bit set
#define inst_SELP1      0x864                 // select HP-IL peripheral register 1, SYNC bit set
#define inst_SELP2      0x8A4                 // select HP-IL peripheral register 2, SYNC bit set
#define inst_SELP3      0x8E4                 // select HP-IL peripheral register 3, SYNC bit set
#define inst_SELP4      0x924                 // select HP-IL peripheral register 4, SYNC bit set
#define inst_SELP5      0x964                 // select HP-IL peripheral register 5, SYNC bit set
#define inst_SELP6      0x9A4                 // select HP-IL peripheral register 6, SYNC bit set
#define inst_SELP7      0x9E4                 // select HP-IL peripheral register 7, SYNC bit set

#define HP41_C_PREG0    0x03A                 // copies HP-IL register 0 to C[1..0], after SELP0..7
#define HP41_C_PREG1    0x07A                 // copies HP-IL register 1 to C[1..0]
#define HP41_C_PREG2    0x0BA                 // copies HP-IL register 2 to C[1..0]
#define HP41_C_PREG3    0x0FA                 // copies HP-IL register 3 to C[1..0]
#define HP41_C_PREG4    0x13A                 // copies HP-IL register 4 to C[1..0]
#define HP41_C_PREG5    0x17A                 // copies HP-IL register 5 to C[1..0]
#define HP41_C_PREG6    0x1BA                 // copies HP-IL register 6 to C[1..0]
#define HP41_C_PREG7    0x1FA                 // copies HP-IL register 7 to C[1..0]

#define HP41_PFSET0    0x003                  // set carry if flag set, after SELP0..7
#define HP41_PFSET1    0x043                  // set carry if flag set
#define HP41_PFSET2    0x083                  // set carry if flag set
#define HP41_PFSET3    0x0C3                  // set carry if flag set
#define HP41_PFSET4    0x103                  // set carry if flag set
#define HP41_PFSET5    0x143                  // set carry if flag set
#define HP41_PFSET6    0x183                  // set carry if flag set
#define HP41_PFSET7    0x1C3                  // set carry if flag set


// HP41 peripherals
#define HP41_PRPHSLCT   0x3F0                 // select peripheral specified in C[2..0]
#define inst_PRPHSLCT   0xFF0                 //   same with SYNC bit set

// values below are as read from the isain state machine
// including SYNC and a copy of the msb
#define inst_SELP9      0xE64                 // 0x264, SELP 9 NUT instruction, starts SELP9 mode, SYNC bit set!
#define SELP9_BUSY      0x003                 // 0x003, set carry if printer busy, no SYNC bit!
#define SELP9_POWON     0x083                 // 0x083, set carry if printer is ON, no SYNC bit!
#define SELP9_VALID     0x043                 // 0x043, set carry if status valid, no SYNC bit!
#define SELP9_PRINTC    0x007                 // 0x007, send byte on C[0..1] to the printbuffer, no SYNC bit!
#define SELP9_RDPTRN    0x03A                 // 0x03A, transfer printer status word to C[10..13], no SYNC bit!
#define SELP9_RTNCPU    0x005                 // 0x005, return control to the HP41 CPU, ends SELP mode, no SYNC bit!

// value of C[2..0] for the PRHPSLCT instruction
#define PRPH_none       0x000                 // no peripheral selected
#define PRPH_41cl       0x0F0                 // HP41CL peripheral mode (not used)
#define PRPH_maxx       0x0F3                 // Monte's MAXX module (not used yet)
#define PRPH_timer      0x0FB                 // Timer module
#define PRPH_cardreader 0x0FC                 // Card reader
#define PRPH_display    0x0FD                 // Display
#define PRPH_wand       0x0FE                 // Wand barcode reader 

#define prt_SMA         15                    // bit 15 - SMA, TRACE mode when set
#define prt_SMB         14                    // bit 14 - SMB, NORM when set, MAN when bit 14 and 15 are clear
#define prt_PRT         13                    // bit 13 - PRT, PRINT key down
#define prt_ADV         12                    // bit 12 - ADV, PAPER ADVANCE key down
#define prt_OOP         11                    // bit 11 - OOP, Out Of Paper
#define prt_LB          10                    // bit 10 - LB , Low BAttery
#define prt_IDL          9                    // bit  9 - IDL, Idle 
#define prt_BE           8                    // bit  8 - BE , Buffer Empty
#define prt_LCA          7                    // bit  7 - LCA, Lower Case Alpha
#define prt_SCO          6                    // bit  6 - SCO, Special Column Output (Graphics mode)
#define prt_DWM          5                    // bit  5 - DWM, Double Wide Mode
#define prt_TEO          4                    // bit  4 - TEO, Type of End-Of-Line
#define prt_EOL          3                    // bit  3 - EOL, Last End-Of-Line
#define prt_HLD          2                    // bit  2 - HLD, Hold for Paper
#define prt_bt1          1                    // bit  1 - not used, always returns 0
#define prt_bt0          0                    // bit  0 - not used, always returns 0

// default value for the printer status register
#define prtstatus_def   0b0000001100000000    // all values zero except:
                                              //  IDLE = 1
                                              //  BUFFER EMPTY = 1


#define prt_SMA_mask    0b1000000000000000
#define prt_SMB_mask    0b0100000000000000
#define prt_PRT_mask    0b0010000000000000
#define prt_ADV_mask    0b0001000000000000
#define prt_OOP_mask    0b0000100000000000
#define prt_LB_mask     0b0000010000000000
#define prt_IDL_mask    0b0000001000000000
#define prt_BE_mask     0b0000000100000000

#define prt_LCA_mask    0b0000000010000000
#define prt_SCO_mask    0b0000000001000000
#define prt_DWM_mask    0b0000000000100000
#define prt_TEO_mask    0b0000000000010000
#define prt_EOL_mask    0b0000000000001000
#define prt_HLD_mask    0b0000000000000100
#define prt_bt1_mask    0b0000000000000010
#define prt_bt0_mask    0b0000000000000001


// FI output definitions (sending lsb first)
// controlling FI_OE, which is an active high signal!
// the flag itself is an active low signal!

// sent on T0..T31
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


#define FI_00_off       0xFFFFFFF0
#define FI_01_off       0xFFFFFF0F
#define FI_02_off       0xFFFFF0FF
#define FI_03_off       0xFFFF0FFF
#define FI_04_off       0xFFF0FFFF
#define FI_05_off       0xFF0FFFFF
#define FI_06_off       0xF0FFFFFF
#define FI_07_off       0x0FFFFFFF

#define FI_08_off       0xFFFFFFF0
#define FI_09_off       0xFFFFFF0F
#define FI_10_off       0xFFFFF0FF
#define FI_11_off       0xFFFF0FFF
#define FI_12_off       0xFFF0FFFF
#define FI_13_off       0xFF0FFFFF


#ifdef __cplusplus
}
#endif

#endif