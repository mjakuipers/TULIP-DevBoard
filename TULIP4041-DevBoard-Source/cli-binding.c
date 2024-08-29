/*
 * cli-binding.c
 *
 * This file is part of the TULIP4041 project.
 * Copyright (C) 2024 Meindert Kuipers
 *
 * Distributed under the MIT license
 * This is free software: you are free to change and redistribute it.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Version info
 * 00.01.01 initial public release
 *          support for limited function set:
 *            system - status/welcome/pio/ident
 *            romlist
 *            emulation on/off
 *            tracer on/off
 *            ilscope on/off
 *            ilprinter on/off
 *            tracefilter inlcude range
 *            tracefilter exclude range
 *            tracer sysrom on/off
 */

// file contains only the bindings for the embedded-cli
// implementation of the functions is in userinterface.c

#include <stdio.h>
#include "cli-binding.h"


// Expand cli implementation here (must be in one file only)
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"

#define CLI_BUFFER_SIZE 500
#define CLI_RX_BUFFER_SIZE 16
#define CLI_CMD_BUFFER_SIZE 32
#define CLI_HISTORY_SIZE 32
#define CLI_BINDING_COUNT 5


EmbeddedCli *cli;

CLI_UINT cliBuffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];

EmbeddedCliConfig *config;

bool firstconnect = false;          // to detect the first connect to a CDC host
                                    // to display a welcome message

// other requirements for the CLI implementatio:


// create an instance in the CLI
//      EmbeddedCli *cli = embeddedCliNew(config);

// or with the default config
//      EmbeddedCli *cli = embeddedCliNewDefault();

// Provide a function that will be used to send chars to the other end:
//      void writeChar(EmbeddedCli *embeddedCli, char c);
//      ...
//      cli->writeChar = writeChar;

// After creation, provide desired bindings to CLI (can be provided at any point in runtime):
//      embeddedCliAddBinding(cli, {
//         "get-led",          // command name (spaces are not allowed)
//         "Get led status",   // Optional help for a command (NULL for no help)
//         false,              // flag whether to tokenize arguments (see below)
//         nullptr,            // optional pointer to any application context
//         onLed               // binding function 
//      });
//      embeddedCliAddBinding(cli, {
//         "get-adc",
//         "Read adc value",
//         true,
//         nullptr,
//         onAdc
//      });

//Don't forget to create binding functions as well:
//  void onLed(EmbeddedCli *cli, char *args, void *context) {
//     // use args as raw null-terminated string of all arguments
//  }
//  void onAdc(EmbeddedCli *cli, char *args, void *context) {
//     // use args as list of tokens
//  }



EmbeddedCli *getCliPointer() {
    return cli;
}


// Function to encapsulate the 'embeddedCliPrint()' call with print formatting arguments (act like printf(), but keeps cursor at correct location).
// The 'embeddedCliPrint()' function does already add a linebreak ('\r\n') to the end of the print statement, so no need to add it yourself.

int cli_printf(const char *format, ...) {
    // Create a buffer to store the formatted string
    char buffer[CLI_PRINT_BUFFER_SIZE];

    // Format the string using snprintf
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Check if string fitted in buffer else print error to stderr
    if (length < 0) {
        fprintf(stderr, "Error formatting the string\r\n");
        return EOF;
    }

    // Call embeddedCliPrint with the formatted string
    tud_task();  // must keep the USB port updated
    embeddedCliPrint(getCliPointer(), buffer);
    return 0;
}


const char* on_off[] =
// lfor generic argument testing
{
    "on"            // feature on
    "off",          // feature off
};

const char* __in_flash()system_cmds[] =
// list of arguments for the system command
{
    "status",
    "pio",
    "cdc",
    "cdcident",
    "REBOOT",
    "BOOTSEL",
    "poweron",
    "calcreset",
};

void onSystemCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);
    int cmd = -1;
    int num_cmds = sizeof(system_cmds) / sizeof(char *);

    if (arg1 == NULL) {
        // no argument given, just show the system status
        // for testing just use the welcome message        
        cli_printf("type help for more info");
        uif_status();      // calling the welcome in userinterface.cpp        
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
      cmd = strcmp(arg1, system_cmds[i]);
      i++;
    }

    if (cmd != 0) 
    {
      i = -1;
    }

    // argument found, now execute
    switch (i) {
      case 1 : uif_status();            // status
            break;
      case 2 : uif_pio_status();        // pio
            break;
      case 3 : uif_cdc_status();        // cdc
            break;
      case 4 : uif_cdc_ident();         // cdcident
            break;
      case 5 : uif_reboot();            // REBOOT
            break;
      case 6 : uif_bootsel();           // BOOTSEL
            break;
      case 7 : uif_poweron();           // poweron
            break;
      case 8 : uif_calcreset();         // calcreset
            break;
      default:
          cli_printf("system: unkown command %s\n", arg1);    // unknown command
    }
}


// dir command, show directopry listing of SD card
void onDirCLI(EmbeddedCli *cli, char *args, void *context) 
{
    const char *arg1 = embeddedCliGetToken(args, 1);
    if (arg1 == NULL) {
        // no argument given, list the root directory 
        uif_dir(".");      
        return;
    }
    else
    {
        // arg1 was not empty
        uif_dir(arg1);
    }
}

// show all roms in flash/fram
void onListCLI(EmbeddedCli *cli, char *args, void *context) 
{
    
    //  extern void uif_list();               // list
}

// initialize the TULIP4041
//      - erase flash pages
//      - progarm HP-IL and HP-IL Printer pages
void onInitCLI(EmbeddedCli *cli, char *args, void *context) 
{

    //  extern void uif_init_rom();           // init CONFIG
}

const char* __in_flash()sdcard_cmds[] =
// list of arguments for the system command
{
    "status",
    "mount",
    "unmount",
    "mounted",
    "connect",
    "eject",
};

void onSDCardCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);
    int cmd = -1;
    int num_cmds = sizeof(sdcard_cmds) / sizeof(char *);

    if (arg1 == NULL) {
        // no argument given, just show the system status
        // for testing just use the welcome message        
        cli_printf("type help for more info");
        uif_sdcard_status();      // calling the welcome in userinterface.cpp        
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
      cmd = strcmp(arg1, sdcard_cmds[i]);
      i++;
    }

    if (cmd != 0) 
    {
      i = -1;
    }

    // argument found, now execute
    switch (i) {
      case 1 : uif_sdcard_status();     // status
          break;
      case 2 : uif_sdcard_mount();      // mount
          break;
      case 3 : uif_sdcard_unmount();    // unmount
          break;
      case 4 : uif_sdcard_mounted();    // mounted
          break;          
      case 5 : uif_sdcard_connect();    // unmount
          break;
      case 6 : uif_sdcard_eject();      // mounted
          break; 

      default:
          cli_printf("sdcard: unkown command %s\n", arg1);    // unknown command
    }
}

const char* __in_flash()import_cmds[] =
// list of arguments for the import command
// import [FLASH/FRAM] [filename]
{
    "FLASH",
    "FRAM",
};

void onImportCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);        // FLASH or FRAM
    const char *arg2 = embeddedCliGetToken(args, 2);        // filename
    int cmd = -1;
    int num_cmds = sizeof(import_cmds) / sizeof(char *);

    if ((arg1 == NULL) || (arg2 ==NULL)) {
        // no argument given, just show the system status
        // for testing just use the welcome message        
        cli_printf("no arguments given, use: import [FLASH/FRAM] [filename]");    
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
      cmd = strcmp(arg1, sdcard_cmds[i]);
      i++;
    }

    if (cmd != 0) 
    {
      i = -1;
    }

    // argument found, now execute
    switch (i) {
      case 1 : 
      case 2 : uif_import(i, *arg2);     // pass 1 or 2, and the filename
          break;
      default:
          cli_printf("invalid argument %s: use: import [FLASH/FRAM] [filename]", arg1);    // unknown command
    }
}

const char* __in_flash()plug_cmds[] =
// list of arguments for the import command
// import [FLASH/FRAM] [filename]
{
    "status",
    "hpil",
    "ilprinter",
    "printer",
};


void onPlugCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);        // only one argument now
    const char *arg2 = embeddedCliGetToken(args, 2);        // 
    int cmd = -1;
    int num_cmds = sizeof(plug_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no argument given, show the plugged ROM  
        cli_printf("no arguments given, use: plug [hpil/ilprinter/printer]");   
        uif_plug(1); 
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, plug_cmds[i]);
        i++;
    }

    if (cmd != 0) 
    {
        i = -1;
    }

    if (i >= 0)
    {
        cli_printf("argument %s, %d", arg1, i);
        uif_plug(i);
    }
    else
    {
        cli_printf("invalid argument %s: use: plug [hpil/ilprinter/printer]", arg1);    // unknown command
    }

}

void onUnPlugCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);        // only one argument now
    const char *arg2 = embeddedCliGetToken(args, 2);        // 
    int cmd = -1;
    int num_cmds = sizeof(plug_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no argument given, show the plugged ROM  
        cli_printf("no arguments given, use: unplug [hpil/ilprinter/printer]");   
        uif_plug(1); 
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, plug_cmds[i]);
        i++;
    }

    if (cmd != 0) {
        i = -1;
    }

    if (i >= 0) {
        uif_unplug(i);   
    }
    else {
        cli_printf("invalid argument %s: use: unplug [hpil/ilprinter/printer]", arg1);    // unknown command
    }

}

const char* __in_flash()printer_cmds[] =
// list of arguments for the HP82143 printer command
// 
{
    "status",           // get status
    "power",            // toggle power
    "trace",            // printer mode trace
    "norm",             // printer mode normal
    "man",              // printer mode manual
    "paper",            // toggle Out Of Paper status
    "print",            // push PRINT button
    "adv",              // push ADV button
};

void onPrinterCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);        // only one argument now
    const char *arg2 = embeddedCliGetToken(args, 2);        // 
    int cmd = -1;
    int num_cmds = sizeof(printer_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no argument given, show the plugged ROM  
        cli_printf("no arguments given, use: printer [command], see help");   
        uif_printer(1); 
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, printer_cmds[i]);
        i++;
    }

    if (cmd != 0) {
        i = -1;
    }

    if (i >= 0) {
        uif_printer(i);
        
    } else {
        cli_printf("invalid argument %s: use: printer [command], see help", arg1);    // unknown command
    }
}


const char* __in_flash()xmem_cmds[] =
// list of arguments for the HP82143 printer command
// 
{
    "status",           // get status
    "dump",             // dump xmem contents
    "pattern",          // program test pattern in FRAM
    "ERASE",            // erase all Extended Memory
};

void onXMEMCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);        // only one argument now
    const char *arg2 = embeddedCliGetToken(args, 2);        // 
    int xmem = 3;
    int cmd = -1;
    int num_cmds = sizeof(xmem_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no argument given, show the plugged ROM  
        cli_printf("no arguments given, see help");   
        uif_xmem(1);   // just show the status
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, xmem_cmds[i]);
        i++;
    }

    if (cmd != 0) {
        // no valid command found, check for a valid number
        i = -1;
        int res = sscanf(arg1, "%d", &xmem);            // if one decimal is found the res = 1

        if ((res != 1) | (xmem > 2) | (xmem < 0)) {
            // no valid result and no valid command, so get out
            cli_printf("invalid argument %s: see help", arg1);    // unknown command
            return;
        }
    }

    // we get here if there is a valid command (i >= 0 or a value 0..2)
    if (i > 0) {
        // status, dump or pattern command found, pass 1..3
        uif_xmem(i);    
    } else {
        // no command found but a valid number 0..2
        // pass a value 10..12
        uif_xmem(xmem + 10);
    }
}

/*
#define TRACER_HELP_TXT "tracer functions\r\n\
        [no argument] shows the tracer status\r\n\
        status        shows the tracer status\r\n\
        trace         toggle tracer enable/disable\r\n\
        sysrom        toggle system rom tracing (Page 0, 1, 2, 3, 5)\r\n\
        ilrom         toggle tracing of Page 6 + 7\r\n\
        sysloop       toggle tracing of system loops\r\n\
        block [a1] [a2] block tracing of range between a1 and a2 (hex addresses 0000-FFFF)\r\n\
        block [n]     toggle tracing of designated block entry, n=0..15\r\n\
        block [Pn]    block Page n (n= hex 0..F)\r\n\
        block del [n] delete entry [n]
        block [no arg] show block entries\r\n\
        pass [a1] [a2] pass only tracing of range between a1 and a2 (hex addresses 0000-FFFF)\r\n\
        pass [n]      toggle tracing of designated pass entry, n=0..15\r\n\
        pass [Pn]     pass only tracing of Page n (n= hex 0..F)\r\n\
        pass [no arg] show pass entries\r\n\
        hpil          toggle HP-IL tracing to ILSCOPE USB port\r\n\
        pilbox        toggle PILBox serial tracing to ILSCOPE serial port\r\n"
*/

const char* __in_flash()tracer_cmds[] =
// list of arguments for the tracer command
// 
{
    "status",           // get status
    "trace",            // toggle tracer enable/disable
    "sysloop",          // toggle tracing of system loops
    "sysrom",           // toggle system rom tracing (Page 0, 1, 2, 3, 5)
    "ilrom",            // toggle tracing of Page 6 + 7
    "hpil",             // toggle HP-IL tracing
    "pilbox",           // toggle PILBox serial tracing
    "ilregs",           // toggle tracing of HP-IL registers
    "save",             // save tracer settings
};

void onTracerCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);        // command
    // const char *arg2 = embeddedCliGetToken(args, 2);        // start address
    // const char *arg3 = embeddedCliGetToken(args, 3);        // end address
    int cmd = -1;
    int num_cmds = sizeof(tracer_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no argument given, show the plugged ROM  
        cli_printf("no arguments given, use: tracer [command], see help");   
        uif_tracer(1); 
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, tracer_cmds[i]);
        i++;
    }

    if (cmd != 0) {
        i = -1;
    }

    if (i >= 0) {
        // cli_printf("argument %s, %d", arg1, i);
        uif_tracer(i);
    }
    else {
        cli_printf("invalid argument %s: use: tracer [command], see help", arg1);    // unknown command
    }

}

void onClearCLI(EmbeddedCli *cli, char *args, void *context) {
    cli_printf("\33[2J");           // clears the screen
    // cli_printf("\x1b[3J");       // clear buffer for TeraTerm
    // cli_printf("\x07");
    // cli_printf("\33[1,1H");          // move the cursor to the top of the screen (TeraTerm)
    
}

void onLed(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);

    int blinks = 5;
    int cmd = -1;
    int num_cmds = sizeof(system_cmds) / sizeof(char *);

    if (arg1 == NULL) {
        // no argument given, default blink is 5*
        // for testing just use the welcome message  
        uif_blink(blinks);
    }
    else
    {
        // check argument for a valid number
        int res = sscanf(arg1, "%d", &blinks);

        if ((res != 1) | (blinks > 9) | (blinks < 0))
        {
            // no valid result
            cli_printf("no valid input, input a number 0..9");
        }
        else
        {
            cli_printf("blinking LED %d times", blinks);
            uif_blink(blinks);
        }
    } 
}


// this routine receives one character from the CLI
void receiveCLIchar()
{
    if(cdc_available(ITF_CONSOLE))
    {
        // data is available in the console so read it
        int c = cdc_read_char(ITF_CONSOLE);
        embeddedCliReceiveChar(cli, c);
    }
}

// send one character to the CLI CDC port
void writeCharToCLI(EmbeddedCli *embeddedCli, char c) 
{
    cdc_send_char(ITF_CONSOLE, c);
    cdc_flush(ITF_CONSOLE);
    tud_task();  // must keep the USB port updated
}

void runCLI()
{
    if ((cli != NULL) && (cdc_connected(ITF_CONSOLE)))
    {
        // only if the CLI is initialized and CDC interface is connected
        receiveCLIchar();
        embeddedCliProcess(cli);
        if (!firstconnect)
        {
            // firstconnect was false, so this is now a new CDC connection
            // display the welcome/status message
            firstconnect = true;
            uif_status();
        }
    }
    else
    // CLI CDC not connected
    {
        firstconnect = false;
    }
}

void initCliBinding() {
    // Define bindings as local variables, so we don't waste static memory

    EmbeddedCliConfig *config = embeddedCliDefaultConfig();
    config->cliBuffer = cliBuffer;
    config->cliBufferSize = CLI_BUFFER_SIZE;
    config->rxBufferSize = CLI_RX_BUFFER_SIZE;
    config->cmdBufferSize = CLI_CMD_BUFFER_SIZE;
    config->historyBufferSize = CLI_HISTORY_SIZE;
    // config->maxBindingCount = CLI_BINDING_COUNT;
    config->maxBindingCount = 12;
    config->invitation = "TULIP> ";
    cli = embeddedCliNew(config);

    if (cli == NULL) {
        printf("\nCli was not created. Check sizes!\n");
        uint16_t size = embeddedCliRequiredSize(config);
        printf(" expected size = %d\n", size);
        // stdio_flush();
        sleep_ms(1000);
        return;
    }

    // binding for the system command
    CliCommandBinding system_binding = {
            .name = "system",
            .help = SYSTEM_HELP_TXT,
            // .help = "system help",
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onSystemCLI
    };    

    // Command binding for the clear command
    CliCommandBinding clear_binding = {
            .name = "clear",
            .help = "Clears the console",
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onClearCLI
    };

    // Command binding for the led command
    CliCommandBinding led_binding = {
            .name = "blink",
            .help = "blink [b], blink the LED b times, just for testing and fun",
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onLed
    };

    // Command binding for the dir command
    CliCommandBinding dir_binding = {
            .name = "dir",
            .help = "dir [subdir], shows uSD card directory",
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onDirCLI
    };

    // Command binding for the sdcard command
    CliCommandBinding sdcard_binding = {
            .name = "sdcard",
            .help = SDCARD_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onSDCardCLI
    };
    
    // Command binding for the plug command
    CliCommandBinding plug_binding = {
            .name = "plug",
            .help = PLUG_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onPlugCLI
    };

    // Command binding for the unplug command
    CliCommandBinding unplug_binding = {
            .name = "unplug",
            .help = UNPLUG_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onUnPlugCLI
    };

    // Command binding for the printer command
    CliCommandBinding printer_binding = {
            .name = "printer",
            .help = PRINTER_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onPrinterCLI
    };

        // Command binding for the printer command
    CliCommandBinding xmem_binding = {
            .name = "xmem",
            .help = XMEM_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onXMEMCLI
    };

    // Command binding for the tracer command
    CliCommandBinding tracer_binding = {
            .name = "tracer",
            .help = TRACER_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onTracerCLI
    };

    // Assign character write function
    cli->writeChar = writeCharToCLI;

    // EmbeddedCli *cli = getCliPointer();
    embeddedCliAddBinding(cli, system_binding);
    embeddedCliAddBinding(cli, clear_binding);
    embeddedCliAddBinding(cli, led_binding);
    embeddedCliAddBinding(cli, dir_binding);
    embeddedCliAddBinding(cli, sdcard_binding);
    embeddedCliAddBinding(cli, plug_binding);
    embeddedCliAddBinding(cli, unplug_binding);
    embeddedCliAddBinding(cli, printer_binding);
    embeddedCliAddBinding(cli, xmem_binding);
    embeddedCliAddBinding(cli, tracer_binding);
}