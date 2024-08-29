/*
 * usb-descriptors.c
 * 
 * USE AT YOUR OWN RISK
 *
 */

// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2021 Álvaro Fernández Rojas <noltari@gmail.com>
 *
 * This file is based on a file originally part of the
 * MicroPython project, http://micropython.org/
 *
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * Copyright (c) 2019 Damien P. George
 */

// #define PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE 0
// #define PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE 1 

#include "pico.h"
#include "hardware/flash.h"

// #include "tusb_config.h"
#include <tusb.h>

#define DESC_STR_MAX 20
#define CFG_TUD_EP_MAX 16

#define USBD_VID 0x2E8A /* Raspberry Pi */
// #define USBD_VID 0xCAFE /* dummy for testing */
#define USBD_PID 0x000A /* Raspberry Pi Pico SDK CDC */

#define TUD_RPI_RESET_DESC_LEN  9 

// added TUD_RPI reset
// #define USBD_DESC_LEN (TUD_CONFIG_DESC_LEN + (TUD_VENDOR_DESC_LEN * CFG_TUD_VENDOR) + (TUD_CDC_DESC_LEN * CFG_TUD_CDC) + TUD_RPI_RESET_DESC_LEN) 
// #define USBD_DESC_LEN (TUD_CONFIG_DESC_LEN + (TUD_VENDOR_DESC_LEN * CFG_TUD_VENDOR) + (TUD_CDC_DESC_LEN * CFG_TUD_CDC)) 

//  to be used for CDC +MSC
// #define USBD_DESC_LEN (TUD_CONFIG_DESC_LEN + (TUD_VENDOR_DESC_LEN * CFG_TUD_VENDOR) + (TUD_CDC_DESC_LEN * CFG_TUD_CDC)) + (TUD_MSC_DESC_LEN)
#define USBD_DESC_LEN (TUD_CONFIG_DESC_LEN  + (TUD_CDC_DESC_LEN * CFG_TUD_CDC)) + (TUD_MSC_DESC_LEN)


#define USBD_MAX_POWER_MA 500

// for the TULIP4041 we will use the following CDC ports:
//	ITF 0		main console, user interface, file transfer
// 	ITF 1		trace output HP41 bus tracer 
//  IFT 2		HP-IL frames
//  ITF 3		HP-IL frame tracing
//  ITF 4		Printer output for HP82143A, input for Wand data
 
#define USBD_ITF_CDC_0 (0)
#define USBD_ITF_CDC_1 (2)
#define USBD_ITF_CDC_2 (4)
#define USBD_ITF_CDC_3 (6)
#define USBD_ITF_CDC_4 (8)
#define USBD_ITF_MSC   (10)

// #define USBD_ITF_RPI_RESET (2)				// for RPI reset with Picotool

// #define USBD_ITF_MAX (10)					// CDC only
#define USBD_ITF_MAX (11)					// CDC + MSC
// #define USBD_ITF_MAX 4 (original)

#define USBD_CDC_0_EP_CMD 0x81
#define USBD_CDC_1_EP_CMD 0x83
#define USBD_CDC_2_EP_CMD 0x85
#define USBD_CDC_3_EP_CMD 0x87
#define USBD_CDC_4_EP_CMD 0x89  

#define USBD_CDC_0_EP_OUT 0x01
#define USBD_CDC_1_EP_OUT 0x03
#define USBD_CDC_2_EP_OUT 0x05
#define USBD_CDC_3_EP_OUT 0x07
#define USBD_CDC_4_EP_OUT 0x09

#define USBD_CDC_0_EP_IN 0x82
#define USBD_CDC_1_EP_IN 0x84
#define USBD_CDC_2_EP_IN 0x86
#define USBD_CDC_3_EP_IN 0x88
#define USBD_CDC_4_EP_IN 0x8A // check this value, is different in example !! 8D works at 9600, 8E seems to work best

#define USBD_CDC_CMD_MAX_SIZE 8
#define USBD_CDC_IN_OUT_MAX_SIZE 64

#define USBD_MSC_OUT	0x0B
#define USBD_MSC_IN		0x8C

#define USBD_STR_0 0x00
#define USBD_STR_MANUF 0x01
#define USBD_STR_PRODUCT 0x02
#define USBD_STR_SERIAL 0x03
#define USBD_STR_SERIAL_LEN 17
#define USBD_STR_CDC 0x04
#define USBD_STR_RPI_RESET (0x05) 

#define USBD_STR_MSC 0x05


static const tusb_desc_device_t usbd_desc_device = {
	.bLength 			= sizeof(tusb_desc_device_t),
	.bDescriptorType 	= TUSB_DESC_DEVICE,
	.bcdUSB 			= 0x0200,

	// Use Interface Association Descriptor (IAD) for CDC
  	// As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)  
	.bDeviceClass 		= TUSB_CLASS_MISC,
	.bDeviceSubClass 	= MISC_SUBCLASS_COMMON,
	.bDeviceProtocol 	= MISC_PROTOCOL_IAD,
	.bMaxPacketSize0 	= CFG_TUD_ENDPOINT0_SIZE,
	.idVendor 			= USBD_VID,
	.idProduct 			= USBD_PID,
	.bcdDevice 			= 0x0100,
	.iManufacturer 		= USBD_STR_MANUF,
	.iProduct 			= USBD_STR_PRODUCT,
	.iSerialNumber 		= USBD_STR_SERIAL,
	.bNumConfigurations = 1,
};


#define TUD_RPI_RESET_DESCRIPTOR(_itfnum, _stridx) \
  /* Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 0, TUSB_CLASS_VENDOR_SPECIFIC, RESET_INTERFACE_SUBCLASS, RESET_INTERFACE_PROTOCOL, _stridx,

static const uint8_t usbd_desc_cfg[USBD_DESC_LEN] = {
	TUD_CONFIG_DESCRIPTOR(1, USBD_ITF_MAX, USBD_STR_0, USBD_DESC_LEN,
		TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USBD_MAX_POWER_MA),
	
	TUD_CDC_DESCRIPTOR(USBD_ITF_CDC_0, USBD_STR_CDC, USBD_CDC_0_EP_CMD,
		USBD_CDC_CMD_MAX_SIZE, USBD_CDC_0_EP_OUT, USBD_CDC_0_EP_IN,
		USBD_CDC_IN_OUT_MAX_SIZE),

	// TUD_RPI_RESET_DESCRIPTOR(USBD_ITF_RPI_RESET, USBD_STR_RPI_RESET)

	TUD_CDC_DESCRIPTOR(USBD_ITF_CDC_1, USBD_STR_CDC, USBD_CDC_1_EP_CMD,
		USBD_CDC_CMD_MAX_SIZE, USBD_CDC_1_EP_OUT, USBD_CDC_1_EP_IN,
		USBD_CDC_IN_OUT_MAX_SIZE),

	TUD_CDC_DESCRIPTOR(USBD_ITF_CDC_2, USBD_STR_CDC, USBD_CDC_2_EP_CMD,
		USBD_CDC_CMD_MAX_SIZE, USBD_CDC_2_EP_OUT, USBD_CDC_2_EP_IN,
		USBD_CDC_IN_OUT_MAX_SIZE),

	TUD_CDC_DESCRIPTOR(USBD_ITF_CDC_3, USBD_STR_CDC, USBD_CDC_3_EP_CMD,
		USBD_CDC_CMD_MAX_SIZE, USBD_CDC_3_EP_OUT, USBD_CDC_3_EP_IN,
		USBD_CDC_IN_OUT_MAX_SIZE),

	TUD_CDC_DESCRIPTOR(USBD_ITF_CDC_4, USBD_STR_CDC, USBD_CDC_4_EP_CMD,
		USBD_CDC_CMD_MAX_SIZE, USBD_CDC_4_EP_OUT, USBD_CDC_4_EP_IN,
		USBD_CDC_IN_OUT_MAX_SIZE),  

	// to be added for the MSC device
	// Interface number, string index, EP Out & EP In address, EP size
  	TUD_MSC_DESCRIPTOR(USBD_ITF_MSC, USBD_STR_MSC, USBD_MSC_OUT, USBD_MSC_IN, 64),  

};

static char usbd_serial[USBD_STR_SERIAL_LEN] = "000000000000";

/* commentd out and replaced with usb_desc_str below for CPP compilation
static const char *const usbd_desc_str[] = {
	[USBD_STR_MANUF] = "Raspberry Pi",
	[USBD_STR_PRODUCT] = "Pico",
	[USBD_STR_SERIAL] = usbd_serial,
	[USBD_STR_CDC] = "TULIP",
	// [USBD_STR_RPI_RESET] = "Reset",
};  */

static const char *const usbd_desc_str[] = {	
	"",
	"Mein-USB",				// Manufacturer string		
	"TEST4041",				// Product		
	usbd_serial,			// Serial number, Flash chip ID will be filled here
	"TULIP-CDC",     		// CDC Interface
	// "TULIP-MSC",	    	// MSC Interface
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor   
const uint8_t *tud_descriptor_device_cb(void)
{
	return (const uint8_t *) &usbd_desc_device;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index)
{
	return usbd_desc_cfg;
}

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
	static uint16_t desc_str[DESC_STR_MAX];
	uint8_t len;

	if (index == 0) {
		desc_str[1] = 0x0409;
		len = 1;
	} else {
		const char *str;
		char serial[USBD_STR_SERIAL_LEN];

		if (index >= sizeof(usbd_desc_str) / sizeof(usbd_desc_str[0]))
			return NULL;

		str = usbd_desc_str[index];
		for (len = 0; len < DESC_STR_MAX - 1 && str[len]; ++len)
			desc_str[1 + len] = str[len];
	}

	desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * len + 2);

	return desc_str;
}


void usbd_serial_init(void)
{
	uint8_t id[8];
	int drivercount;

	flash_get_unique_id(id);

	snprintf(usbd_serial, USBD_STR_SERIAL_LEN, "%02X%02X%02X%02X%02X%02X%02X%02X",
		 id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
}
