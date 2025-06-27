/*
 * cdc_helper.c
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

// cdc_helper.c
// helper functions for the multi CDC USB interface
// ITF assignments:
//      0 - console, user interface (currently user interface running on UART)
//          console is now used for the tracer output
//      1 - HP82143 printer port
//      2 - TRACE listing
//      3 - HP-IL/PILBox communication
//      4 - HP-IL scope output

#include "cdc_helper.h"

// for the TULIP4041 we will use the following CDC ports:
//	ITF 0		main console, user interface, file transfer
// 	ITF 1		trace output HP41 bus tracer 
//  IFT 2		HP-IL frames
//  ITF 3		HP-IL frame tracing
//  ITF 4		Printer output for HP82143A, input for Wand data

const char* __in_flash() ITF_str[] = {  "ITF_CONSOLE",
                                        "ITF_TRACE",
                                        "ITF_HPIL",
                                        "ITF_ILSCOPE",
                                        "ITF_PRINT", };

// flushes the CDC port
void cdc_flush(int itf)
{
    tud_cdc_n_write_flush(itf);
}


// returns the status of the port, if something is connected
bool cdc_connected(int itf)
{
    return tud_cdc_n_connected(itf);
}


// function to wait for enough room in the USB output buffer
static void wait_for_write(int itf, uint32_t len)
{
    uint32_t avail;
    do {
        tud_task();
        avail = tud_cdc_n_write_available(itf);
    } while (avail < len);
}


// send a complete prepared buffer to a CDC inteface
// does not check if something is connected, should be done by the caller
// does not flush the buffer, can be done by the caller if needed
void cdc_sendbuf(int itf, char* buffer, int len)
{
    int sent;       // number of bytes really sent

    wait_for_write(itf, len);                   // wait until enough room is in the output queue
    sent = tud_cdc_n_write(itf, buffer, len);   // and send it
}

// printf version for printing into the console
// and flushes the buffer
void cdc_printf_console(const char *format, ...) {

    char buffer[CDC_PRINT_BUFFER_SIZE];

    int i = CDC_PRINT_BUFFER_SIZE;
    int len;
    
    len = sprintf(buffer, format);

    if (len >= 0) 
    {
        tud_cdc_n_write(ITF_CONSOLE, buffer, len);
        // tud_cdc_n_write_flush(ITF_CONSOLE);
    }
}

// printf version for printing a formatted string
// does not flush
void cdc_printf_(int itf, const char *format, ...) {

    char buffer[CDC_PRINT_BUFFER_SIZE];
    int len;
    
    len = sprintf(buffer, format);
    if (len >= 0) 
    {
        tud_cdc_n_write(itf, buffer, len);
    }
}

// function to send one char to the printerport, and flush buffer
void cdc_send_printport(char c)
{
	tud_cdc_n_write_char(ITF_PRINT, c);
	tud_cdc_n_write_flush(ITF_PRINT);
}

// function to send one char to a port, no flush
void cdc_send_char(int itf, char c)
{
	tud_cdc_n_write_char(itf, c);
}

// function to send one char to a port and flush
void cdc_send_char_flush(int itf, char c)
{
	tud_cdc_n_write_char(itf, c);
	tud_cdc_n_write_flush(itf);
}

// functions to read one byte from a CDC port
// returns -1 if no byte available
int16_t cdc_read_byte(int itf)
{
    char c;
    
    if (tud_cdc_n_available(itf))               // return number of bytes available
    {
        tud_cdc_n_read(itf, &c, 1);
        return (int16_t)c;
    }
    else
    {
        return -1;
    }
}

// function to read one char from a CDC port
// returns 0 if no chars available
char cdc_read_char(int itf)
{
    char c;
    
    if (tud_cdc_n_available(itf))               // return number of bytes available
    {
        tud_cdc_n_read(itf, &c, 1);
        return c;
    }
    else
    {
        return 0;
    }
}


// read len bytes from the CDC buffer
// the function returns the number of bytes actually read
int cdc_read_buf(int itf, char* buffer, int len)
{
    return tud_cdc_n_read(itf, buffer, len);
}


// function to return number of bytes available in the CDC buffer
uint32_t cdc_available(int itf)
{
    return tud_cdc_n_available(itf);
}

// the functions below shall not be used, will be removed
void cdc_send_console(char* buffer, int len)
{

    // while (len) {
    //     wait_for_write(ITF_CONSOLE, len + 50);
    //     uint32_t w = tud_cdc_n_write(ITF_CONSOLE, buffer, len);
    //     buffer += w;
    //     len -= w;
    // }
    int sent;       // number of bytes really sent


    wait_for_write(ITF_CONSOLE, len);

    sent = tud_cdc_n_write(ITF_CONSOLE, buffer, len);

    if (sent != len)
    {
        // something going on
        printf("\n** len: %d - sent: %d  %.10s\n", len, sent, buffer);

    }
    /*
    if (len >64)
    {
        tud_cdc_n_write(ITF_CONSOLE, buffer, 64);
        wait_for_write(ITF_CONSOLE, len);
        tud_cdc_n_write(ITF_CONSOLE, buffer + 64, len -64);
    }
    else{
        tud_cdc_n_write(ITF_CONSOLE, buffer, len);
    }
    */

    // tud_cdc_n_write_flush(ITF_CONSOLE);
}

void cdc_flush_console()
{
    tud_cdc_n_write_flush(ITF_CONSOLE);
}

void cdc_read_flush(int itf)
{
    tud_cdc_n_read_flush(itf);
}

// send the string in buffer with len characters
// the function does not flush
void cdc_send_string(int itf, char* buffer, int len)
{
    // while (len) {
    //     wait_for_write(ITF_CONSOLE, len + 50);
    //     uint32_t w = tud_cdc_n_write(ITF_CONSOLE, buffer, len);
    //     buffer += w;
    //     len -= w;
    // }
    int sent;       // number of bytes really sent

    wait_for_write(itf, len);                   // wait until len bytes available in the send buffer
    sent = tud_cdc_n_write(itf, buffer, len);
    if (sent != len)
    {
        // something going on
        printf("\n** len: %d - sent: %d  %.10s\n", len, sent, buffer);
    }
}


// initializes the USB CDC ports
void usbd_init()
{
    usbd_serial_init();
}




