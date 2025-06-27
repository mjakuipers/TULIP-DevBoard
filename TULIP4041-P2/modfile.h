/*=======================================================================

Author: Warren Furlow (email: warren@furlow.org)

License: PUBLIC DOMAIN - May be freely copied and incorporated into any work

Description:  Describes the structure of the MODULE file for HP-41 ROM images

.MOD File Structure:
These structures define the .MOD file format which replaces the older ROM
image dump formats (ROM, BIN).  MOD format allows the definition of entire
plug-in modules which may be composed of ROM images, RAM, special hardware
etc.  The HP-41C, -CV and -CX base operating system is defined in one MOD
file (ie The CX base includes 4 memory modules, the timer hardware, and
XFuns/XMem registers as well as 6 ROM images).  Additionally, Single Memory,
Quad Memory, XFuns/XMem, XMem, Timer modules can be defined each in their own
MOD file.  Obviously certain configurations do not make sense any more than
with the real hardware and may return an error (ie an HP-41CV AND a Quad
Memory Module).  It is also possible to define MLDL RAM using a blank page.

Strings are null terminated and all unused bytes are set to zero.  Fields are
strictly limited to valid values defined below.  Some combinations of values
would make no sense and not represent any actual hardware.  
File size=sizeof(ModuleFileHeader)+NumPages*sizeof(ModuleFilePage)
=======================================================================*/

/*
 * ffmanager.cpp
 *
 * This file is part of the TULIP4041 project.
 * Copyright (C) 2024 Meindert Kuipers
 *
 * modified from the original version to inlcude the TULIP meta-header
 * MODFile type headers copiued from V41 sources by Warren Furlow (MODFile.h)
 * to support the Flash File manager
 * 
 * USE AT YOUR OWN RISK
 *
 */

#pragma once

// includes for getting the RP2040 relevant types
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
// #include "tulip.h"
// #include "pico/stdlib.h"


typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

#define MOD_FORMAT  "MOD1"
#define MOD_FORMAT2 "MOD2"

/* Module type codes */
#define CATEGORY_UNDEF              0  /* not categorized */
#define CATEGORY_OS                 1  /* base Operating System for C,CV,CX */
#define CATEGORY_APP_PAC            2  /* HP Application PACs */
#define CATEGORY_HPIL_PERPH         3  /* any HP-IL related modules and devices */
#define CATEGORY_STD_PERPH          4  /* standard Peripherals: Wand, Printer, Card Reader, XFuns/Mem, Service, Time, IR Printer */
#define CATEGORY_CUSTOM_PERPH       5  /* custom Peripherals: AECROM, CCD, HEPAX, PPC, ZENROM, etc */
#define CATEGORY_BETA               6  /* BETA releases not fully debugged and finished */
#define CATEGORY_EXPERIMENTAL       7  /* test programs not meant for normal usage */
#define CATEGORY_MAX                7  /* maximum CATEGORY_ define value */

/* Hardware codes */
#define HARDWARE_NONE               0  /* no additional hardware specified */
#define HARDWARE_PRINTER            1  /* 82143A Printer */
#define HARDWARE_CARDREADER         2  /* 82104A Card Reader */
#define HARDWARE_TIMER              3  /* 82182A Time Module or HP-41CX built in timer */
#define HARDWARE_WAND               4  /* 82153A Barcode Wand */
#define HARDWARE_HPIL               5  /* 82160A HP-IL Module */
#define HARDWARE_INFRARED           6  /* 82242A Infrared Printer Module */
#define HARDWARE_HEPAX              7  /* HEPAX Module - has special hardware features (write protect, relocation) */
#define HARDWARE_WWRAMBOX           8  /* W&W RAMBOX - has special hardware features (RAM block swap instructions) */
#define HARDWARE_MLDL2000           9  /* MLDL2000 */
#define HARDWARE_CLONIX             10 /* CLONIX-41 Module */
#define HARDWARE_MAX                10 /* maximum HARDWARE_ define value */

/* relative position codes- do not mix these in a group except ODD/EVEN and UPPER/LOWER */
/* ODD/EVEN, UPPER/LOWER can only place ROMS in 16K blocks */
#define POSITION_MIN                0x1f   /* minimum POSITION_ define value */
#define POSITION_ANY                0x1f   /* position in any port page (8-F) */
#define POSITION_LOWER              0x2f   /* position in lower port page relative to any upper image(s) (8-F) */
#define POSITION_UPPER              0x3f   /* position in upper port page */
#define POSITION_EVEN               0x4f   /* position in any even port page (8,A,C,E) */
#define POSITION_ODD                0x5f   /* position in any odd port page (9,B,D,F) */
#define POSITION_ORDERED            0x6f   /* position sequentially in order of MOD file loading, one image per page regardless of bank */
#define POSITION_MAX                0x6f   /* maximum POSITION_ define value */


// Module Meta Header (added by Meindert Kuipers)
#define FILETYPE_EMPTY              0x00      // indicates an erased file 
#define FILETYPE_DELETED            0x00      // deleted file, not used
#define FILETYPE_MOD1               0x01      // MOD1 file
#define FILETYPE_MOD2               0x02      // MOD2 file
#define FILETYPE_ROM                0x03      // ROM file
#define FILETYPE_QROM               0x04      // highest bit indicates this is QROM, stored in FRAM
#define FILETYPE_FRAM               0x04      // FRAM file, aliased with QROM
#define FILETYPE_UMEM               0x10      // for storing User Memory images
#define FILETYPE_MMAP               0x20      // for storing Module Map
#define FILETYPE_GLOB               0x30      // for storing Global Settings
#define FILETYPE_TRAC               0x40      // for storing Tracer triggers and settings
#define FILETYPE_4041               0x41      // initialization file

#define FILETYPE_ERASED             0x7F      // erased file, not used
#define FILETYPE_DUMMY              0x7F      // indicates an erased file for empty space
                                              // the space after this file contains all FF's ?? to be checked if this can work
                                              // to be further defined if this is workable
#define FILETYPE_FFFF               0xFF      // typically the end of the file system
 
#define FILETYPE_END                0xFF      // unused space, maiden flash
                                              // this is always the last entry in the file list

typedef struct {
  byte      FileType;         // Type of the file  
  char      FileName[31];     // filename
  uint32_t  FileSize;         // total number of bytes in the file contents, this header not included
  uint32_t  NextFile;         // address of the next file, relative to FF_SYSTEM_BASE
} ModuleMetaHeader_t;


/* Module header */
typedef struct {
  char FileFormat[5];     // constant value defines file format and revision
  char Title[50];         // the full module name (the short name is the name of the file itself)
  char Version[10];       // module version, if any
  char PartNumber[20];    // module part number
  char Author[50];        // author, if any
  char Copyright[100];    // copyright notice, if any
  char License[200];      // license terms, if any
  char Comments[255];     // free form comments, if any
  byte Category;          // module category, see codes below
  byte Hardware;          // defines special hardware that module contains
  byte MemModules;        // defines number of main memory modules (0-4)
  byte XMemModules;       // defines number of extended memory modules (0=none, 1=Xfuns/XMem, 2,3=one or two additional XMem modules)
  byte Original;          // allows validation of original contents:
                          //    1 = images and data are original
                          //    0 = this file has been updated by a user application (data in RAM written back to MOD file, etc)
  byte AppAutoUpdate;     // tells any application to: 1=overwrite this file automatically when saving other data, 0=do not update
  byte NumPages;          // the number of pages in this file (0-255, but normally between 1-6)
  byte HeaderCustom[32];  // for special hardware attributes
} ModuleFileHeader_t;

// page struct for MOD1
typedef struct {
  byte Image[5120];       // the image in packed format (.BIN file format)
  byte PageCustom[32];    // for special hardware attributes
} V1_t;

/*
  The format of a packed ROM file (.BIN format) is as follows:
  BIN - This format is used by Emu41 (J-F Garnier) and HP41EPC (HrastProgrammer).
      Note: HP41EPC uses BIN format but names them .ROM files.
      All bits are packed into 5120 bytes, 4 machine words are packed into 5 bytes:
        Byte0=Word0[7-0]
        Byte1=Word1[5-0]<<2 | Word0[9-8]
        Byte2=Word2[3-0]<<4 | Word1[9-6]
        Byte3=Word3[1-0]<<6 | Word2[9-4]
        Byte4=Word3[9-2]
*/

// page struct for MOD2
typedef struct {
  word Image[4096];       // the image in unpacked format (.ROM file format)
  byte PageCustom[32];    // for special hardware attributes
} V2_t;

// page struct for module
typedef struct {
  char Name[20];          // normally the name of the original .ROM file, if any
  char ID[9];             // ROM ID code, normally two letters and a number are ID and last letter is revision - if all zeros, will show up as @@@@
  byte Page;              // the page that this image must be in (0-F, although 8-F is not normally used) or defines each page's
                          // position relative to other images in a page group, see codes below
  byte PageGroup;         // 0=not grouped, otherwise images with matching PageGroup values (1..8) are grouped according to POSITION code
  byte Bank;              // the bank that this image must be in (1-4)
  byte BankGroup;         // 0=not grouped, otherwise images with matching BankGroup values (1..8) are bankswitched with each other
  byte RAM;               // 0=ROM, 1=RAM - normally RAM pages are all blank if Original=1
  byte WriteProtect;      // 0=No or N/A, 1=protected - for HEPAX RAM and others that might support it
  byte FAT;               // 0=no FAT, 1=has FAT
} ModuleHeader_t;

// page struct for module
typedef struct {
  ModuleHeader_t header;
  union {
    byte image;
    V1_t v1;              // page struct for MOD1
    V2_t v2;              // page struct for MOD2
  };
} ModuleFilePage_t;

// struct for a straight .ROM image file
typedef struct {
  uint16_t RomImage[4096];    // the image in unpacked format (.ROM file format)
} ROMFilePage_t;

// page struct for complete file header as stored in FLASH or FRAM
typedef struct {
  ModuleMetaHeader_t MetaHeader;
  ModuleFileHeader_t ModuleHeader;
} ModuleFlashPage_t;

