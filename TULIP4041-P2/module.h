/*
 * module.h
 *
 * This file is part of the TULIP4041 project
 * File based on Thomas Fänge implementation of module.h
 * 
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


#ifndef __MODULE_H__
#define __MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <pico.h>
#include "pico/stdlib.h"

#include "hpinterface_hardware.h"
#include "ffmanager.h"	
#include "modfile.h"
#include "fram.h"


// bit field for the Image flags, used for Pages
enum {
    PAGE_none      = 0x0000,       // nothing plugged here
    PAGE_ACTIVE    = 0x0001,       // page is active (has a valid content)
    PAGE_FLASH     = 0x0002,       // page is in FLASH or FRAM
    PAGE_ROM       = 0x0004,       // page is ROM or not
    PAGE_MOD       = 0x0008,       // page is MOD1 or MOD2
    PAGE_ENABLED   = 0x0010,       // page is enabled for reading
    PAGE_DIRTY     = 0x0020,       // page is dirty
    PAGE_WRITEABLE = 0x0040,       // page is write enabled
    PAGE_RESERVED  = 0x0080,       // page reserved by a physical module
    PAGE_EMBEDDED  = 0x0100,       // page is embedded in the firmware
                                   // used for the HP-IL and Printer ROMs
};


#define PAGE_FLAGS  (PAGE_ACTIVE | PAGE_FLASH | PAGE_ROM | PAGE_MOD | PAGE_ENABLED | PAGE_DIRTY | PAGE_WRITEABLE)

// bit field for the Bank flags, used for Banks
// value 0 is always default
enum {
    BANK_none      = 0x0000,       // nothing plugged here
    BANK_ACTIVE    = 0x0001,       // 1: bank is active (has a valid content)
    BANK_FLASH     = 0x0002,       // 1: bank is in FLASH, 0: bank is in FRAM
    BANK_ROM       = 0x0004,       // 1: bank is ROM, 0: not ROM (but MOD1 or MOD2)
    BANK_MOD       = 0x0008,       // 1: bank is MOD1, 0: bank is MOD2
    BANK_ENABLED   = 0x0010,       // 1: bank is enabled for reading, 0: not enabled for reading
    BANK_DIRTY     = 0x0020,       // 1: bank is dirty (written to but not saved), 0: bank is OK
                                   // remnant from Tiny41, not used in TULIP for now
                                   // may be needed if Read-Modify-Write for QROm MOD files is too slow on FRAM
    BANK_WRITEABLE = 0x0040,       // 1: bank is write enabled, 0: no writing possible
    BANK_RESERVED  = 0x0080,       // 1: bank reserved by a physical module, 0: pluggable for TULIP
    BANK_EMBEDDED  = 0x0100,       // page is embedded in the firmware
                                   // used for the HP-IL and Printer ROMs
                                   // other bits are reserved for future use
};

#define BANK_FLAGS  (BANK_ACTIVE | BANK_FLASH | BANK_ROM | BANK_MOD | BANK_ENABLED | BANK_DIRTY | BANK_WRITEABLE)

// bit fields for quick access to the flags
#define ACTIVE_ROM_FLASH   (BANK_ACTIVE | BANK_FLASH | BANK_ROM | PAGE_ENABLED)
#define ACTIVE_MOD1_FLASH  (BANK_ACTIVE | BANK_FLASH | BANK_MOD | PAGE_ENABLED)
#define ACTIVE_MOD2_FLASH  (BANK_ACTIVE | BANK_FLASH | BANK_MOD | PAGE_ENABLED)

#define ACTIVE_EMBEDDED_ROM (BANK_ACTIVE | BANK_FLASH | BANK_ROM | PAGE_EMBEDDED | PAGE_ENABLED)


// definition of ROM image sources
typedef enum {rom, qram, ram} ROM_TYPE;

// #define EMBED_RAM

// definitions for the Module and Page handling
#define RAM_SIZE    0x1000
#define PAGE_SIZE   0x1000
#define PAGE_MASK   0x0FFF
#define ADDR_MASK   0xFFFF
// #define INST_MASK   (BIT_10-1)
#define FIRST_PAGE  0x04
#define NR_PAGES    0x10
#define LAST_PAGE   (NR_PAGES - 1)

#define PAGE(p)     (p>>12)
#define ISA_SHIFT   44
//#define TRACE_ISA
#define QUEUE_STATUS  


void fram_rommap_init(); 

inline uint16_t swap16(uint16_t b)
{
  return __builtin_bswap16(b);
}

// Helper to decode the bank instruction
static uint8_t nBank[4] = {0,2,1,3};

static const char __in_flash("emb2")HPChar[] = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ |\"#$%&`()*+{-}/0123456789†,<=>?"; 

static const char __in_flash("emb1")*PageText[] = 
{
//"12345678901234567890123456789012" (31 chars max)
  "HP41 System ROM0",                   // Page 0, always reserved
  "HP41 System ROM1",                   // Page 1, always reserved   
  "HP41 System ROM2",                   // Page 2, always reserved
  "HP41CX XFunctions ROM",              // Page 3, always reserved
  "use for Take Over ROM",              // Page 4, used for Take Over ROMs
  "use for TIME/CX XFN",                // Page 5, used for TIME/CX XFN
  "use for Printers",                   // Page 6, used for Printers
  "use for HP-IL",                      // Page 7, used for HP-IL
  "Page 8 / Port 1 L",                  // Page 8, regular page
  "Page 9 / Port 1 U",                  // Page 9, regular page 
  "Page A / Port 2 L",                  // Page A, regular page  
  "Page B / Port 2 U",                  // Page B, regular page
  "Page C / Port 3 L",                  // Page C, regular page
  "Page D / Port 3 U",                  // Page D, regular page
  "Page E / Port 4 L",                  // Page E, regular page
  "Page F / Port 4 U",                  // Page F, regular page
};

static const char __in_flash("emb1")*EmbeddedText[] = 
{
//"12345678901234567890123456789012" (31 chars max)
  "Embedded HP-IL Module",                   // For the HP-IL ROM in Page 7
  "Embedded HP-IL Printer",                  // For the HP-IL Printer ROM in Page 6
  "Embedded HP82143A Printer",               // For the HP82143A Printer ROM in Page 6
};

// define a single Bank in a Page
struct CBank {
  uint32_t  b_img_rom;      // Offset to current image in FLASH or FRAM
  uint16_t  *b_img_data;    // hard address of the image in FLASH
  uint16_t  b_img_flags;    // Flags, type and status of the Bank
  char      b_img_name[32]; // Filename of the plugged module (in case of a ROM file usually)
  uint32_t  b_img_file;     // Offset to the MOD or ROM file in FLASH or FRAM
                            // in case more info is needed
};

// define a single Page with 4 Banks
// used by the CModule class

#define bank_mask   0x03    // mask to get the active number from m_bank
#define bank_sticky 0x80    // sticky bit to prevent resetting bank on light/deep SLEEP
                            // used for ZEPROM emulation
struct CPage{
  CBank     m_banks[5];     // The four Banks in the Page, we use 5 because of the 1..4 range used
                            // Bank 0 is used for generic information about the Page                      
  uint8_t   m_bank;         // Current active bank in this page, when 0 this Page is not in use
                            // defaults to 1 when plugged, use the sticky bit for ZEPROM emulation
};

// keep in mind that TULIP_Pages.mbanks[0] contains generic information about the Page
// so the Banks are numbered 1..4 in the Page structure for Pages and Banks that are really plugged



// This class handles all ports, banks and inserted modules
// Main class that is called by the application to handle
// Thomas originally had a CModule class that was used to handle the modules
// and the banks. This class is integrated in the CModules class
// 
// the following functions are implemented:
// - plug() to plug a module to a port/bank
// - unplug() to unplug a module from a port/bank
// - image_offs() to return the offset to the image in the given Page/Bank
// - clearAll() to clear all modules in the system
// - getword() to read a word from the module (fast)
// - wrom() to write a word to the module (fast)
// - isplugged() to check if a module is plugged (fast)
// - getflags() to get the flags of a bank
// - setflags() to set the flags of a bank
// - getname() to get the filename of a bank, get filename of the module
// - setname() to set the filename of a bank, set filename of the module
// - gettype() to get the type of a bank
// - selectbank() to select a bank (fast)
// - getbank() to get the selected bank (fast)

class CModules {
// private:
  


public:
  CModules() {
    clearAll();                   // initialize all modules
  }

  CPage Pages[NR_PAGES];          // All pages in the HP41 system 

  // called on initialization
  // inititialize all memory space for the modules
  void clearAll() {
    memset(Pages, 0, sizeof(Pages));

    // set the initial values for the banks

    for (int i = 0; i < NR_PAGES; i++) {
      Pages[i].m_banks[1].b_img_flags = BANK_none;                            // nothing is plugged
      Pages[i].m_banks[1].b_img_rom = 0;                                      // no image in the Page
      Pages[i].m_banks[1].b_img_data = NULL;                                  // no image data in the Page
      Pages[i].m_banks[1].b_img_file = 0;                                     // no image file in the Page
      Pages[i].m_banks[1].b_img_name[0] = 0;                                  // no name for the Page yet
      Pages[i].m_bank = 0;                                                    // no active bank in the Page
      // copy the name of the Page from the PageText array
      // ensure that the name is null terminated
      memset(Pages[i].m_banks[1].b_img_name, 0, sizeof(Pages[i].m_banks[1].b_img_name)); // clear the name
      strncpy(Pages[i].m_banks[1].b_img_name, PageText[i], sizeof(Pages[i].m_banks[1].b_img_name) - 1);
      Pages[i].m_banks[1].b_img_name[sizeof(Pages[i].m_banks[1].b_img_name) - 1] = '\0'; // ensure null termination
    }

    Pages[0].m_banks[0].b_img_flags = PAGE_RESERVED;                          // Page 0 is always reserved
    Pages[1].m_banks[0].b_img_flags = PAGE_RESERVED;                          // Page 1 is always reserved
    Pages[2].m_banks[0].b_img_flags = PAGE_RESERVED;                          // Page 2 is always reserved
    Pages[3].m_banks[0].b_img_flags = PAGE_RESERVED;                          // Page 3 is always reserved

    Pages[0].m_banks[1].b_img_flags = BANK_RESERVED;                          // Page 0 is always reserved
    Pages[1].m_banks[1].b_img_flags = BANK_RESERVED;                          // Page 1 is always reserved
    Pages[2].m_banks[1].b_img_flags = BANK_RESERVED;                          // Page 2 is always reserved
    Pages[3].m_banks[1].b_img_flags = BANK_RESERVED;                          // Page 3 is always reserved

    // All other values are already initialized at 0, nothing to do there
    // the initialization routine will check if the FRAM copy is valid and initialized
    // and copy to FRAM if needed
  }
  
  // plugs a module in a Page/Bank
  // the arguments are:
  //     port: the port number (0..15)
  //     bank: the bank number (1..4)
  //     flags: the flags of the image. Image type is in the flags
  //     img_file: offset to the file in the file system
  //     the filename and type handled by other functions
  // no check is done on validity of the parameters, should be done by the caller
  void plug(int port, int bank, uint16_t flags, uint32_t image_offs) {
    Pages[port].m_banks[bank].b_img_flags = flags;
    Pages[port].m_banks[bank].b_img_rom   = image_offs;
    Pages[port].m_bank = 1; // set the active bank to 1, this is the default bank

    // set the hard pointer to the image in FLASH
    Pages[port].m_banks[bank].b_img_data = (uint16_t *)(FF_SYSTEM_BASE + image_offs + sizeof(ModuleMetaHeader_t));

    // set the name of the module in the bank to the file name
    // take it from the name in the offset
    // get a pointer to the filename in the file system
    ModuleMetaHeader_t *MetaH = (ModuleMetaHeader_t *)(FF_SYSTEM_BASE + image_offs);

    // copy the file name to the bank
    strncpy(Pages[port].m_banks[bank].b_img_name, MetaH->FileName, sizeof(Pages[port].m_banks[bank].b_img_name) - 1);
    Pages[port].m_banks[bank].b_img_name[sizeof(Pages[port].m_banks[bank].b_img_name) - 1] = '\0'; // ensure null termination

  }

  // plug one of the embedded modules in a Page
  // this applies to the HP-IL module, the HP-IL Printer module and the HP82143A Printer module
  // always plugged in Bank 1
  // the arguments are:
  //     Page: the Page number (0..15), but normally only 6 or 7
  //     flags: the flags of the image. Image type is in the flags
  //     img_pointer: pointer to the embedded image in FLASH
  void plug_embedded(int port, int bank, uint16_t flags, const uint16_t *img_pointer) {
    Pages[port].m_banks[bank].b_img_flags = flags;
    Pages[port].m_banks[bank].b_img_rom   = 0; // FLASH offset not applicable here
    Pages[port].m_bank = 1; // set the active bank to 1, this is the default bank

    // set the hard pointer to the image in FLASH
    Pages[port].m_banks[bank].b_img_data = (uint16_t *)img_pointer;

    #ifdef DEBUG
    cli_printf("  Embedded XROM 0x%04X @ 0x%08X", Pages[port].m_banks[bank].b_img_data[0], img_pointer);
    #endif

    // and set the name of the ROM in the Page
    if ((port == 6) && (img_pointer[1] == 0x001B)) {
      // this is the HP-IL Printer module if the number of functions is 0x1B
      // the HP82143A printer has 0x19 functions
      strncpy(Pages[port].m_banks[bank].b_img_name, EmbeddedText[1], sizeof(Pages[port].m_banks[bank].b_img_name) - 1);
    } else if ((port == 6) && (img_pointer[1] == 0x0019)) {
      // this is the HP82143A Printer module
      strncpy(Pages[port].m_banks[bank].b_img_name, EmbeddedText[2], sizeof(Pages[port].m_banks[bank].b_img_name) - 1);
    } else if (port == 7) {
      // this is the HP-IL module
      strncpy(Pages[port].m_banks[bank].b_img_name, EmbeddedText[0], sizeof(Pages[port].m_banks[bank].b_img_name) - 1);
    }

    #ifdef DEBUG
    cli_printf("  Plugged embedded module in P %d, B %d, f: 0x%04X, img_pointer: %p", port, bank, flags, img_pointer);
    #endif
  }

  // unplugs an image from a bank
  // the arguments are:
  //     port: the port number (0..15)
  //     bank: the bank number (1..4)
  void unplug(int port, int bank) {
    // cannot unplug the reserved pages 0..3
    if ((port == 0) || (port == 1) || (port == 2) || (port == 3)) {
      return; 
    }
    Pages[port].m_banks[bank].b_img_rom = 0;
    Pages[port].m_banks[bank].b_img_flags = BANK_none;

    // restore image file name to the original init value
    strncpy(Pages[port].m_banks[1].b_img_name, PageText[port], sizeof(Pages[port].m_banks[1].b_img_name) - 1);
    Pages[port].m_banks[1].b_img_name[sizeof(Pages[port].m_banks[1].b_img_name) - 1] = '\0'; // ensure null termination

    // Pages[port].m_banks[bank].b_img_name[0] = 0;          // image file name
    Pages[port].m_banks[bank].b_img_file = 0;  
    Pages[port].m_banks[bank].b_img_data = NULL;          // clear the pointer to the image in FLASH

    if (bank == 1) {
      // if we unplug bank 1, we need to reset the active bank and disable ZEPROM emulation
      Pages[port].m_bank = 0;       // no active bank in the Page
    }
  }


  // read a ROM word given the address
  // Does check if a module is plugged
  // the address is the address in the module space (0..0xFFFF)
  // should not be called if a physical module is plugged
  // returns 0 if there is no plugged ROM
  // returns the word from Bank 1 of the enabled Bank is not plugged/enabled
  uint16_t __not_in_flash() getword(uint16_t addr, uint8_t bank) {
    // get the page and bank from the address
    int port = PAGE(addr);            // work out the page from the address
    uint8_t bk = bank;                // get the bank number from the bank argument
    uint16_t word = 0;                // word to return
    uint16_t res1, res2, result;
    // we have the following options:
    // embedded image in FLASH
    // first check if the page is active and/or enabled

    if ((bk > 1) && !(Pages[port].m_banks[bank].b_img_flags & PAGE_ENABLED)) {
      // bank is not enable, return the word for Bank 1
      bk = 1; // switch to Bank 1 
    }

    if ((Pages[port].m_banks[bank].b_img_flags & PAGE_ENABLED) == 0) return 0;  // bank is not enabled

    // get the word if the page is an embedded page
    if ((Pages[port].m_banks[bank].b_img_flags & PAGE_EMBEDDED) != 0) {
      // this is an embedded module, so we can read the word directly from the image

      word = Pages[port].m_banks[bank].b_img_data[addr & PAGE_MASK]; // get the word from the image in FLASH
      return word; // return the word from the image
    }

    // get the word if the page is of the ROM type
    if ((Pages[port].m_banks[bank].b_img_flags & PAGE_ROM) != 0) {
      // this is a ROM module, so we can read the word directly from the image but byte swapping is needed
      word = swap16(Pages[port].m_banks[bank].b_img_data[addr & PAGE_MASK]); // get the word from the image in FLASH
      return word; // return the word from the image
    }

    // get the word if the page is of the MOD1 type
    if ((Pages[port].m_banks[bank].b_img_flags & PAGE_MOD) != 0) {
      // this is a MOD1 module, and the image data is compressed
      // first get the relevant words from the image and decompress them
      if (Pages[port].m_banks[bank].b_img_data == NULL) return 0; // no image data available
        /*  The format of a packed ROM file (.BIN format) is as follows:
            BIN - This format is used by Emu41 (J-F Garnier) and HP41EPC (HrastProgrammer).
            Note: HP41EPC uses BIN format but names them .ROM files.
            All bits are packed into 5120 bytes, 4 machine words are packed into 5 bytes:
              Byte0=Word0[7-0]
              Byte1=Word1[5-0]<<2 | Word0[9-8]
              Byte2=Word2[3-0]<<4 | Word1[9-6]
              Byte3=Word3[1-0]<<6 | Word2[9-4]
              Byte4=Word3[9-2]
        */
        // the image is packed in 4 words per 5 bytes, so we need to unpack it
        // need to evaluate the code below for speed and improve where possible
        uint8_t *bin = (uint8_t*)Pages[port].m_banks[bank].b_img_data;

        uint16_t offset = (addr * 5) / 4;            // offset in the packed ROM file of the first byte
        int shift1 = (addr & 0x0003) * 2;
        int shift2 = 8 - shift1;
        uint16_t mask1 = 0xFF << shift1;
        uint16_t mask2 = 0xFF >> (shift2 -2);
        res1 = (bin[offset] & mask1) >> shift1;    // get the first part of the word
        res2 = (bin[offset + 1] & mask2);
        res2 = res2 << shift2; // get the second part of the word

        result = res1 + res2;
        return result;
    }

    // next check the code for a MOD2 image, but should be identical to the ROM image
    // do not implement this yet, as we do not have MOD2 images in the system

    // if we get here, we have no valid image in the bank
    return 0;
  }

  // read the flags from a Page given the address
  // always reads from the active bank of the module
  // Does not check if a module is plugged, this should be done by the caller
  // the address is the address in the module space (0..0xFFFF)
  uint16_t __not_in_flash() getflags(int port, int bank) {
    // returnh the flags given port and bank
    return (Pages[port].m_banks[bank].b_img_flags); // return the flags from the image
  }

  void setComment(int port, int bank, const char *comment) {
    // set the comment for the Page/Bank
    // copy the comment to the b_img_name field
    // shorten the comment to 31 chars
    if (comment == NULL) {
      // if no comment is given, set the name to empty
      Pages[port].m_banks[bank].b_img_name[0] = '\0';
      return;
    }
    strncpy(Pages[port].m_banks[bank].b_img_name, comment, sizeof(Pages[port].m_banks[bank].b_img_name) - 1);
    Pages[port].m_banks[bank].b_img_name[sizeof(Pages[port].m_banks[bank].b_img_name) - 1] = '\0'; // ensure null termination
  }

  void setFlags(int port, int bank, uint16_t flags) {
    // set the flags for the Page/Bank
    // the flags are a bit field of the BANK_FLAGS
    Pages[port].m_banks[bank].b_img_flags = flags; // set the flags for the image
  }

  bool __not_in_flash() isEmbeddedROM(int Port, int Bank) {
    // get the page and bank from the address
    return (Pages[Port].m_banks[Bank].b_img_flags & BANK_EMBEDDED); // return true if the image is embedded
  }

  bool isReserved(int port) {
    // check if the Page/Bank is reserved
    return ((Pages[port].m_banks[0].b_img_flags & BANK_RESERVED) ||
            (Pages[port].m_banks[1].b_img_flags & BANK_RESERVED) ||
            (Pages[port].m_banks[2].b_img_flags & BANK_RESERVED) ||
            (Pages[port].m_banks[3].b_img_flags & BANK_RESERVED) ||
            (Pages[port].m_banks[4].b_img_flags & BANK_RESERVED));
  }

  bool isUsed(int Port, int Bank) {
    // check if the Page/Bank is used
    return ((Pages[Port].m_banks[Bank].b_img_flags & BANK_ACTIVE) ||
            (Pages[Port].m_banks[Bank].b_img_flags & BANK_ENABLED) ||
            (Pages[Port].m_banks[Bank].b_img_flags & BANK_EMBEDDED) ||
            isReserved(Port));
  }

  bool is_rommmap_inited() {
    // check if the ROM map is initialized in FRAM
    // can only do this when POW is low!
    uint16_t init_val = 0;
    init_val = fr_read16(FRAM_INIT_ADDR);              // read the initialization value from FRAM
    return (init_val == FRAM_INIT_VALUE);     // check if the value is the initialization value
  }

  void init_rommap() {
    // initialize the ROM map in FRAM   
    uint16_t init_val = 0x4041; // value to indicate that the FRAM ROMMAP is initialized
    uint8_t buf[sizeof(Pages)];

    clearAll();    // initialize the Pages structure

    // save the ROM map in FRAM
    void *Pages_ptr = (void*)(&Pages[0]);               // get a pointer to the ram map struct
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_ROMMAP_START, (uint8_t*)Pages_ptr, sizeof(Pages));
    #ifdef DEBUG
      cli_printf("  ROM map initialized in FRAM, written %d bytes", sizeof(Pages));
    #endif
    // write the initialization value to FRAM
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_INIT_ADDR, (uint8_t*)&init_val, sizeof(init_val));
    #ifdef DEBUG
      cli_printf("  ROM map initialized in FRAM, written init value 0x%04X", init_val);
    #endif

  }

  void save() {
    // save the rommap in FRAM
    // can only do this when POW is low!
    if (gpio_get(P_PWO) == 0) {      
      void *Pages_ptr = (void*)(&Pages[0]);               // get a pointer to the ram map struct
      fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_ROMMAP_START, (uint8_t*)Pages_ptr, sizeof(Pages));
      #ifdef DEBUG
        cli_printf("  ROM map saved to FRAM, written %d bytes", sizeof(Pages));
      #endif
    }
  }

  void retrieve() {
    // retrieve the rommap from FRAM
    // can only do this when POW is low!
    if (gpio_get(P_PWO) == 0) {      
      void *Pages_ptr = (void*)(&Pages[0]);               // get a pointer to the ram map struct
      fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_ROMMAP_START, (uint8_t*)Pages_ptr, sizeof(Pages));
      #ifdef DEBUG
        cli_printf("  ROM map retrieved from FRAM, read %d bytes\n", sizeof(Pages));
      #endif
    }
  }

  bool __not_in_flash()isPlugged(int port, int bank) {
    // check if a module is plugged in the given Page/Bank
    return (Pages[port].m_banks[bank].b_img_flags & PAGE_ACTIVE);
  }

  bool __not_in_flash()isEnabled(int port, int bank) {
    // check if a module is enabled in the given Page/Bank
    return (Pages[port].m_banks[bank].b_img_flags & PAGE_ENABLED);
  }

  
  // returns the offset to the image in the given Page/Bank 
  uint32_t image_offs(int port, int bank) {
    if ((port < 0) || (port >= NR_PAGES)) return 0;
    if ((bank < 0) || (bank > 4)) return 0;
    if (Pages[port].m_banks[bank].b_img_rom == 0) return 0;
    return Pages[port].m_banks[bank].b_img_rom;
  }

  // returns the XROM number of the plugged module
  uint16_t getXROM(int port, int bank) {
    if ((port < 1) || (port >= NR_PAGES)) return 0;
    if ((bank < 1) || (bank > 4)) return 0;
    if (Pages[port].m_banks[bank].b_img_rom == 0) return 0;
    return swap16(Pages[port].m_banks[bank].b_img_data[0]);  // get the XROM number from the image
  }

  // returns a pointer to the filename of the plugged module
void getFileName(int port, int bank, char *filename) {
  for (int i = 0; i < 32; i++) {
    filename[i] = Pages[port].m_banks[bank].b_img_name[i]; // copy the name to the output buffer
  }
  // filename[sizeof(Pages[port].m_banks[bank].b_img_name) - 1] = '\0'; // ensure null termination
}


  // returns the revision string of the plugged module
  // no check if a module is plugged!
  int getRevision(int port, int bank, char *rev) {
    static char p[6];      
    if ((port < 1) || (port >= NR_PAGES)) return false;
    if ((bank < 1) || (bank > 4)) return false;

    if (Pages[port].m_banks[bank].b_img_flags & BANK_EMBEDDED) {
      // this is an embedded module, so we can read the revision from the image
      // without swapping the words
      rev[0] = HPChar[(Pages[port].m_banks[bank].b_img_data[0xFFE]) & 0x3F]; 
      rev[1] = HPChar[(Pages[port].m_banks[bank].b_img_data[0xFFD]) & 0x3F];
      rev[2] = '-';
      rev[3] = HPChar[(Pages[port].m_banks[bank].b_img_data[0xFFC]) & 0x3F];
      rev[4] = HPChar[(Pages[port].m_banks[bank].b_img_data[0xFFB]) & 0x3F];
      rev[5] = 0;    // end of string       
      return true; 
    } else {
      // this is not an embedded module, so we can read the revision from the image
      // but the words must be byte swapped
      rev[0] = HPChar[swap16(Pages[port].m_banks[bank].b_img_data[0xFFE]) & 0x3F]; 
      rev[1] = HPChar[swap16(Pages[port].m_banks[bank].b_img_data[0xFFD]) & 0x3F];
      rev[2] = '-';
      rev[3] = HPChar[swap16(Pages[port].m_banks[bank].b_img_data[0xFFC]) & 0x3F];
      rev[4] = HPChar[swap16(Pages[port].m_banks[bank].b_img_data[0xFFB]) & 0x3F];
      rev[5] = 0;    // end of string 
      return true;
    }
    return true;
  }

  // returns the number of functions in the plugged module
  uint16_t getFunctions(int port, int bank) {
    if ((port < 1) || (port >= NR_PAGES)) return 0;
    if ((bank < 1) || (bank > 4)) return 0;
    if (Pages[port].m_banks[bank].b_img_rom == 0) return 0;
    return swap16(Pages[port].m_banks[bank].b_img_data[1]); // get the number of functions from the image
  }


  void dumpAll() {
    // dump all pages and banks
    for (int i = 0; i < NR_PAGES; i++) {
      cli_printf("Page %d: %s", i, Pages[i].m_banks[1].b_img_name);
      for (int j = 0; j <= 4; j++) {
          cli_printf("  Bank %X: %s, flags: 0x%04X, ROM offset: 0x%08X - 0x%08x", j, 
                                                                          Pages[i].m_banks[j].b_img_name,
                                                                          Pages[i].m_banks[j].b_img_flags, 
                                                                          Pages[i].m_banks[j].b_img_rom,
                                                                          Pages[i].m_banks[j].b_img_data);
      }
    }
  }

  void dumpPage(int port) {
    // dump a single page
    if ((port < 0) || (port >= NR_PAGES)) return; // invalid port
    cli_printf("Page %X: %s", port, Pages[port].m_banks[1].b_img_name);
    for (int j = 0; j <= 4; j++) {
        cli_printf("  Bank %d: %s, flags: 0x%04X, ROM offset: 0x%08X - 0x%08x", j, 
                                                                        Pages[port].m_banks[j].b_img_name,
                                                                        Pages[port].m_banks[j].b_img_flags, 
                                                                        Pages[port].m_banks[j].b_img_rom,
                                                                        Pages[port].m_banks[j].b_img_data);
    }   
  }
  
/*

  void remove(int port) {
    m_modules[port].clr();
	}
  bool isDirty(int port) {
    return m_modules[port].isDirty();
  }
  void clear(int port) {
    m_modules[port].clear();
  }
  bool isInserted(int port) {
    return m_modules[port].isInserted();
  }
  bool isLoaded(int port) {
    return m_modules[port].isLoaded();
  }
  bool isRam(int port) {
    return m_modules[port].isRam();
  }
  void togglePlug(int port) {
    m_modules[port].togglePlug();
  }

  void toggleRam(int port) {
    m_modules[port].toggleRam();
  }
  void setRam(int port) {
    m_modules[port].setRam();
  }
  void setRom(int port) {
    m_modules[port].setRom();
  }
  CModule *port(int p) {
    return &m_modules[p];
  }
  CModule *at(int addr) {
    return port(PAGE(addr));
  }
  CModule *operator [](uint16_t addr) {
    return &m_modules[addr & 0xF];
  }

  */
};

/*

// This class handles a single module Page with 4 banks
class CMod {
  CPage m_page;             // this is the pointer to the Page structure with 4 Banks

  // this class contains the following functions:
  // - set_bank() to connect an image to a bank
  // - set_bank_flags() to change the flags of a bank
  // - clear_bank() to unplug an image from a bank

  // to be implemented
  // - getword          read a word from the module (fast)
  // - wrom             write a word to the module (fast)
  // - isplugged        check if a module is plugged (fast)
  // - getflags         get the flags of a bank
  // - getname          get the filename of a bank, get filename of the module
  // - gettype          get the type of a bank      
  // - selectbank       select a bank (fast)
  // - getbank          get the selected bank
  // - unplug           unplug the complete module, clear all banks
  //   return pointer to a bank image
  //   return pointer to a bank/module file

  // Below from Thomas
  // - bank() to select a bank
  // - image() to return the image pointer of a bank
  // - port() to set the port of the module
  // - haveBank() to check if there are more than the default bank
  // - clr() to deselect any bank
  // - isDirty() to check if the module is updated and not saved
  // - clear() to clear the dirty flag
  // - isInserted() to check if the module is inserted
  // - isLoaded() to check if the module is loaded
  // - isRam() to check if the module is RAM
  // - togglePlug() to toggle the inserted flag
  // - plug() to set the inserted flag
  // - unplug() to clear the inserted flag
  // - toggleRam() to toggle the RAM flag
  // - setRam() to set the RAM flag
  // - setRom() to clear the RAM flag
  // - read() to read a 10-bit instruction from the image
  // - write() to write a 10-bit instruction to the image
  // - read_ram() to read a 10-bit instruction from the RAM
  // - write_ram() to write a 10-bit instruction to the RAM
  // - read_fram() to read a 10-bit instruction from the FRAM
  // - write_fram() to write a 10-bit instruction to the FRAM
  // - read_mod() to read a 10-bit instruction from the MOD
  // - write_mod() to write a 10-bit instruction to the MOD
  // - read_rom() to read a 10-bit instruction from the ROM
  // - read_rom() to write a 10-bit instruction to the ROM
  // - read_rom() to read a 10-bit instruction from the ROM
  // - read_rom() to write a 10-bit instruction to the ROM
  // - read_rom() to read a 10-bit instruction from the ROM



public:
  CMod() {
    // initialize the Banks in this Page
    m_page.m_bank = 1;            // default bank 1
    m_page.m_name[0] = 0;         // no module plugged
    m_page.m_phys = false;    // default is not a physical module
    m_page.m_flags = 0;           // nothing plugged here
    m_page.m_file = 0;            // no file offset
    for (int i = 0; i < 4; i++) {
      //initialize the individual banks
      m_page.m_banks[i].b_img_rom = 0;
      m_page.m_banks[i].b_img_name[0] = 0;
      m_page.m_banks[i].b_img_flags = 0;
      m_page.m_banks[i].b_img_file = 0;
    }
  }

  
  // Connect an image in flash to the correct bank of the module
  // for the image type we use the filetypes defines in modfile.h
  // #define FILETYPE_MOD1               0x01      // MOD1 file
  // #define FILETYPE_MOD2               0x02      // MOD2 file
  // #define FILETYPE_ROM                0x03      // ROM file
  // #define FILETYPE_FRAM               0x04      // FRAM/QROM file
  // the arguments are
  //     offs_image: offset in flash to the image
  //     bank: the bank number to connect the image to
  //     name: the name of the file with the image
  //     flags: the flags of the image. Image type is in the flags
  //     img_file: offset to the file
  void set_bank_flash(uint32_t offs_image, uint8_t bank, char *name, uint8_t flags, uint32_t img_file) {
    if ((bank <= 4) && (bank > 0)) {
      // bank number now validated, must be 1..4
      m_page.m_banks[bank].b_img_rom = offs_image;
      strcpy(m_page.m_banks[bank].b_img_name, name);
      m_page.m_banks[bank].b_img_flags = flags;
      m_page.m_banks[bank].b_img_file = 0;
      m_page.m_name[0] = 0;
    }
  }
  
  void set_page_flash(uint32_t offs_image, char *name, uint8_t flags, uint32_t img_file) {
    strcpy(m_page.m_name, name);
    // m_page.RealModule = false;
    m_page.m_flags = flags;
    m_page.m_file = img_file;
  }

  // to change only the flags of a bank
  void set_bank_flags(uint8_t bank, uint8_t flags) {
    if ((bank <= 4) && (bank > 0)) {
      m_page.m_banks[bank].b_img_flags = flags;
    }
  } 

  // change only the flags of a Page
  void set_page_flags(uint8_t flags) {
    m_page.m_flags = flags;
  }


  // unplugs everything from a bank
  void clear_bank(uint8_t bank) {
    if ((bank <= 4) && (bank > 0)) {
      m_page.m_banks[bank].b_img_rom = 0;
      m_page.m_banks[bank].b_img_name[0] = 0;
      m_page.m_banks[bank].b_img_flags = 0;
    }
  }

  // unplugs everything from a Page
  void clear_page() {
    m_page.m_name[0] = 0;
    // m_page.RealModule = false;
    m_page.m_flags = 0;
    m_page.m_file = 0;
    for (int i = 0; i < 4; i++) {
      m_page.m_banks[i].b_img_rom = 0;
      m_page.m_banks[i].b_img_name[0] = 0;
      m_page.m_banks[i].b_img_flags = 0;
      m_page.m_banks[i].b_img_file = 0;
    }
  }

  // return a word from active bank of the module given the address
  // currently only for ROM images
  uint16_t __not_in_flash() getword(uint16_t addr) {

    uint16_t offset = addr & PAGE_MASK;
    uint32_t img = m_page.m_banks[m_page.m_bank].b_img_rom;
    if (img) {
      return *(uint16_t *)(img + offset);
    } else {
      return 0;
    }
  }

  /*

  // Select current bank given bank-instruction
  void bank(int bank) {
    // Convert from instruction to bank #
    bank = nBank[(bank>>6)&0b11];
    // If bank is present - update image pointer
    if( image(bank) )
      m_img = image(bank);
  }
  // Return pointer to bank image
  uint16_t *image(int bank) {
    return m_banks[bank];
  }
  void port(int p) {
//    m_port = p;
  }
  // True if more than the default bank
  bool haveBank(void) {
    return (m_banks[1]||m_banks[2]||m_banks[3]) ? true : false;
  }
  // Deselect any bank and pull from address space
  void clr(void) {
    m_img = NULL;
    m_flgs = IMG_NONE;
  }
  // Module is updated and not saved
  bool isDirty(void) {
    return m_flgs & IMG_DIRTY;
  }
  void clear(void) {
    m_flgs &= ~IMG_DIRTY;
  }
  bool isInserted(void) {
    return m_flgs & IMG_INSERTED;
  }
  bool isLoaded(void) {
    return m_img != NULL;
  }
  bool isRam(void) {
    return m_flgs & IMG_RAM;
  }
  void togglePlug(void) {
    m_flgs ^= IMG_INSERTED;
  }
  void plug(void) {
    m_flgs |= IMG_INSERTED;
  }
  void unplug(void) {
    m_flgs &= ~IMG_INSERTED;
  }
  void toggleRam(void) {
    m_flgs ^= IMG_RAM;
  }
  void setRam(void) {
    m_flgs |= IMG_RAM;
  }
  void setRom(void) {
    m_flgs &= ~IMG_RAM;
  }
  // Read 10-bit instruction from image given address
  uint16_t read(uint16_t addr) {
    return m_img[addr & PAGE_MASK] & INST_MASK;
  }
  // Read 10-bit instruction from image given address
  uint16_t operator [](uint16_t addr) {
    return m_img[addr & PAGE_MASK] & INST_MASK;
  }
  // Write 10-bit instruction to current image given address
  void write(uint16_t addr, uint16_t dta) {
    m_img[addr & PAGE_MASK] = dta;
    m_flgs |= IMG_DIRTY;
  }

};

*/

#ifdef __cplusplus
}
#endif

#endif    //__MODULE_H__