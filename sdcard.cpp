/*
 * sdcard.cpp
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

#include "sdcard.h"



// this file contains all functions relevant for the microSD card on TULIP4041
// mainly called by userinterface.cpp


// functions implemented here:
//
//      sd_mount
//      sd_unmount
//      sd_status
//      sd_dir
//      sd_error

// only one drive is available on TULIP


// to keep tack who is using the uSD card
//      true  - used by USB MSC stack
//      false - used by local filesystem functions
static bool sdcard_usb_used = true;

// using a global object for the sd card structure to keep track of the status
//static sd_card_t *sd_card_p;
// static sd_card_t *TULIP_uSD_p;


/* Implement library message callbacks */
void put_out_error_message(const char *s) {
    cli_printf("x");
    cli_printf("%s", s);
}
void put_out_info_message(const char *s) {
    cli_printf("x");cli_printf("%s", s);
}
/* This will not be called unless build_flags include "-D USE_DBG_PRINTF": */
void put_out_debug_message(const char *s) {
    cli_printf("x");cli_printf("%s", s);
}

// #define printf cli_printf
// #define puts cli_printf

#define error(s)                  \
    {                             \
        printf("ERROR: %s", s); \
        for (;;) __breakpoint();  \
    }

static char const *fs_type_string(int fs_type) {
    switch (fs_type) {
        case FS_FAT12:
            return "FAT12";
        case FS_FAT16:
            return "FAT16";
        case FS_FAT32:
            return "FAT32";
        case FS_EXFAT:
            return "EXFAT";
        default:
            return "Unknown";
    }
} 

// initialize the needed timer and the driver for FatFS
void sdcard_init()
{
    // time_init();   // no RTC on the RP2350
    sd_init_driver();
}

// to verify if the uSD card is mounted
void sd_mounted()
{
    sd_card_t *TULIP_uSD_p = sd_get_by_num(DRIVENUM);
    if (!TULIP_uSD_p) {
        // no valid drive initialized
        cli_printf("  no drive initialized");
    } else {
        // check if drive mounted
        if (TULIP_uSD_p->state.mounted) {
            cli_printf("  SD card mounted");
        } else {
            cli_printf("  SD card not mounted");
        }
    }
}

bool sd_mount()
{
    sd_card_t *TULIP_uSD_p = sd_get_by_num(DRIVENUM);
    
    if (!TULIP_uSD_p)
    {
        // not good, cannot get pointer
        cli_printf("Unknown logical drive id");
        return false;
    }

    FATFS *fs_p = &TULIP_uSD_p->state.fatfs;   
    FRESULT fr;
    if (!TULIP_uSD_p->state.mounted) {
        cli_printf("  uSD card is not mounted");
        cli_printf("  mounting uSD card");
        fr = f_mount(fs_p, DEFAULT_DRIVE, 1);
        if (FR_OK != fr) {
            cli_printf("  f_mount error: %s (%d)", FRESULT_str(fr), fr);
            return false;
        }
        TULIP_uSD_p->state.mounted = true;
        cli_printf("  uSD card is mounted");
    }
    else
    {
        cli_printf("  uSD card is already mounted");
    }
    return true;
}


// mount the SD card
// same as sd_mount, but silent, no messages to the CLI except errors
bool sd_mount_s()
{
    sd_card_t *TULIP_uSD_p = sd_get_by_num(DRIVENUM);
    
    if (!TULIP_uSD_p)
    {
        // not good, cannot get pointer
        cli_printf("Unknown logical drive id");
        return false;
    }

    FATFS *fs_p = &TULIP_uSD_p->state.fatfs;   
    FRESULT fr;
    if (!TULIP_uSD_p->state.mounted) {
        fr = f_mount(fs_p, DEFAULT_DRIVE, 1);
        if (FR_OK != fr) {
            cli_printf("  f_mount error: %s (%d)", FRESULT_str(fr), fr);
            return false;
        }
        TULIP_uSD_p->state.mounted = true;
    }
    return true;
}


void sd_unmount()
{
    sd_card_t *TULIP_uSD_p = sd_get_by_num(DRIVENUM);
    if (!TULIP_uSD_p)
    {
        // not good, cannot get pointer
        cli_printf("Unknown logical drive id");
        return;
    }

    FRESULT fr = f_unmount(DEFAULT_DRIVE);
    if (FR_OK != fr) {
        cli_printf(" f_unmount error: %s (%d)", FRESULT_str(fr), fr);
        return;
    }
    TULIP_uSD_p->state.mounted = false;
    TULIP_uSD_p->state.m_Status |= STA_NOINIT;  // in case medium is removed

    cli_printf("  uSD card is unmounted");
}



// this is the dir functions, shows the directory listing
FRESULT sd_dir(const char *dir)
{
    char cwdbuf[FF_LFN_BUF] = {0};
    FRESULT fr; /* Return value */
    char const *p_dir;
    if (dir[0]) {
        p_dir = dir;
    } else {
        fr = f_getcwd(cwdbuf, sizeof cwdbuf);
        if (FR_OK != fr) {
            cli_printf("  f_getcwd error: %s (%d)", FRESULT_str(fr), fr);
            return fr;
        }
        p_dir = cwdbuf;
    }
    cli_printf("Directory Listing: %s", p_dir);
    DIR dj = {};      /* Directory object */
    FILINFO fno = {}; /* File information */
    assert(p_dir);
    fr = f_findfirst(&dj, &fno, p_dir, "*");
    if (FR_OK != fr) {
        cli_printf("  f_findfirst error: %s (%d)", FRESULT_str(fr), fr);
        return fr;
    }
    while (fr == FR_OK && fno.fname[0]) { /* Repeat while an item is found */
        tud_task();  // must keep the USB port updated

        /* Create a string that includes the file name, the file size and the
         attributes string. */
        const char *pcWritableFile = "rw ",
                   *pcReadOnlyFile = "r- ",
                   *pcDirectory = "DIR";
        const char *pcAttrib;
        /* Point pcAttrib to a string that describes the file. */
        if (fno.fattrib & AM_DIR) {
            pcAttrib = pcDirectory;
        } else if (fno.fattrib & AM_RDO) {
            pcAttrib = pcReadOnlyFile;
        } else {
            pcAttrib = pcWritableFile;
        }
        /* Create a string that includes the file name, the file size and the
         attributes string. */
        cli_printf("%-20s [%s] [%10d]", fno.fname, pcAttrib, (long)fno.fsize);


        fr = f_findnext(&dj, &fno); /* Search for next item */
    }
    f_closedir(&dj);
    return FR_OK;
}

// return the number of total sector in the SD card
// called by msc_device_disk callbacks from the TinyUSB stack
uint32_t sd_sect_count()
{
    sd_card_t *sd_card_p = sd_get_by_num(0);
    FATFS *fs_p = &sd_card_p->state.fatfs; 

    DWORD fre_clust, fre_sect, tot_sect;
    FRESULT fr = f_getfree(DEFAULT_DRIVE, &fre_clust, &fs_p);
    if (FR_OK != fr) {
        cli_printf("  f_getfree error: %s (%d)", FRESULT_str(fr), fr);
        return 0;
    }

    // Get total sectors
    tot_sect = (fs_p->n_fatent - 2) * fs_p->csize;
    return tot_sect;
}


// SD card status
void sd_status() 
{
    sd_card_t *TULIP_uSD_p = sd_get_by_num(DRIVENUM);

    for (size_t i = 0; i < sd_get_num(); ++i) {
        sd_card_t *TULIP_uSD_p = sd_get_by_num(i);
        if (TULIP_uSD_p) {
            cli_printf("  uSD card at drivenum : %d", i);
        } else {
            cli_printf("  no uSD card found at : %d", i);
        }
    }

    if (!TULIP_uSD_p)
    {
        // card was not initialized, try again
        cli_printf("  uSD card was not initialized, try again");
        TULIP_uSD_p = sd_get_by_num(DRIVENUM);
    }

    if (!TULIP_uSD_p)
    {
        // not good, cannot get pointer
        cli_printf("Unknown logical drive id");
        return;
    }

    // now initialize the disk
    int ds = TULIP_uSD_p->init(TULIP_uSD_p);
    if (STA_NODISK & ds || STA_NOINIT & ds) {
        // cannot initialize
        cli_printf("  uSD card initialization failed");
        return;
    } else {
        cli_printf("  uSD card initialized");
    }

    // Card IDendtification register. 128 buts wide.
    // cidDmp(sd_card_p, cli_printf);
    // Card-Specific Data register. 128 bits wide.
    // csdDmp(sd_card_p, cli_printf);
    
    // SD Status, mount the card if it was not mounted


    // check code below, does not seem to read value
    size_t au_size_bytes;
    bool ok = sd_allocation_unit(TULIP_uSD_p, &au_size_bytes);
    if (ok)
        cli_printf("  uSD card Allocation Unit (AU_SIZE) or \"segment\": %zu bytes (%zu sectors)", 
        au_size_bytes, au_size_bytes / sd_block_size);


    FATFS *fs_p = &TULIP_uSD_p->state.fatfs;   
    FRESULT fr;
    if (!TULIP_uSD_p->state.mounted) {
        cli_printf("  uSD card is not mounted");
        cli_printf("  mounting uSD card");
        fr = f_mount(fs_p, DEFAULT_DRIVE, 1);
        if (FR_OK != fr) {
            cli_printf("  f_mount error: %s (%d)", FRESULT_str(fr), fr);
            return;
        }
        TULIP_uSD_p->state.mounted = true;
    }

    /* Get volume information and free clusters of drive */
    if (!fs_p) {
        cli_printf("  Unknown logical drive id");
        return;
    }

    DWORD fre_clust;
    uint64_t fre_sect, tot_sect;
    fr = f_getfree(DEFAULT_DRIVE, &fre_clust, &fs_p);
    if (FR_OK != fr) {
        cli_printf("  f_getfree error: %s (%d)", FRESULT_str(fr), fr);
        return;
    }

    // fix sector size 
    int sect_size = SECT_SIZE;

    /* Get total sectors and free sectors */
    tot_sect = (uint64_t)((fs_p->n_fatent - 2) * fs_p->csize);
    fre_sect = (uint64_t)(fre_clust * fs_p->csize);
    uint64_t used_sect = tot_sect - fre_sect;

    #if FF_USE_LABEL
    // Report label:
    TCHAR buf[34] = {};/* [OUT] Volume label */
    DWORD vsn;         /* [OUT] Volume serial number */
    fr = f_getlabel(DEFAULT_DRIVE, buf, &vsn);
    if (FR_OK != fr) {
        cli_printf("  f_getlabel error: %s (%d)", FRESULT_str(fr), fr);
        // return;
    } else {
        cli_printf("  Volume label            : \"%s\"", buf);
        cli_printf("  Volume serial number    : %lu", vsn);
    }
    #endif

    /* Print the free space (assuming 512 bytes/sector) */
    cli_printf("  Filesystem type         : %s", fs_type_string(fs_p->fs_type));
    cli_printf("  Sector size             : %10lu bytes", sect_size);    
    cli_printf("  Cluster size            : %10lu sectors/cluster", fs_p->csize);


    cli_printf("  Total drive space       : %10" PRIu64 " KByte %10" PRIu64 " MByte", tot_sect/2, tot_sect /2 / 1024);
    cli_printf("  Avialable               : %10" PRIu64 " KByte %10" PRIu64 " MByte", fre_sect/2, fre_sect /2 / 1024);
    cli_printf("  Used                    : %10" PRIu64 " KByte %10" PRIu64 " MByte", used_sect/2, used_sect /2 / 1024);

	cli_printf("  Volume base sector      : %10d", fs_p->volbase);		
	cli_printf("  FAT base sector         : %10d", fs_p->fatbase);		
	cli_printf("  Root base sector/cluster: %10d", fs_p->dirbase);		 
	cli_printf("  Data base sector        : %10d", fs_p->database);		

    // Report cluster size ("allocation unit")
    cli_printf("  FAT Cluster size        : %10d sectors (%llu bytes)",
           fs_p->csize,
           (uint64_t)TULIP_uSD_p->state.fatfs.csize * FF_MAX_SS);

}





