/*
 * fram.c
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


#include "fram.h"

//
// FRAM read/write routines
// 

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
} 


// initialize the SPI port
void init_spi_fram()
{
    spi_init(SPI_PORT_FRAM, SPI_FRAM_SPEED);
    gpio_set_function(PIN_SPI0_RX, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI0_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI0_TX, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_SPI0_CS);
    gpio_put(PIN_SPI0_CS, 1);
    gpio_set_dir(PIN_SPI0_CS, GPIO_OUT);  

    // SPI modes supported by the device: mode 0 (0, 0) and mode 3 (1,1)
}




// read from FRAM device, with fixed spi channel and cs pin
void fr_read(uint32_t addr, uint8_t *buf, size_t len)
{
    // read bytes from the FRAM device
    // addr is the address in the FRAM device, buf is the buffer to read into, len is the number of bytes to read
    fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, addr, buf, len);
}

uint8_t fr_read8(uint32_t addr)
{
    // read a single byte from the FRAM device at address addr
    uint8_t result = 0;
    fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, addr, (uint8_t*)&result, 2);
    return result;
}

uint32_t fr_read32(uint32_t addr)
{
    // read 32-bit words from the FRAM device
    // addr is the address in the FRAM device, buf is the buffer to read into, len is the number of words to read
    uint32_t result = 0;
    fram_read32(SPI_PORT_FRAM, PIN_SPI0_CS, addr, &result, 4);
}

uint16_t fr_read16(uint32_t addr)
{
    // read 16-bit words from the FRAM device at address addr
    uint16_t result = 0;
    fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, addr, (uint8_t*)&result, 2);
    return result;
}


void fr_write(uint32_t addr, uint8_t *buf, size_t len)
{
    // write bytes to the FRAM device
    // addr is the address in the FRAM device, data is the buffer to write from, len is the number of bytes to write
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, addr, buf, len);
}

void fr_write8(uint32_t addr, uint8_t bt)
{
    // write a single byte to the FRAM device
    // addr is the address in the FRAM device
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, addr, (uint8_t*)&bt, 1);
}
void fr_write16(uint32_t addr, uint16_t word)
{
    // write 16-bit words to the FRAM device
    // addr is the address in the FRAM device, data is the buffer to write from, len is the number of words to write
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, addr, (uint8_t*)&word, 2);
}

void fr_write32(uint32_t addr, uint32_t word)
{
    // write 32-bit words to the FRAM device
    // addr is the address in the FRAM device, data is the buffer to write from, len is the number of words to write
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, addr, (uint8_t*)&word, 4);
}


void __not_in_flash_func(fram_read)(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t *buf, size_t len) 
// base function to read bytes form the fram device
{
    cs_select(cs_pin);
    uint8_t cmdbuf[4] = {
            FRAM_READ_CMD,
            addr >> 16,
            addr >> 8,
            addr
    };
    spi_write_blocking(spi, cmdbuf, 4);                       // write READ command
    spi_read_blocking(spi, 0, (uint8_t*)buf, len);            // read result
    cs_deselect(cs_pin);
} 

void __not_in_flash_func(fram_read32)(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint32_t *buf, size_t len) 
// base function to read bytes from the fram device, for 32-bit wide words
{
    cs_select(cs_pin);
    uint8_t cmdbuf[4] = {
            FRAM_READ_CMD,
            addr >> 16,
            addr >> 8,
            addr
    };
    spi_write_blocking(spi, cmdbuf, 4);                       // write READ command
    spi_read_blocking(spi, 0, (uint8_t*)buf, len);            // read result
    cs_deselect(cs_pin);
} 

void __not_in_flash_func(fram_read16)(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint16_t *buf, size_t len) 
// base function to read bytes from the fram device, for 32-bit wide words
{
    cs_select(cs_pin);
    uint8_t cmdbuf[4] = {
            FRAM_READ_CMD,
            addr >> 16,
            addr >> 8,
            addr
    };
    spi_write_blocking(spi, cmdbuf, 4);                     // write READ command
    spi_read_blocking(spi, 0, (uint8_t*)buf, len);          // read result
    cs_deselect(cs_pin);
} 

// read FRAM device ID
// this is always 9 bytes!
void __not_in_flash_func(fram_read_id)(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t *buf, size_t len) 
// base function to read bytes from the fram device
{
    cs_select(cs_pin);
    uint8_t cmdbuf[1] = {FRAM_RDID_CMD}; // command to read device ID};
    spi_write_blocking(spi, cmdbuf, 1);                       // write READ command
    spi_read_blocking(spi, 0, (uint8_t*)buf, len);            // read result
    cs_deselect(cs_pin);
} 

// === write control ===
void __not_in_flash_func(fram_write_enable)(spi_inst_t *spi, uint cs_pin) {
    CS_on ; 
    uint8_t cmd = FRAM_WREN_CMD ;
    spi_write_blocking(spi, &cmd, 1);
    CS_off ; 
}

void __not_in_flash_func(fram_write_disable)(spi_inst_t *spi, uint cs_pin) {
    CS_on ; 
    uint8_t cmd = FRAM_WREN_CMD ;
    spi_write_blocking(spi, &cmd, 1);
    CS_off ; 
}

// === write commands ===
void __not_in_flash_func(fram_write)(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t * data, size_t len) {
    uint8_t cmdbuf[4] = {
            FRAM_WRITE_CMD,
            addr >> 16,
            addr >> 8,
            addr
    };
    fram_write_enable(spi, cs_pin) ;
    CS_on ;
    spi_write_blocking(spi, cmdbuf, 4) ;
    spi_write_blocking(spi, data, len) ;
    CS_off ;
    fram_write_disable(spi, cs_pin) ;
}


// function to show FRAM contents in the CLI with 16 bytes per line 
// in blocks of 4K bytes
// FRAM is read in chunks of 16 bytes for one line
void fram_show(uint32_t addr)
{
  char  ShowPrint[250];
  int   ShowPrintLen = 0;
  int i = 0;                                // counter for the number of bytes in a line
  char c;
  uint8_t fram_line[16];                    // buffer for one line of 16 bytes
  uint32_t endaddr = addr + 0x1000;         // end of the dump

  do {
    // build the line
    ShowPrintLen = 0;

    // print the address
    ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%05X  ", addr);

    // Read 16 bytes from FRAM
    fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, addr, (uint8_t*)fram_line, 16);

    // print 16 bytes
    for (int m = 0; m < 16; m++) {
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%02X ", fram_line[m]);
    }

    // print byte values as characters
    ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "  ");
    for (int m = 0; m < 16; m++) {
      c = fram_line[m];
      if ((c < 0x20) || (c > 0x7E)) {
        c = '.';
      }
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%c", c);
      
    } 
    addr += 16;  // increment address by 16 bytes
    cli_printf("%s", ShowPrint);

  } while (addr < endaddr);   // list 4K bytes by default

}

void fr_readid(uint8_t *buf)
{
    // read the device ID from the FRAM device
    // buf is the buffer to read into, len is the number of bytes to read
    fram_read_id(SPI_PORT_FRAM, PIN_SPI0_CS, 0, buf, 4); // read 4 bytes of ID
}


// erase all FRAM to zero
void fr_nukeall()
{
    // erase all FRAM to zerofram dump 1000
    uint8_t buf[256] = {0};  // buffer to write 256 bytes at a time
    for (uint32_t addr = 0; addr < FRAM_SIZE; addr += 256) {
      fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, addr, buf, 256);
        // fr_write(addr, buf, sizeof(buf));  // write 256 bytes of zero
    }
    cli_printf("  All FRAM erased to zero");
}


// and adding all functions like the ffmanager now for FRAM
// this is a copy of the ffmanager functions, adapted for FRAM


// write num bytes in buf to the FRAM address starting at offs
// and returns true if the write was successful
bool fr_write_range(uint32_t offs, uint8_t *buf, int num)
{
    uint8_t bt = 0; // for reading
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, offs, buf, num);     // write the block back

    // now check of programming was succesful
    bool succes = true;
    int i = 0;
    do {
      fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, offs + i, &bt, 1); // read the byte back
      succes = (buf[i] == bt);
      i++;
    } while ((i < num) && succes);

    return succes;
}

// check if the FRAM is fully erased
// num indicates granularity, 1 every byte is checked, 256 means every 256nd byte is checked
// checks the range starting with offs for size bytes
uint32_t fr_erased(uint32_t offs, uint32_t size, int num)
{
    uint32_t addr = offs;             // our address counter, 
                                      // start at the beginning of the file system
    uint32_t end = FRAM_SIZE;         // end of the file system
    bool erased = true;               // assume FRAM is erased

    // check for correct range
    if ((offs + size) > end) return false;

    do {
      uint8_t bt = 0;
      erased = (bt == 0x00);          // check if byte is erased
      fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, addr, &bt, 1); // read the byte
      addr += num;                   // next byte to check
    } while ((addr < (offs + size)) && erased);     // until end of file system or not erased

    if (erased) return NOTFOUND;                    // all is erased
    return (addr - num);                            // return the first non-erased address
}




// find the last free address in FRAM starting from offs
// returns the address of the last free byte or 0xFFFFFFFF if none found    
uint32_t fr_lastfree(uint32_t offs)
{
    uint32_t addr = offs;               // our address counter, 
                                        // start at the beginning of the file system
    ModuleMetaHeader_t MetaH;           // meta header for getting info of next file

    // first find the last available slot in FRAM

    uint32_t end = FRAM_SIZE;

    while (offs < (end - sizeof(ModuleMetaHeader_t))) {

      #ifdef DEBUG
        // cli_printf("  checking file at 0x%08X end at: 0x%08X", offs, end);
      #endif

      // read the File Header from FRAM
      fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, offs, (uint8_t*)&MetaH, sizeof(ModuleMetaHeader_t));

      if (MetaH.FileType == FILETYPE_FFFF) break;                 // if it is erased then it is useable
      // check if this is a valid file to prevent looping
      if ((MetaH.FileType > FILETYPE_4041)) {
        // unsupported file type, this is not a valid file
        return NOTFOUND;                                          // no free space found
      }
      offs = MetaH.NextFile;                                      // go to the next file
    }
    if (offs > end) return NOTFOUND;                              // no free space found
    return offs;                                                  // return the offset of the last free slot
}

bool fr_isinited()
{
    // check if the fram file system is initialized
    // this is done by checking the first file type

    // Read the first byte from FRAM
    uint8_t filetype = 0; 
    fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, FF_SYSTEM_BASE, &filetype, 1); // read the byte
    
    if (filetype == FILETYPE_4041) {
      // file system is initialized
      return true;
    } else {
      // file system is not initialized
      return false;
    }



}

// find a free block of size bytes in FRAM starting from offs
// returns the address of the first free byte or 0xFFFFFFFF if none found
uint32_t fr_findfree(uint32_t offs, uint32_t size)
{
  uint32_t addr = offs;                         // our address counter, 
  ModuleMetaHeader_t MetaH;                     // meta header for getting info of next file

                                                  // start at the beginning of the file system
  uint32_t fr_end = FRAM_SIZE;                  // end of the file system
  uint32_t next;                                // offset to the next file
  uint32_t candidate = 0;                       // candidate for the file slot
  uint32_t cand_size = 0;                       // size of the candidate file slot
  uint32_t current = 0;                         // current file slot under investigation
  uint32_t space = 0;                           // size of the free space
  uint32_t space_acc = 0;                       // accumulated size of the free space

  size = size + sizeof(ModuleMetaHeader_t);     // required size of file plus header

  #ifdef DEBUG
    cli_printf("  searching for free space in FRAM, size %d / %d bytes", size, size - sizeof(ModuleMetaHeader_t));
  #endif

  while (offs < fr_end) {
    // read the File Header from FRAM
    fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, offs, (uint8_t*)&MetaH, sizeof(ModuleMetaHeader_t));
    uint8_t filetype = MetaH.FileType;                      // get the file type

    tud_task();  // keep the USB port updated

    switch(filetype) {
      case FILETYPE_FFFF:                                   // end of the file system
        #ifdef DEBUG
          cli_printf("  end of file system reached at %08X", offs);
        #endif
        // we have reached the end of the File System
        // check the remaining space

        if ((fr_end - offs) > size) {
          // there is enough size left
          // is this space is smaller than the current candidate then this is better
          // and we can return the offset
          if ((fr_end - offs) < cand_size) {
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
        next = MetaH.NextFile;             // pointer to the next file to get the available space
        space = next - offs;               // available space in this file

        #ifdef DEBUG
          cli_printf("  deleted space at %08X of %d bytes", offs, space);
        #endif

        // if the file fits we report this space, if not we check if there is free space after this file
        if (space >= size) {
          // there is enough space in this file
          // check if this is smaller than the current candidate then this is better
          // and we can return the offset and continue the search
          // if the candidate was still 0 this is a possible candidate
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
          // cli_printf("  skipping file at %08X of type %02X, next is %08X", offs, filetype, MetaH.NextFile);
        #endif
        offs = MetaH.NextFile;                                     // go to the next file
        break;
    }
  }

  // if we get here then we have reached the end of the file system
  // and most likely no free space is found and candidate is still 0
  return candidate;                               

}




// find a file by name in FRAM starting from offs
// returns the address of the file header or 0xFFFFFFFF if not found
uint32_t fr_findfile(const char *name)
{
    uint32_t addr = FF_SYSTEM_BASE;      // our address counter, 
                                        // start at the beginning of the file system
    ModuleMetaHeader_t MetaH;           // meta header for getting info of next file

    do {
      // read the File Header from FRAM
      fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, addr, (uint8_t*)&MetaH, sizeof(ModuleMetaHeader_t));

      if (MetaH.FileType == FILETYPE_FFFF) break;                 // if it is erased then file not found
      // check if this is a valid file to prevent looping
      if ((MetaH.FileType > FILETYPE_4041)) {
        // unsupported file type, this is not a valid file
        return NOTFOUND;                                          // file not found
      }
      // compare the name
      if (strncmp(MetaH.FileName, name, sizeof(MetaH.FileName)) == 0) {
        // found the file
        return addr;
      }
      addr = MetaH.NextFile;                                      // go to the next file
    } while (addr < FRAM_SIZE);                                   // until end of file system

    return NOTFOUND;                                              // file not found


}





