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

//  ff_import       - program a file from uSD to FLASH/FRAM
//  ff_export       - save a file from FLASH/FRAM to uSD
//  ff_cat          - list files in FLASH/FRAM
//                    optional with details
//  ff_free         - get remaining free space
//  ff_delete       - delete a file from FLASH/FRAM
//  ff_findfile     - search for a file by name and return the pointer
//                    used for plug/unplug/info
//  ff_findfile_n   - find file by index number and return the pointer
//                    used for CAT etc

//  ff_findfree     - find next free/erased slot and report size
//                    search starting a a given offset
//  ff_lastfree     - find last free/erased slot
//                  - use this for adding new images to prevent wear on the flash
//  ff_listfree     - list all free slots with size
//  ff_findnext     - 
//
//  Helper functions
//  ff_writeable    - check if a single byte can be written in FLASH
//  ff_writeable_range - check if a range can be programmed
//  ff_write        - write a single byte to FLASH
//                    use a local 256-byte buffer to read, change and then write
//  ff_write_range  - write a range of bytes to FLASH
//                    target range must be erased or only 1 bits flipping to 0
//                    after writing range will be verified for correct programming
//  ff_erase        - erase an arbitrary block of FLASH (256-byte boundaries)
//                    to prepare for writing a block
//  ff_erase_all    - erase all FLASH/FRAM
//  ff_erased       - check if a range is fully erased
//  ff_program      - erase and re-program an arbitrary range of FLASH
//                    based on 256-byte boundaries




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

          File_02:      starts at next 256-byte boundary

    File types: 
    #define     FILETYPE_EMPTY  0x00      // indicates an erased file for empty space
    #define     FILETYPE_MOD1   0x01      // MOD1 file
    #define     FILETYPE_MOD2   0x02      // MOD2 file
    #define     FILETYPE_ROM    0x03      // ROM file
    #define     FILETYPE_QROM   0x04      // highest bit indicates this is writeable QROM, stored in FRAM
    #define     FILETYPE_UMEM   0x10      // for storing User Memory images
    #define     FILETYPE_MMAP   0x20      // for storing Module Map
    #define     FILETYPE_GLOB   0x30      // for storing Global Settings
    #define     FILETYPE_TRAC   0x40      // for storing Tracer triggers and settings
    #define     FILETYPE_FFFF   0xFF      // unused space, maiden flash

*/





// ff_findfree
// finds the next free entry in the list of MOD/ROM files in FLASH with the minimum required size
// starts at the given FLASH offset, which must be a valid file start, typically 0 for the first call
// returns the offset in FLASH of the first free file slot, this may be an empty or deleted entry
// offset is always from the start of the FF_SYSTEM_BASE, default at 0x00080000 + XIP_BASE 
// returns 0xFFFFFFFF when no free space is found
uint32_t ff_findfree(uint32_t offs, uint32_t size)
{
  ModuleMetaHeader_t *MetaH;                    // pointer to meta header for getting info of next file
  size = size + sizeof(ModuleMetaHeader_t);     // required sizen of file plus header

  do {
    MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);                         // map header to struct
    if ((MetaH->FileType == FILETYPE_FFFF)) {
      // file space found, now check if it is enough
      // there is probably no next file but check anyway
      if (MetaH->NextFile == 0xFFFFFFFF) {
        // is indeed erased FLASH, now check how much space there is
        if (size < (FF_SYSTEM_SIZE - offs)) {
          return offs;              // there is enough space
        }
      } else return NOTFOUND;     // not enough space left
    }

    if (MetaH->FileType == FILETYPE_EMPTY) {
      // this was space of an erased file
      // find out how much space there is
      // we do this by looping through the next files until we find an occupied slot
      // or the end of the chain

      
      

      
      return offs;
    }

    // we get here if the space was not free
    offs = MetaH->NextFile;             // go to the next file
  } while (offs < FF_SYSTEM_SIZE);
  return NOTFOUND;
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
  
  do {
    MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);                         // map header to struct
    if (MetaH->FileType == FILETYPE_FFFF) return offs;

    // we get here if the space was not free
    offs = MetaH->NextFile;             // go to the next file
  } while (offs < FF_SYSTEM_SIZE);
  return NOTFOUND;
}


// ff_import
// program a files from microSD to FLASH
// parameters passed:
//    filepointer to file object (prepared by user interface)
//    pointer to module meta header structure
bool ff_import(FIL *fil, ModuleMetaHeader_t *MetaH)
{


  // first find necessary free space in FLASH and check if possible to FLASH
  // erase section if needed

  // breakdown in chunks of 4K
  // calculate number of 4K chunks
  // calculate remainder in 4K chunk
  // work out data to save already in FLASH and buffer last chunk
  // do the programming
  // verify programming and exit



  return false;


}



// returns a valid full address to read from FLASH
inline const uint8_t *flashPointer(uint32_t offs)
{
  return (const uint8_t*)(XIP_BASE + offs);
}



// check if a single byte can be written to a location in FLASH
// if there are no 0'to be programmed as 1's
bool ff_writeable(uint32_t offs, uint8_t data)
{
  // to check if a FLASH byte is writeable check if only 1 bits are flipped to 0
  // or 1 bits and 0 bits are unchanged
  // done by bitwise AND then XOR. If result is zero writing is OK
  uint8_t flash_val = flash_contents_bt[offs] & data;    // this is the AND
  flash_val ^= data;                               // XOR
  return flash_val == 0;
}

// check if a range in FLASH is writeable by the buf contents
// offset must be 256 byte aligned, number of bytes must be a multiple of 256, this is not checked!
bool ff_writeable_range(uint32_t offs, uint8_t *buf, int num)
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


// write a single byte to FLASH without erasing, can only change a 1 to a 0
// writes a single byte data to the address flash_offs. This is the offset in flash!
bool ff_write(uint32_t offs, uint8_t data)
{
    uint8_t buf[FLASH_PAGE_SIZE];                               // 256 byte buffer for temporary storage

    const uint8_t *fp = flashPointer(offs & BLOCK_MASK);        // pointer to FLASH PAGE
    memcpy(buf, (void *)&flash_contents_bt[offs & BLOCK_MASK], FLASH_PAGE_SIZE);                           // copy block from FLASH

    uint8_t bt = buf[offs & 0xFF];                              // original byte we want to change
    flash_range_program(offs & BLOCK_MASK, buf, FLASH_PAGE_SIZE ); 

    // check if programming was succesful
    return (data == flash_contents_bt[offs]);

    // to check if it is possible to write: first XOR and then AND, see ff_writeable

}

// writes num bytes in buf to the FLASH address starting at offs
// the range must be writeable, either all FF's or only 1's flipping to 0's
// range will be verified after writing
// offset must be 256 byte aligned, number of bytes must be a multiple of 256, this is not checked!
bool ff_write_range(uint32_t offs, uint8_t *buf, int num)
{
    const uint8_t *fp = flashPointer(offs & BLOCK_MASK);            // pointer to FLASH PAGE
    flash_range_program(offs &BLOCK_MASK, buf, FLASH_PAGE_SIZE );   // program the range

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
// fl_start and fl_end is always the offset in the flash memory!
// original contents outside the range butinside a 4K block will be preserved (programmed back)
// uses 8KByte for 2* save buffer
void ff_erase(uint32_t fl_start, uint32_t fl_end)
{
    uint8_t buf1[FLASH_BLOCK_SIZE];         // 4K buffer for temporary storage
    uint8_t buf2[FLASH_BLOCK_SIZE];         // 4K buffer for temporary storage
    uint32_t savebytes1, savebytes2;        // number of bytes to save

    bool on_4K = false;


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

    // first determine number of blocks to erase
    uint32_t erase_bytes = fl_end - fl_start;               // number of bytes to erase, always a multiple of 256
    uint32_t firstblock = fl_start & BLOCK_MASK;            // get start address of first block
    uint32_t lastblock  = fl_end & BLOCK_MASK;              // get start address of last block
    uint32_t endoflastblock = lastblock + FLASH_BLOCK_SIZE; // get last address of last block
    savebytes1 = fl_start - firstblock;                     // bytes to save from first block
    savebytes2 = endoflastblock - fl_end;                   // bytes to save from last block

    memcpy(&firstblock + XIP_BASE, &buf1, savebytes1);      // save bytes to save from the first block
    memcpy(&fl_end + XIP_BASE, &buf2, savebytes2);          // save bytes to save from the last block

    // now sort out number of blocks to erase and do the erase
    flash_range_erase(firstblock, (lastblock - firstblock + 1) * FLASH_BLOCK_SIZE);

    // and now restore the original data
    flash_range_program(firstblock, buf1, savebytes1);      // first block that was saved
    flash_range_program(fl_end, buf2, savebytes2);          // last block that was saved
}


// to erase all flash in the filesystem
// no checks, use with care!
void ff_erase_all() {


}