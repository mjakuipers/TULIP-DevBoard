/*
 * ffmanager.cpp
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

#include "ffmanager.h"

// all functions to manage the 'filesystem' in FLASH and FRAM
// for handling ROM, MOD files and other files

// ROM and MOD files are stored in FLASH, starting at the 1 MByte boundary
// all files start at 256-byte boundaries (last byte/8 bits are 0)

// functions defined:
// BETA version supports only FLASH, no FRAM. FRAM only for settings

// initialization
//  ff_init         - * initialize the file system
//  ff_nuke         - * totally erase all FLASH reserved for the file system
//  ff_erased       - * check if the FLASH range is fully erased

// importing MOD/ROM files
//  ff_lastfree     - find the last erased slot
//  ff_findfile     - search for a file by name and return the pointer
//                  - can check if it exists and compare with the file on uSD
//  ff_findfree     - find next free/erased slot and report size
//  ff_erased       - * check if a range is fully erased
//  ff_writeable    - check if a range byte can be written to FLASH
//  ff_writeableb   - check if a single byte can be programmed in FLASH
//  ff_compare      - compare a range of bytes in FLASH with a buffer (for the update function)

// manipulation of files and FLASH
//  ff_write        - write a single byte to FLASH
//  ff_write_range  - write a range of bytes to FLASH
//  ff_free         - get remaining free space in FLASH
//  ff_delete       - delete a file from FLASH/FRAM
//  ff_erase        - erase an arbitrary block of FLASH (256-byte boundaries)
//                    to prepare for writing a block
//  ff_program      - erase and re-program an arbitrary range of FLASH
//                    based on 256-byte boundaries
//  ff_findfile_n   - find file by index number and return the pointer
//  ff_show         - show FLASH contents in the CLI with 16 bytes per line


/*  layout of FLASH memory


    start of Flash File System at FF_SYSTEM_BASE ( = XIP_BASE + 0x80000)
    a file with FileType FF must always be the last entry and indicates the end of the chain

    FF_SYSTEM_BASE      File_01:      
          typedef struct {                // in modfile.h
              byte      FileType;         // Type of the file  
              char      FileName[31];     // filename
              uint32_t  FileSize;         // total number of bytes in the file, including all headers
              uint32_t  NextFile;         // address of the next file, relative to FF_SYSTEM_BASE
          } ModuleMetaHeader_t;
          followed by ROM or MOD file contents

          File_02: starts at next 4K byte boundary, just for testing
          File_02: starts at next 256-byte boundary, final goal


    File types: 
    #define     FILETYPE_EMPTY  0x00      // indicates an erased file for empty space
    #define     FILETYPE_DELETED 0x00     // ROM file
    #define     FILETYPE_MOD1   0x01      // MOD1 file
    #define     FILETYPE_MOD2   0x02      // MOD2 file
    #define     FILETYPE_ROM    0x03      // ROM file
    #define     FILETYPE_QROM   0x04      // highest bit indicates this is writeable QROM, stored in FRAM
    #define     FILETYPE_UMEM   0x10      // for storing User Memory images
    #define     FILETYPE_MMAP   0x20      // for storing Module Map
    #define     FILETYPE_GLOB   0x30      // for storing Global Settings
    #define     FILETYPE_TRAC   0x40      // for storing Tracer triggers and settings
    #define     FILETYPE_4041   0x41      // initialization file
    #define     FILETYPE_FFFF   0xFF      // unused space, maiden flash

*/

// also adding the functions for OTP access
// OTP is used for storing the serial number only
// the serial numnber is stired at address 0x400 and 0x410
// the code follows the example in the pico-sdk
// the string programmed is "TULIP4041 HW V 1.0 serial #xxxx", 31 chars

const uint8_t  *flash_contents_bt  = (const  uint8_t *)(XIP_BASE + FF_OFFSET);    // pointer to FLASH byte array of FLASH File system
const uint16_t *flash_contents_wd  = (const uint16_t *)(XIP_BASE + FF_OFFSET);    // pointer to FLASH word array of FLASH File system
const uint8_t  *flash_contents_all = (const uint8_t *)(XIP_BASE);                    // pointer to FLASH byte array of the complete FLASH

static uint32_t ints;


// function to read the serial number from OTP
// the serial number is a 4 character string
bool otp_write_serial(char *serial_str)
{
  
  #define serial_length 32

  otp_cmd cmd;
  int8_t res;
  uint8_t buf[serial_length];             // buffer for string messages

  uint16_t ecc_row = 0x400;               // row to write ECC data
  uint16_t raw_row = 0x410;               // row to write raw data 

  cmd.flags = ecc_row;
  res = rom_func_otp_access(buf, serial_length, cmd);
  if (res) {
    cli_printf("  ERROR READING OTP: Initial ECC Row Read failed with error %d", res);
  }

  // check if range is empty, otherwise we cannot write
  for (int i = 0; i < serial_length; i++) {
    if (buf[i] != 0x00) {
      cli_printf("  ERROR: OTP ECC is not empty, cannot write serial number", raw_row);
      return false;  // cannot write to OTP
    }
  } 

  // copy the serial string to the buffer
  for (int i = 0; i < serial_length; i++) {
    if (serial_str[i] == '\0') {
      // end of string, fill the rest with 0x00
      buf[i] = 0x00;
    } else {
      buf[i] = serial_str[i];
    }
  }

  // now write the serial number to the ECC row
  cmd.flags = ecc_row | OTP_CMD_ECC_BITS | OTP_CMD_WRITE_BITS;
  res = rom_func_otp_access(buf, serial_length, cmd);
  if (res) {
    cli_printf("  ERROR: ECC Serial Number Write failed with error %d", res);
    return false;
  } else {
    cli_printf("  ECC Write Serial Number succeeded: %s", serial_str);
  }

  // Read it back
  cmd.flags = ecc_row | OTP_CMD_ECC_BITS;
  res = rom_func_otp_access(buf, serial_length, cmd);
  if (res) {
    cli_printf("  ERROR: ECC Read failed with error %d", res);
    return false;
  } else {
    cli_printf("  ECC Data read is \"%s\"", buf);
  }

  return true;
}

bool otp_read_serial(char *serial_str)
{
  otp_cmd cmd;
  int8_t res;
  uint8_t buf1[64];             // buffer for string from ECC row
  uint8_t buf2[64];             // buffer for string from RAW row

  uint16_t ecc_row = 0x400;    // row to read ECC data
  uint16_t raw_row = 0x410;    // row to read raw data 

  // first read the ECC row
  cmd.flags = ecc_row;
  res = rom_func_otp_access(buf1, sizeof(buf1), cmd);
  if (res) {
    cli_printf("  ERROR READING OTP: ECC Row Read failed with error %d", res);
    return false;
  }

  // copy the ECC data to the serial string
  // the ECC data bytes are grouped per 2 bytes with 1 byte ECC and 1 byte 0x00
  // copy the bytes and throw away every 3rd and 4th byte
  // since the serial number string is max 32 bytes long we can count until 32
  for (int i = 0; i < 31; i += 2) {
    buf2[i]     = buf1[i*2];
    buf2[i + 1] = buf1[i*2 + 1]; 
  } 

  #ifdef DEBUG
  // show the serial number in hex
  cli_printfn("  OTP ECC Row in hex   : ");
  for (int i = 0; i < sizeof(buf1); i++) {
    cli_printfn("%02X ", buf1[i]);
  }   
  cli_printf("");

  // show the compressed string in hex
  cli_printfn("  OTP ECC String in hex: ");
  for (int i = 0; i < sizeof(buf2); i++) {
    cli_printfn("%02X ", buf2[i]);
  } 
  cli_printf("");
  cli_printf("  OTP Serial Number is: %s", buf2);

  #endif

  // copy buf2 to the serial_str
  for (int i = 0; i < serial_length; i++) {
    serial_str[i] = buf2[i];
  }
  
  return true;   // success
}



// function to wait 0.5 seconds while flushing the console output
// use this before erasing or programming flash
void ff_delay500()
{
    int waitcount = 500;  // 0.5 seconds
    while(waitcount > 0) {
    sleep_ms(2);
    waitcount--;
  
    // flush the outpout buffer
    cdc_flush(ITF_CONSOLE);
    tud_task();  // must keep the USB port updated
  }
}

// erase all FLASH in the filesystem area
void ff_nuke()
{

  ff_delay500();  // wait for 0.5 seconds to flush the console output

  // to prevent issues with the tusb stack, erasing is done in chunks of 1 MByte

  uint32_t ff_end = FF_SYSTEM_SIZE;  // end of the file system
  uint32_t ff_start = FF_OFFSET;      // start of the file system
  int numblocks = FF_SYSTEM_SIZE / (1024 * 1024); // number of 1 MByte blocks to erase

  for (int i = 0; i < numblocks; i++) {
    // erase the FLASH in 1 MByte blocks

    cli_printf("  Erasing FLASH File System block %2d at 0x%08X", i, ff_start);

    ff_delay500();  // wait for 0.5 seconds to flush the console output

    // disable interrupts to prevent issues with the flash programming
    ints = save_and_disable_interrupts();

    // erase the FLASH block
    flash_range_erase(ff_start, (1024 * 1024)); // erase 1 MByte block

    // and restore interrupts
    restore_interrupts(ints);

    ff_start += (1024 * 1024); // next block to erase
  }
}

// check if the FLASH is fully erased
// num indicates granularity, 1 every byte is checked, 256 means every 256nd byte is checked
// checks the range starting with offs for size bytes
// returns the first non-erased address or 0xFFFFFFFF if all is erased
uint32_t ff_erased(uint32_t offs, uint32_t size, int num)
{
  uint32_t addr = offs;             // our address counter, 
                                    // start at the beginning of the file system
  uint32_t end = FF_SYSTEM_SIZE;    // end of the file system
  bool erased = true;               // assume FLASH is erased

  // check for correct range
  if ((offs + size) > end) return false;

  do {
    erased = (flash_contents_bt[addr] == 0xFF);   // check if byte is erased
    addr += num;                                  // next byte to check
  } while ((addr < (offs + size)) && erased);     // until end of file system or not erased

  if (erased) return NOTFOUND;                    // all is erased
  return (addr - num);                            // return the first non-erased address
}



void printbuf(const uint8_t *buf, int len)
{
  char  ShowPrint[250];
  int   ShowPrintLen = 0;
  int idx = 0;

  do {
    ShowPrintLen = 0;
    ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%04X  ", idx);
    for (int m = 0; m < 16; m++) {
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%02X ", buf[idx]);
      // check if content is filled
      idx++;
    }
    cli_printf("%s", ShowPrint);
  }
  while (idx < len);
}


// ff_init
// initialize the file system
// check if the file system is present and if not, create it
// first checks if the FLASH is fully erased
// Then creates the file system header with a first file of 256 bytes
void ff_init()
{
  // declare byte buffer for flash programming
  uint8_t buf[FLASH_PAGE_SIZE];           // 256 byte buffer for temporary storage

  char  ShowPrint[250];
  int   ShowPrintLen = 0;

  int idx = 0;

  ModuleMetaHeader_t *MetaH;              // Meta header for the first file

  // assign MetaH to point to buf
  MetaH = (ModuleMetaHeader_t*)buf;

  uint32_t addr = ff_erased(0, FF_SYSTEM_SIZE - 256, 1);

  // check if the FLASH is fully erased
  if (addr == NOTFOUND) {
    // FLASH is erased, now create the file system
    cli_printf("  FLASH is erased, creating file system");
  } else {
    // FLASH is not erased, do not overwrite
    cli_printf("  FLASH is not erased at address %08X, cannot create file system", addr);
    return;
  }
  
  // initialize the buffer with 0xFF
  memset(buf, 0xFF, FLASH_PAGE_SIZE);

  MetaH->FileType = FILETYPE_4041;
  strncpy(MetaH->FileName, "TULIP4041 FLASH HEADER", sizeof(MetaH->FileName) - 1);
  MetaH->FileName[sizeof(MetaH->FileName) - 1] = '\0';  // Ensure null termination
  MetaH->FileSize = 2;                                  // just 2 bytes
  MetaH->NextFile = 0x100;                              // point to next file immediate after this one
  idx = sizeof(ModuleMetaHeader_t);                     // set index to the end of the header
  buf[idx]   = 0x40;
  buf[idx+1] = 0x41;

  // now program the first file in FLASH and show it
  flash_range_program(FF_OFFSET, buf, FLASH_PAGE_SIZE);

  #ifdef DEBUG
    cli_printf("  FLASH File System initialized, first file created at %08X", FF_OFFSET);
    printbuf(flash_contents_bt, FLASH_PAGE_SIZE);
  #endif

}

bool ff_isinited()
{
  // check if the file system is initialized
  // this is done by checking the first file type
  ModuleMetaHeader_t *MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE); // point to the first file header

  if (MetaH->FileType == FILETYPE_4041) {
    // file system is initialized
    return true;
  } else {
    // file system is not initialized
    return false;
  }
}

// function to show FLASH contents in the CLI with 16 bytes per line 
// in blocks of 4K bytes
// skip erased contents
void ff_show(uint32_t addr)
{
  char  ShowPrint[250];
  int   ShowPrintLen = 0;
  int i = 0;                                // counter for the number of bytes in a line
  char c;
  uint32_t endaddr = addr + 0x1000;         // end of the dump
  if (endaddr > FF_SYSTEM_SIZE) {
    endaddr = FF_SYSTEM_SIZE;               // do not exceed the end of the file system
  }

  do {
    ShowPrintLen = 0;

    // print the address
    ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%08X  ", addr);

    // print 16 bytes
    for (int m = 0; m < 16; m++) {
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%02X ", flash_contents_bt[addr]);
      // check if content is erased
      // line_erased = line_erased && (flash_contents_bt[addr] == 0xFF);
      addr++;
    }
    addr = addr - 16;
    // print byte values as characters
    ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "  ");
    for (int m = 0; m < 16; m++) {
      c = flash_contents_bt[addr];
      if ((c < 0x20) || (c > 0x7E)) {
        c = '.';
      }
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%c", c);
      addr++;
    } 
    
    // flush the USB buffer
    tud_task();

    cli_printf("%s", ShowPrint);

  } while (addr < endaddr);   // list 4K bytes by default

}

// ff_findfree
// finds the smallest free entry in the FLASH File System matching the given file size
// size is the filesize WITHOUT the header size	
// starts at the given FLASH offset, which must be a valid file start, typically 0 for the first call
// returns the offset in FLASH of this free file slot, this may be an empty or deleted entry
// this may also the offset to the end of the file system if no free space is found
// offset is always from the start of the FF_SYSTEM_BASE 
// returns 0 when no free space is found
uint32_t ff_findfree(uint32_t offs, uint32_t size)
{
  ModuleMetaHeader_t *MetaH;                    // pointer to meta header for getting info of file
  ModuleMetaHeader_t *MetaH_next;               // pointer to meta header for getting info of the next file

  // find the first free slot in FLASH
  uint32_t ff_end = FF_SYSTEM_SIZE;             // end of the file system
  uint32_t next;                                // offset to the next file
  uint32_t candidate = 0;                       // candidate for the file slot
  uint32_t cand_size = 0;                       // size of the candidate file slot
  uint32_t current = 0;                         // current file slot under investigation
  uint32_t space = 0;                           // size of the free space
  uint32_t space_acc = 0;                       // accumulated size of the free space


  size = size + sizeof(ModuleMetaHeader_t);     // required size of file plus header

  #ifdef DEBUG
    cli_printf("  searching for free space in FLASH, size %d / %d bytes", size, size - sizeof(ModuleMetaHeader_t));
  #endif

  while (offs < ff_end) {
    MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);    // map header to struct
    uint8_t filetype = MetaH->FileType;                      // get the file type
    tud_task();  // keep the USB port updated

    switch(filetype) {
      case FILETYPE_FFFF:                                   // end of the file system
        #ifdef DEBUG
          cli_printf("  end of file system reached at %08X", offs);
        #endif
        // we have reached the end of the File System
        // check the remaining space

        if ((ff_end - offs) > size) {
          // there is enough size left
          // is this space is smaller than the current candidate then this is better
          // and we can return the offset
          if ((ff_end - offs) < cand_size) {
            return offs;
          }
          // if the new slot is larger than the candidate then we use the candidate
          // but we have to check if the candidate is valid as it could be 0
          if (candidate == 0) {
            // no candidate found yet, so this is the first one
            return offs;
          }
        } else {
          // not enough space left, this means that our candidate, if any, is now valid
          // if the candidate was 0, then bad luck, no space available
          return NOTFOUND;         // no free space found
        }   

        return candidate;
        break;                      // we should never get here

      case FILETYPE_DELETED:                                   
      case FILETYPE_DUMMY:                  
        // found an empty file or dummy, check if it is large enough
        // get the offset to the next file to determine the size
        // no check for subsequent dummy or deleted files, maybe add this later
        next = MetaH->NextFile;             // pointer to the next file to get the available space
        space = next - offs;                // available space in this file

        #ifdef DEBUG
          cli_printf("  deleted space at %08X of %d bytes", offs, space);
        #endif

        // if the file fits we report this space, if not we check if there is free space after this file
        if (space >= size) {
          // there is enough space in this file
          // check if this is smaller than the current candidate then this is better
          // and we can return the offset and continue the search
          // if the cadidate was still 0 this is a possible candidate
          if (cand_size == 0) {
            // no candidate found yet, so this is the first one
            candidate = offs;              // remember the current file offset as a candidate
            cand_size = space;             // remember the size of the candidate
            #ifdef DEBUG
              cli_printf("  first candidate found at %08X of %d bytes", offs, space);
            #endif
          } else if (space < cand_size) {
            // this slot is smaller then the previous candidate and it fits
            // this is now a better candidate
            #ifdef DEBUG
              cli_printf("  better candidate at %08X of %d bytes", offs, space);
            #endif
            candidate = offs;              // remember the current file offset as a candidate
            cand_size = space;             // remember the size of the candidate
          }
        } 
        offs = next;
        
        // the next part is skipped for now
        // this is checking if 2 consecutive files are ok
        /* 
        else {
          // if this slot is smaller then we can check if there is free space after this file
          // get the filetype of the next file
          MetaH_next = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + next);    // map header to struct

          // if this slot is smaller then we can check if there is free space after this fil      
          if ((MetaH_next->FileType != FILETYPE_FFFF) || (MetaH_next->FileType != FILETYPE_DELETED) || 
            (MetaH_next->FileType != FILETYPE_DUMMY)) {
            // there is a file which is in use, so the space cannot be used
            // we keep the current candidate
            offs = next;                  // go to the next file
          } else {
            // there is erased or free space after this
            // for now we skip this, implement later
            offs = next;                // go to the next file
          }
        }
          */
        break;
      default:
        // all other filetypes can be skipped, this is not potential free space
        // move to the next file
        #ifdef DEBUG
          // cli_printf("  skipping file at %08X of type %02X, next is %08X", offs, filetype, MetaH->NextFile);
        #endif
        offs = MetaH->NextFile;                                     // go to the next file
        break;
    }
  }

  // if we get here then we have reached the end of the file system
  // and most likely no free space is found and candidate is still 0
  return candidate;                               

}


// ff_lastfree
// finds the last free entry in the list of MOD/ROM files in FLASH
// starts at the given FLASH offset, which must be a valid file start, typically 0 for the first call
// returns the offset in FLASH of the first free erased entry, this will usually be after the last file
// use this call when searching for a free slot to prevent wear on the FLASH, but may increase fragmentation
// offset is always from the start of the FF_SYSTEM_BASE, default at 0x00080000 + XIP_BASE 
// returns 0xFFFFFFFF when no free space is found
uint32_t ff_lastfree(uint32_t offs)
{
  ModuleMetaHeader_t *MetaH;          // pointer to meta header for getting info of next file

  // first find the last available slot in FLASH
  uint32_t end = FF_SYSTEM_SIZE;
  while (offs < end) {
    #ifdef DEBUG
      // cli_printf("  checking file at 0x%08X end at: 0x%08X", offs, end);
    #endif
    MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct
    if (MetaH->FileType == FILETYPE_FFFF) break;                // if it erased then it is useable
    // check if this is a valid file to prevent looping
    if ((MetaH->FileType > FILETYPE_4041)) {
      // unsupported file type, this is not a valid file
      return NOTFOUND;                                          // no free space found
    }
    offs = MetaH->NextFile;                                     // go to the next file
  }
  if (offs > end) return NOTFOUND;                              // no free space found
  return offs;                                                  // return the offset of the last free slot
}


// ff_findnext
// returns a pointer to the next file in the chain of MOD/ROM files in FLASH
// offs must point to a valid file start, typically 0 for the first call
uint32_t ff_findnextf(uint32_t offs)
{
  ModuleMetaHeader_t *MetaH;          // pointer to meta header for getting info of next file
  MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct
  return MetaH->NextFile;                                     // return a pointer to the next file
}


// ff_findfile
// search for a file by name and return the pointer
// returns the offset in FLASH of the file with the given name
// returns 0xFFFFFFFF when the file is not found
// will also find a deleted file!
uint32_t ff_findfile(const char *name)
{
  ModuleMetaHeader_t *MetaH;          // pointer to meta header for getting info of next file
  uint32_t offs = 0;                  // start of the file system
  uint32_t end = FF_SYSTEM_SIZE;      // end of the file system

  // check if a file exists, do a case insensitive compare
  while (offs < end) {
    MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct
    if (MetaH->FileType == FILETYPE_FFFF) return NOTFOUND;      // end of chain reached
    // if the file is marked for erase then we must skip it
    if (strcasecmp(MetaH->FileName, name) == 0 && (MetaH->FileType != FILETYPE_DELETED)) {
      return offs;    // found the file
    }
    offs = MetaH->NextFile;                                     // go to the next file
  }

  return NOTFOUND;                                              // file not found
}


// Check if a single byte can be written to a location in FLASH.
// A byte is writable if no 0 bits need to be changed to 1 bits.
bool ff_writableb(uint32_t offs, uint8_t data)
{
  // A FLASH byte is writable if:
  // - Only 1 bits are flipped to 0 bits.
  // - 1 bits and 0 bits remain unchanged.
  // This is checked using bitwise AND followed by XOR. If the result is zero, writing is OK.
  uint8_t flash_val = flash_contents_bt[offs] & data;    // this is the AND
  flash_val ^= data;                                     // XOR to check if any bits need to change from 0 to 1, which is not allowed in FLASH
  return (flash_val == 0);
}

bool ff_writeableb(uint8_t flash_byte, uint8_t data)
{
  // A FLASH byte is writable if:
  // - Only 1 bits are flipped to 0 bits.
  // - 1 bits and 0 bits remain unchanged.
  // This is checked using bitwise AND followed by XOR. If the result is zero, writing is OK.
  uint8_t flash_val = flash_byte & data;    // this is the AND
  flash_val ^= data;                        // XOR to check if any bits need to change from 0 to 1, which is not allowed in FLASH
  return (flash_val == 0);
}


// check if a range in FLASH is writeable by the buf contents
// offset must be 256 byte aligned, number of bytes must be a multiple of 256, this is not checked!
bool ff_writable(uint32_t offs, uint8_t *buf, int num)
{
    bool writeable = true;
    uint8_t flash_val;
    int i = 0;
    do {
      // to check if a FLASH byte is writeable check if only 1 bits are flipped to 0
      // or 1 bits and 0 bits are unchanged
      // done by bitwise AND then XOR. If result is zero writing is OK
      flash_val = flash_contents_bt[offs] & buf[i];
      flash_val ^= buf[i]; 
      i++;
    } while ((i < num) || (flash_val == 0));

    return (flash_val == 0);
}

// compare a part of flash with a buffer
// returns 0 if the contents are identical
//         1 if the contents are different and the flash can be reprogrammed without erasing
//         2 if the contents are different and the flash must be erased before reprogramming
#define flash_identical 0
#define flash_reprogram 1
#define flash_erase     2
int ff_compare(uint32_t offs, uint8_t *buf, int num)
{
    int i = 0;
    int result = 0;
    int result_acc = 0;      // accumulated result
    int result_erase = 0;
    uint8_t flash_val;

    do {
      result = (flash_contents_bt[offs] == buf[i]);
      if (!result) {
        result_acc = 1;  // different
        flash_val = flash_contents_bt[offs] & buf[i];
        flash_val ^= buf[i];
        result_erase = (flash_val == 0);
        if (!result_erase) return flash_erase;  // different and must be erased
      }
      i++;

    } while (i < num);
    return result_acc;
}

// flashpointer 
// returns a pointer to the FLASH address
// offs is the offset in the FLASH memory, NOT in the file system!
const inline uint8_t *flashPointer(uint32_t offs)
{
  return (const uint8_t *)(XIP_BASE + offs);
}



// write a single byte to FLASH without erasing, can only change a 1 to a 0
// writes a single byte data to the address offs. This is the offset in the flash file system!
bool ff_write(uint32_t offs, uint8_t data)
{
    uint8_t buf[FLASH_PAGE_SIZE];                               // 256 byte buffer for temporary storage

    memcpy(buf, &flash_contents_bt[offs & FLASH_PAGE_OFFS], FLASH_PAGE_SIZE);     // copy block from FLASH

    uint8_t bt = buf[offs & FLASH_PAGE_MASK];                         // original byte we want to change
    buf[offs & FLASH_PAGE_MASK] = data;                               // change the byte in the buffer

    ff_delay500();  // wait for 0.5 seconds to flush the console output
    // disable interrupts to prevent issues with the flash programming
    ints = save_and_disable_interrupts();
    flash_range_program(FF_OFFSET + (offs & FLASH_PAGE_OFFS), buf, FLASH_PAGE_SIZE ); 

    // and restore interrupts
    restore_interrupts(ints);

    #ifdef DEBUG
      cli_printf("  write %02X to %08X, original was %02X", data, FF_OFFSET + (offs & FLASH_PAGE_OFFS), bt);
      cli_printf("  new value is %02X", flash_contents_bt[offs]);
    #endif

    // check if programming was succesful
    return (data == flash_contents_bt[offs]);
}

// writes num bytes in buf to the FLASH address starting at offs
// the range must be writeable, either all FF's or only 1's flipping to 0's
// range will be verified after writing
// offset must be 256 byte aligned, number of bytes must be a multiple of 256, this is not checked!
bool ff_write_range(uint32_t offs, uint8_t *buf, int num)
{

    ff_delay500();  // wait for 0.5 seconds to flush the console output

    // disable interrupts to prevent issues with the flash programming
    ints = save_and_disable_interrupts();

    flash_range_program(FF_OFFSET + offs, buf, num);   // program the range

    // and restore interrupts
    restore_interrupts(ints);

    // now check of programming was succesful
    bool succes = true;
    int i = 0;
    do {
      succes = (buf[i] == flash_contents_bt[offs + i]);
      i++;
    } while ((i < num) || succes);

    return succes;
}


// erase an arbitrary block of FLASH from fl_start to fl_end
// fl_start and fl_end is always the offset in the Flash File System!
// fl_start and fl_end must be 256-byte aligned
// original contents outside the range but inside a 4K block will be preserved (programmed back)
// uses 8KByte for 2* save buffer
void ff_erase(uint32_t fl_start, uint32_t fl_end)
{
    uint8_t buf1[FLASH_SECTOR_SIZE];         // 4K buffer for temporary storage
    uint8_t buf2[FLASH_SECTOR_SIZE];         // 4K buffer for temporary storage
    uint32_t savebytes1, savebytes2;        // number of bytes to save

    /*
          +-------------------+   firstblock: first 4K boundary in range to erase
          +  data to keep 1   +
          +-------------------+   fl_start: start of area to erase (on 256-byte boundary)
          +
                .
                .                 multiple 4K blocks in between 
                .
          +-------------------+   lastblock: last 4K boundary in range to erase
          +                   +
          +-------------------+   fl_end:   end of area to erase   (on 256-byte boundary)
          + data to keep 2    +
          +-------------------+   endoflastblock: last 4K boundary in range to erase

    
    */

    // align fl_start and fl_end to flash address instead of offset in the file system
    fl_start = (fl_start & FLASH_PAGE_OFFS) + FF_OFFSET;      // align to 256-bit FLASH address
    fl_end   = (fl_end & FLASH_PAGE_OFFS) + FF_OFFSET;        // align to 256-bit FLASH address

    // first determine addresses needed
    uint32_t firstblock = fl_start & FLASH_SECTOR_OFFS;       // get start address of first block and align to FLASH sector
    uint32_t lastblock  = fl_end & FLASH_SECTOR_OFFS;         // get start address of last block and align to FLASH sector
    uint32_t endoflastblock = lastblock + FLASH_SECTOR_SIZE;  // get last address (+1) of last block

    savebytes1 = fl_start - firstblock;                       // bytes to save from first block
    savebytes2 = endoflastblock - fl_end;                     // bytes to save from last block

    void *block1_flash = (void *)(XIP_BASE + firstblock);     // hard pointer to first block in FLASH
    void *block2_flash = (void *)(XIP_BASE + fl_end);         // hard pointer to last block in FLASH

    uint32_t erase_bytes = fl_end - fl_start;             	  // number of bytes to erase


    #ifdef DEBUG
      cli_printf("  Saving %d bytes from %08X", savebytes1, firstblock);
      cli_printf("  Saving %d bytes from %08X", savebytes2, fl_end);
      cli_printf("  Erasing %d bytes from %08X to %08X", erase_bytes, fl_start, fl_end);
      cli_printf("  Erase command: start at %08X,  %d bytes", firstblock, endoflastblock - firstblock);
    #endif


    // example: file simplex.rom starting at 0x002D3E00, end at 0x002D5F00
    // must add the offset of the File System start 0x00100000
    // firstblock = 0x003D3E00 & 0xFFFFF000 = 0x003D3E00 (align at 256-bit address)
    // lastblock  = 0x003D5F00 & 0xFFFFF000 = 0x003D5F00 (align at 256-bit address)
    // endoflastblock = 0x002D5F00 + FLASH_SECTOR SIZE = 0x002D6000 (align at 256-bit address)
    //                  this is the start of the area to save
    //
    // start of first sector to erase 0x003D3E00 & 0xfffff000 = 0x003D3000
    // start of last sector to erase  0x003D5F00 & 0xfffff000 = 0x003D5000
    // end of last sector to erase    0x003D5000 + 0x00001000 = 0x003D6000 (not including this byte address)
    // so we erase from 0x003D3000 to 0x003D6000, which is 0x00003000 bytes or 12K bytes
    // 
    // we must save the first part of the sector from 0x003D3000 to 0x003D3E00, which is 0x00000E00 bytes or 3584 bytes
    // and the last part of the sector from 0x003D5F00 to 0x003D6000, which is 0x00000100 bytes or 256 bytes
    //
    // copy the first and last block to the buffers
    memset(buf1, 0xFF, FLASH_SECTOR_SIZE);  // initialize the buffers with 0xFF
    memset(buf2, 0xFF, FLASH_SECTOR_SIZE);  // initialize the buffers with 0xFF

    // copy the first and last block to the buffers
    memcpy(buf1, (void *)flashPointer(firstblock), savebytes1);  // copy first block to buffer
    memcpy(buf2, (void *)flashPointer(fl_end), savebytes2);      // copy last block to buffer


    ff_delay500();  // wait for 0.5 seconds to flush the console output

    // disable interrupts to prevent issues with the flash programming
    ints = save_and_disable_interrupts();

    // sort out number of blocks to erase and do the erase
    flash_range_erase(firstblock, endoflastblock - firstblock);

    // restore the original data
    flash_range_program(firstblock, buf1, savebytes1);      // first block that was saved
    flash_range_program(fl_end, buf2, savebytes2);          // last block that was saved

    // and restore interrupts
    restore_interrupts(ints);
}


// erase a 4K block anywhere in FLASH
void ff_erase_block(uint32_t address) {

    // align address to 4K boundary
    address &= FLASH_SECTOR_OFFS;
    
    ff_delay500();  // wait for 0.5 seconds to flush the console output

    // disable interrupts to prevent issues with the flash programming
    ints = save_and_disable_interrupts();
    flash_range_erase(address, FLASH_SECTOR_SIZE);
    
    restore_interrupts(ints);

}

// program a block anywhere in FLASH
void ff_program_block(uint32_t address, const uint8_t *data, size_t size) {
    // align address to 256 byte boundary
    address &= FLASH_PAGE_OFFS;

    ff_delay500();  // wait for 0.5 seconds to flush the console output

    // disable interrupts to prevent issues with the flash programming
    ints = save_and_disable_interrupts();
    flash_range_program(address, data, size);
    restore_interrupts(ints);
}


uint32_t ff_erased_block(uint32_t offs, uint32_t size, int num) {
// check if the FLASH is fully erased
// offs is here an address in the complete FLASH!
// num indicates granularity, 1 every byte is checked, 256 means every 256nd byte is checked
// checks the range starting with offs for size bytes
// returns the first non-erased address or 0xFFFFFFFF if all is erased

  uint32_t addr = offs;             // our address counter, 
                                    // start at the beginning of the file system
  uint32_t end = PICO_FLASH_SIZE_BYTES;    // end of the file system
  bool erased = true;               // assume FLASH is erased

  // check for correct range
  if ((offs + size) > end) return false;

  do {
    erased = (flash_contents_all[addr] == 0xFF);   // check if byte is erased
    addr += num;                                  // next byte to check
  } while ((addr < (offs + size)) && erased);     // until end of file system or not erased

  if (erased) return NOTFOUND;                    // all is erased
  return (addr - num);                            // return the first non-erased address
}


// to erase all flash in the filesystem
// no checks, use with care!
void ff_erase_all() {

  // function not used

}