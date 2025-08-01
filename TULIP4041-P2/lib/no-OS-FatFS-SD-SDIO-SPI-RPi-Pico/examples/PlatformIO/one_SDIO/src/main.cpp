/* 
Copyright 2023 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use 
this file except in compliance with the License. You may obtain a copy of the 
License at

   http://www.apache.org/licenses/LICENSE-2.0 
Unless required by applicable law or agreed to in writing, software distributed 
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR 
CONDITIONS OF ANY KIND, either express or implied. See the License for the 
specific language governing permissions and limitations under the License.
*/

/* Write "Hello, world!\n" to SD Card */
#include <string.h>
#include "FatFsSd_C.h"
//
#include "SerialUART.h"

#define printf Serial1.printf
#define puts Serial1.println

/* Implement library message callbacks */
void put_out_error_message(const char *s) {
    Serial1.write(s);
}
void put_out_info_message(const char *s) {
    Serial1.write(s);
}
// This will not be called unless build_flags include "-D USE_DBG_PRINTF":
// void put_out_debug_message(const char *s) {
//     Serial1.write(s);
// }

/*
This example assumes the following wiring:
    | GPIO | SD Card |
    | ---- | ------- |
    | GP2  | CLK     |
    | GP3  | CMD     |
    | GP4  | D0      |
    | GP5  | D1      |
    | GP6  | D2      |
    | GP7  | D3      |
    | GP9  | DET     |
*/
static sd_sdio_if_t sdio_if = {
    /* 
    Pins CLK_gpio, D1_gpio, D2_gpio, and D3_gpio are at offsets from pin D0_gpio.
    The offsets are determined by sd_driver\SDIO\rp2040_sdio.pio.
        CLK_gpio = (D0_gpio + SDIO_CLK_PIN_D0_OFFSET) % 32;
        As of this writing, SDIO_CLK_PIN_D0_OFFSET is 30, 
            which is -2 in mod32 arithmetic, so:
        CLK_gpio = D0_gpio -2.
        D1_gpio = D0_gpio + 1;
        D2_gpio = D0_gpio + 2;
        D3_gpio = D0_gpio + 3;
    */
    .CMD_gpio = 3,
    .D0_gpio = 4,
    .SDIO_PIO = pio1,
    .DMA_IRQ_num = DMA_IRQ_1,
    .baud_rate = 125 * 1000 * 1000 / 6  // 20833333 Hz
};

// Hardware Configuration of the SD Card "objects"
static sd_card_t sd_card = {
    .type = SD_IF_SDIO,
    .sdio_if_p = &sdio_if,
    // SD Card detect:
    .use_card_detect = true,
    .card_detect_gpio = 9,  
    .card_detected_true = 0, // What the GPIO read returns when a card is present.
};
/* 
The following functions are required by the library API. 
They are how the library finds out about the configuration.
*/
extern "C" size_t sd_get_num() { return 1; }

extern "C" sd_card_t *sd_get_by_num(size_t num) {
    if (0 == num) {
        return &sd_card;
    } else {
        return NULL;
    }
}

// Check the FRESULT of a library call.
//  (See http://elm-chan.org/fsw/ff/doc/rc.html.)
#define CHK_FRESULT(s, fr)                                  \
    if (FR_OK != fr) {                                      \
        printf("%s:%d %s error: %s (%d)\n",                 \
               __FILE__, __LINE__, s, FRESULT_str(fr), fr); \
        for (;;) __breakpoint();                            \
    }

void setup() {
    Serial1.begin(115200);  // set up Serial library at 9600 bps
    while (!Serial1)
        ;  // Serial is via USB; wait for enumeration
    printf("\033[2J\033[H");  // Clear Screen
    puts("Hello, world!");

    // See FatFs - Generic FAT Filesystem Module, "Application Interface",
    // http://elm-chan.org/fsw/ff/00index_e.html
    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->state.fatfs, "", 1);
    CHK_FRESULT("f_mount", fr);
    FIL fil;
    const char* const filename = "filename.txt";
    fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
    CHK_FRESULT("f_open", fr);
    char const * const str = "Hello, world!\n";
    if (f_printf(&fil, str) < strlen(str)) {
        printf("f_printf failed\n");
        for (;;) __breakpoint();
    }
    fr = f_close(&fil);
    CHK_FRESULT("f_close", fr);
    fr = f_unmount("");
    CHK_FRESULT("f_unmount", fr);

    puts("Goodbye, world!");
}
void loop() {}
