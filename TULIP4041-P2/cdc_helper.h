/*
 * cdc_helper.h
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

#ifndef __CDC_HELPER_H__
#define __CDC_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <malloc.h>
#include <stdarg.h>

#include <tusb_option.h>
#include <tusb.h>

#define CDC_PRINT_BUFFER_SIZE 512

// for the TULIP4041 we will use the following CDC ports:
//	ITF 0		main console, user interface, file transfer
// 	ITF 1		trace output HP41 bus tracer 
//  IFT 2		HP-IL frames
//  ITF 3		HP-IL frame tracing
//  ITF 4		Printer output for HP82143A, input for Wand data

#define ITF_CONSOLE     0           // CDC port 0: console, user interface
#define ITF_TRACE       1           // CDC port 1: trace listing
#define ITF_HPIL        2           // CDC port 2: HP-IL/ PILBox frames
#define ITF_ILSCOPE     3           // CDC port 3: HP-IL scope, PILBox frame monitor
#define ITF_PRINT       4           // CDC port 4: printer bytes


extern const char* __in_flash() ITF_str[];

void cdc_sendbuf(int itf, char* buffer, int len);


void cdc_send_console(char* buffer, int len);
void cdc_send_string(int itf, char* buffer, int len);
void cdc_send_char(int itf, char c);
void cdc_send_char_flush(int itf, char c);

void cdc_send_printport(char c);
bool cdc_connected(int itf);

void cdc_flush_console();
void cdc_flush(int itf);

uint32_t cdc_available(int itf);
int16_t cdc_read_byte(int itf);
int cdc_read_buf(int itf, char* buffer, int len);
char cdc_read_char(int itf);

void cdc_printf_console(const char *format, ...);       // prevent using this function
void cdc_printf_(int itf, const char *format, ...);     // prevent using this function


#ifdef __cplusplus
}
#endif

#endif