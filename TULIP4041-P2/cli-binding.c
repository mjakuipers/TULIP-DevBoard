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

#define CLI_BUFFER_SIZE     1500
#define CLI_RX_BUFFER_SIZE  64
#define CLI_CMD_BUFFER_SIZE 128
#define CLI_HISTORY_SIZE    64
#define CLI_BINDING_COUNT   32


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
    embeddedCliPrint(getCliPointer(), buffer);
    tud_task();  // must keep the USB port updated
    return 0;
}

// Function to encapsulate the 'embeddedCliPrint()' call with print formatting arguments (act like printf(), but keeps cursor at correct location).
// The 'embeddedCliPrintN()' function does NOT add a linebreak ('\r\n') to the end of the print statement, so you need to add it yourself.
int cli_printfn(const char *format, ...) {
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
    embeddedCliPrintN(getCliPointer(), buffer);
    tud_task();  // must keep the USB port updated
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
    "configinit",
    "configlist",
    "gpio",
    "owner",     
    #if (TULIP_HARDWARE == T_MODULE)
    "serial",       // program/read the TULIP serial number
    #endif
    
};

/*
        #define help_status     1
        #define help_pio        2
        #define help_cdc        3
        #define help_cdcident   4
        #define help_reboot     5
        #define help_bootsel    6
        #define help_poweron    7
        #define help_calcreset  8
        #define help_configinit 9
        #define help_configlist 10
        #define help_gpio       11
        #define help_owner      12
        #define help_serial     13

*/

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
      case help_status : 
                uif_status();            // status
                break;
      case help_pio : 
                uif_pio_status();        // pio
                break;
      case help_cdc : 
                uif_cdc_status();        // cdc
                break;
      case help_cdcident : 
                uif_cdc_ident();         // cdcident
                break;
      case help_reboot : 
                uif_reboot();            // REBOOT
                break;
      case help_bootsel : 
                uif_bootsel();           // BOOTSEL
                break;
      case help_poweron : 
                uif_poweron();           // poweron
                break;
      case help_calcreset : 
                uif_calcreset();         // calcreset
                break;
      case help_configinit : 
                uif_configinit();        // re-initialize persistent settings
                break;
      case help_configlist : 
                uif_configlist();        // re-initialize persistent settings
                break;            
      case help_serial :                 
                uif_serial(arg2);        // program/read the TULIP serial number
                break;
      case help_gpio : 
                uif_gpio_status();       // show the GPIO status
                break;
        case help_owner :
                uif_owner(arg2);         // program/read the owner of the device
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
    while ((cmd != 0) && (i < num_cmds)) {
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

/*
#define import_no_match        -1
#define import_no_arg           0
#define import_all              1
#define import_update           2
#define import_compare          3
#define import_fram             4
#define import_fram_compare     5
#define import_fram_update      6
*/



/*
#define IMPORT_HELP_TXT "import functions\r\n\
        [filename]                   import a single file to FLASH\r\n\
        [filename]  [compare]        compare a single file with the one in FLASH\r\n\
        [filename]  [UPDATE]         update a single file in FLASH\r\n\
        [directory] [ALL]            import all files in a directory to FLASH\r\n\
        [directory] [compare] [ALL]  compare all files in a directory with FLASH\r\n\
        [directory] [UPDATE] [ALL]   update all files in a directory in FLASH\r\n\
        - import functions for FRAM:\r\n\
        [filename]  [fr]             import a single file in FRAM\r\n\
        [filename]  [compare] [fr]   compare a single file with the one in FRAM\r\n\
        [filename]  [UPDATE] [fr]    update a single file in FRAM\r\n"
*/

const char* __in_flash()import_cmds[] =
// list of arguments for the import command
// import [filename]
{
    "ALL",      // import all files in the directory
    "UPDATE",   // update files in the file sysytem
    "compare",  // compare files in the file system prior to an update
    "q",       // import to FRAM instead of FLASH
};

// the following commans line options are possible:
// import [filename] <FRAM>
// import [filename] <UPDATE> <FRAM>
// import [filename] <COMPARE> <FRAM>
// import [directory] [ALL]
// import [directory] [ALL] <COMPARE>
// import [directory] [ALL] <UPDATE>
//    - import functions for FRAM:
// import [filename] [q]             import a single file in QROM space (FRAM)\r\n\
// import [filename] [q] [compare]   compare a single file with the one in QROM space (FRAM)\r\n\
// import [filename] [q] [UPDATE]    update a single file in QROM space (FRAM)\r\n"

void onImportCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);        // filename or directory
    const char *arg2 = embeddedCliGetToken(args, 2);        // ALL, UPDATE or compare
    const char *arg3 = embeddedCliGetToken(args, 3);        // ALL, UPDATE or compare

    int cmd = -1;
    int num_cmds = sizeof(import_cmds) / sizeof(char *);



    // arg1 is always a filename or directory, this is handled in the uif_import function
    if ((arg1 == NULL)) {
        // no argument given, this does nothing       
        cli_printf("arguments missing, use: import [file/directory] <ALL> <UPDATE/compare>");    
        return;
    }

    if (arg2 == NULL) {
        // no second argument, import a single file, name in arg1
        // this is a single file import, so pass the filename only
        uif_import((char*)arg1, import_no_arg, import_no_arg);     // pass the filename only
        return;
    }

    // scan arg2 for something known, this can be ALL, UPDATE or compare
    int a2 = 0;
    while (cmd != 0 && a2 < num_cmds) {
        cmd = strcmp(arg2, import_cmds[a2]);
        a2++;
    }

    if (cmd != 0) {   // no match in the argument
        a2 = -1;
    }

    // if arg2 is an unknown argument we get out
    if (a2 == -1) {
        // there is an argument in arg2 but it is not valid
        cli_printf("import: unknown 2nd argument %s", arg2);
        return;
    }

    // scan arg3 for something known, this can be ALL, UPDATE or compare
    // but of course only if there is an argument
    int a3 = 0;
    cmd = -1;
    if (arg3 != NULL) {
        while (cmd != 0 && a3 < num_cmds) {
            cmd = strcmp(arg3, import_cmds[a3]);
            a3++;
        }
        if (cmd != 0) {
            // there is an argument in arg3 but it is not valid
            cli_printf("import: unknown 3rd argument %s", arg3);
            return;
        }
    } else {
        // if arg3 == NULL
        a3 = 0;
    }


    // a2 or a3 can be any of the following:
    // #define import_no_arg   0
    // #define import_all      1
    // #define import_update   2
    // #define import_compare  3
    // #define import_qrom     4
    // 
    // and are validated according to the following:
    //   a2        a3       command                                 meaning
    //   0         *        - import [file]                           single file import, handled above
    //   1         0        - import [directory] ALL                  import ALL files in a directory
    //   1         1        - import [directory] ALL ALL              * not valid
    //   1         2        - import [directory] ALL UPDATE           update all files in a directory
    //   1         3        - import [directory] ALL compare          compare all files in a directory
    //   2         0        - import [file] UPDATE                    single file update
    //   2         1        - import [directory] UPDATE ALL           update all files in a directory
    //   2         2        - import [file] UPDATE UPDATE             * not valid
    //   2         3        - import [file] UPDATE compare            * not valid
    //   3         0        - import [file] compare                   single file compare
    //   3         1        - import [directory] compare ALL          compare all files in a directory
    //   3         2        - import [file] compare UPDATE            * not valid
    //   3         3        - import [file] compare compare           * not valid

    // call to uif_import can be the following:
    // a2              a3
    // import_no_arg   import_no_arg
    // import_all      import_no_arg            import [directory] ALL
    // import_update   import_no_arg            import [file] UPDATE
    // import_compare  import_no_arg            import [file] compare
    // import_all      import_update            import [directory] ALL UPDATE
    // import_all      import_compare           import [directory] ALL compare





    if ((a2 == import_all) && (a3 == import_no_arg)) {
        // import [directory] ALL
        uif_import((char*)arg1, import_all, import_no_arg);     // pass the arguments
        return;
    }

    if ((a2 == import_update) && a3 == import_no_arg) {
        // import [file] UPDATE
        uif_import((char*)arg1, import_update, import_no_arg);     // pass the arguments
        return;
    }   

    if ((a2 == import_compare) && (a3 == import_no_arg)) {
        // import [file] compare
        uif_import((char*)arg1, import_compare, import_no_arg);     // pass the arguments
        return;
    }

    if (((a2 == import_all)    && (a3 == import_update)) ||
        ((a2 == import_update) && (a3 == import_all))) {
        // import [directory] ALL UPDATE
        uif_import((char*)arg1, import_all, import_update);     // pass the arguments
        return;
    }

    if (((a2 == import_all)    && (a3 == import_compare)) ||
        ((a2 == import_compare) && (a3 == import_all))) {
        // import [directory] ALL compare
        uif_import((char*)arg1, import_all, import_compare);     // pass the arguments
        return;
    }    

    // if we get here this is an unsupported comnbination of arguments
    cli_printf("arguments not supported, use: import [file/directory] <ALL> <UPDATE/COMPARE>");
}


void onDeleteCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);        // only one argument now
    const char *arg2 = embeddedCliGetToken(args, 2);        // 
    int cmd = -1;
    // int num_cmds = sizeof(delete_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no argument given
        cli_printf("no arguments given, use: delete [filename], see help");   
        return;
    }

    uif_delete((char*)arg1);       // pass the filename
}



const char* __in_flash()plug_cmds[] =
// list of arguments for the plug command
// plug [FLASH/FRAM] [filename]        
{
    "hpil",        // plug the embedded HP-IL ROM in Page 7 and enables emulation
    "ilprinter",   // plug the embedded HP-IL Printer ROM in Page 6
    "printer",     // plug the embedded HP82143A Printer ROM in Page 6 and enables emulation
};


/*
#define PLUG_HELP_TXT "plug functions\r\n\
        hpil          plugs the embedded HP-IL ROM in Page 7 and enables emulation\r\n\
        ilprinter     plugs the embedded HP-IL Printer ROM in Page 6\r\n\
        printer       plugs the embedded HP82143A Printer ROM in Page 6 and enables emulation\r\n\
                      [filenm] can be a .ROM or .MOD file present in FLASH\r\n\
        [filenm] P    plug the named ROM in Page P (hex) and Bank 1\r\n\
        [filenm] P B  plug the named ROM in Page P (hex) and Bank B (1..4)\r\n\
        [filenm]      ROM file: no Page number will autoplug and find a free Page from 8..F\r\n\
                      MOD file: will attempt to plug according to the MOD file parameters\r\n\
        [filenm] T    Autoplug Test only, will not plug for real\r\n\
                      just to check where the ROM/MOD will be plugged\r\n"



        #define plug_hpil       1
        #define plug_ilprinter  2
        #define plug_printer    3
        #define plug_file_X     4   // file in specific Page
        #define plug_file_A     5   // Autoplug file in first free Page
        #define plug_file_T     6   // Autoplug Test only
        #define import_fram             4
        #define import_fram_compare     5
        #define import_fram_update      6
*/  


// plug command, plug a ROM in the system
void onPlugCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);        // file name
    const char *arg2 = embeddedCliGetToken(args, 2);        // Page number in hex
    const char *arg3 = embeddedCliGetToken(args, 3);        // Should be Bank number
    int cmd = -1;
    int num_cmds = sizeof(plug_cmds) / sizeof(char *);
    int arg_topass = 0;

    if ((arg1 == NULL)) {
        // no arguments given, show the plugged ROM as status
        cli_printf("no arguments given, see help");   
        cli_printf("use the cat command to show the plugged ROMs");
        // uif_plug(plug_status, 0, 0, NULL); 
        return;
    }

    // check arg2, could be a Page number, nothing or T
    if (arg2 == NULL) {
        // no Page number or arg2 geven, use AutoPlug
        arg_topass = plug_file_A;  // use the AutoPlug function
    } else if (strcmp(arg2, "T") == 0) {
        // arg2 is T, use the AutoPlug Test function
        arg_topass = plug_file_T;  // use the AutoPlug Test function
    } else {
        // arg2 is something else, probably a Page number in hex, so use the plug_file_X function
        arg_topass = plug_file_X;  // use the plug_file_X function
    }

    // check arg1 for known arguments
    int a1 = 0;
    while (cmd != 0 && a1 < num_cmds) {
        cmd = strcmp(arg1, plug_cmds[a1]);
        a1++;
    }

    if (cmd != 0) {
        a1 = -1;
    }

    if (a1 >= 0) {
        // arg1 is a known command, so execute it
        switch (a1) {
            case plug_hpil : 
                uif_plug(plug_hpil, 7, 1, NULL);        // plug the HP-IL ROM in Page 7
                return;
            case plug_ilprinter : 
                uif_plug(plug_ilprinter, 6, 1, NULL);   // plug the HP-IL Printer ROM in Page 6
                return;
            case plug_printer : 
                uif_plug(plug_printer, 6, 1, NULL);     // plug the HP82143A Printer ROM in Page 6
                return;
            default:
                // proceed with the file name
        }
    }

    // check for Px with the Page number in hex
    if (arg_topass == plug_file_X) {
        // arg2 must be a Page number in hex, so check it
        int p = 0;
        int res = sscanf(arg2, "%X", &p);            // if one decimal is found the res = 1
        if ((res != 1) | (p > 15) | (p < 4)) {
            // no valid result and no valid command, so get out
            cli_printf("invalid Page number, must be 4..F (hex)");    // unknown command
            return;
        } else {
            // p now contains a valid page number, pass this with the ROM file name
            // file name checking is done in the uif_plug function
            // Now check if there is a Bank number given
            if (arg3 != NULL) {
                // arg3 is not NULL, so it must be a Bank number
                int b = 0;
                res = sscanf(arg3, "%d", &b);          // if one decimal is found then res = 1
                if ((res != 1) | (b < 1) | (b > 4)) {
                    // no valid result and no valid command, so get out
                    cli_printf("invalid Bank number, must be 1..4");    // unknown command
                    return;
                }
                // b now contains a valid bank number, pass this with the ROM file name
                uif_plug(arg_topass, p, b, arg1);
                return;
            }
            // no Bank number given, so use Bank 1
            uif_plug(arg_topass, p, 1, arg1);
            return;
        }
    } else {
        // AutoPlug or AutoPlug Test, so no Page number is needed
        uif_plug(arg_topass, 0, 1, arg1);   // pass the file name only,
    }
}

/*
#define UNPLUG_HELP_TXT "unplug functions\r\n\
        [no argument] shows the current plugged ROMs\r\n\
        P             unplug the ROM in Page P (hex), including reserved Pages\r\n\
                      unplug all Banks of that Page\r\n\
        P B           unplug the ROM in Page P and Bank B\r\n\
        all           unplug all plugged ROMs except reserved Pages\r\n\
        ALL           unplug all plugged ROMs including reserved Pages\r\n"

        #define unplug_all     1   // unplug all plugged ROMs except reserved Pages
        #define unplug_ALL     2   // unplug all plugged ROMs including reserved Pages
        // all other values 4..F are a valid Page number to unplug

*/

// unplug command, unplug a ROM from the system
void onUnPlugCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);        // only one argument now
    const char *arg2 = embeddedCliGetToken(args, 2);        // 
    int cmd = -1;
    int num_cmds = sizeof(plug_cmds) / sizeof(char *);
    int bk;

    if ((arg1 == NULL)) {
        // no argument given, show the plugged ROM  
        cli_printf("no arguments given, use: unplug Page <all/ALL>(in hex), see help");
        // uif_unplug(plug_status);  
        return;
    }

    // check for know arguments all or ALL
    if (strcmp(arg1, "all") == 0) {
        // unplug all plugged ROMs except reserved Pages
        uif_unplug(unplug_all, 0);
        return;
    } else if (strcmp(arg1, "ALL") == 0) {
        // unplug all plugged ROMs including reserved Pages
        uif_unplug(unplug_ALL, 0);
        return;
    }   



    // check for Px with a valid Page number in hex
    int p = 0;
    int res = sscanf(arg1, "%X", &p);            // if one decimal is found then res = 1
    if ((res != 1) | (p > 15) | (p < 4)) {
        // no valid result and no valid command, so get out
        cli_printf("invalid Page number, must be >=4 and <=F (hex)", arg2);    // unknown command
        return;
    }
    // p now contains a valid page number

    // check for a valid bank number in arg2
    if (arg2 != NULL) {
        // arg2 is not NULL, so it must be a Bank number
        bk = 0;
        int res = sscanf(arg2, "%d", &bk);          // if one decimal is found then res = 1
        if ((res != 1) | (bk < 1) | (bk > 4)) {
            // no valid result and no valid command, so get out
            cli_printf("invalid Bank number, must be 1..4");    // unknown command
            return;
        }
    } else {
        bk = 0;  // no Bank number given, so use Bank 0, this will unplug all banks in the Page
    }
    // Bank is not used here now
    uif_unplug(p, bk);
}

/*
#define RESERVE_HELP_TXT "reserve a Page for a physical module\r\n\
        [X] <comment> reserve a physical module in Page X\r\n\
                      X is the Page number in hex (0..F)\r\n\
                      comment is a descriptive text for the module\r\n\
                      use no spaces or enclose in quotes\r\n\
        cx            reserve Page 3 and 5 for the HP41CX\r\n\
        timer         reserve Page 5 for the HP41 timer module\r\n\
        printer       reserve Page 6 for a printer\r\n\
        hpil          reserve Page 7 for the HP-IL module\r\n\
        clear <X>     cancel reservation for Page X (in hex)\r\n\
        clear all     cancel all reservations\r\n"

// all values 4..F pass a Page number
#define reserve_cx      1
#define reserve_timer   2       
#define reserve_printer 3
#define reserve_hpil    4
#define reserve_clear   5
#define reserve_page    6   
*/

const char* __in_flash()reserve_cmds[] =
// list of arguments for the system command
{
    "cx",
    "timer",
    "printer",
    "hpil",
    "clear",
};

void onReserveCLI(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);
    int cmd = -1;
    int num_cmds = sizeof(reserve_cmds) / sizeof(char *);
    int p;
    int res;

    if (arg1 == NULL) {
        // no argument given, just show the system status
        // for testing just use the welcome message        
        cli_printf("type help for more info, use the cat command for a list of plugged/reserved ROM images");    
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while ((cmd != 0) && (i < num_cmds)) {
      cmd = strcmp(arg1, reserve_cmds[i]);
      i++;
    }

    if (cmd != 0) 
    {
      i = -1;
    }

    // argument found, now execute
    switch (i) {
        case 1 : uif_reserve(reserve_cx, 0, NULL);           // cx, reserve Pages 3+5
                 return;
        case 2 : uif_reserve(reserve_timer, 0, NULL);        // timer, reserve Page 5
                 return;
        case 3 : uif_reserve(reserve_printer, 0, NULL);      // printer, reserve Page 6
                 return;
        case 4 : uif_reserve(reserve_hpil, 0, NULL);         // hpil, reserve Page 7
                 return;          
        case 5 : // cancel reservation, check for a Page number or all
                 if (arg2 == NULL) {
                    // no Page number given, so show the help
                    cli_printf("no Page number given, use: reserve clear [P] or <all>");
                    return;
                 }

                 if (strcmp(arg2, "all") == 0) {
                     uif_reserve(reserve_clear, 0, NULL);    // cancel all reservations
                     return;
                 }
                 // check for a Page number in hex in arg2
                 p = 0;
                 res = sscanf(arg2, "%X", &p);            // if one decimal is found then res = 1
                 if ((res != 1) | (p > 15) | (p < 4)) {
                    // no valid result and no valid command, so get out
                    cli_printf("invalid Page number, must be >=4 and <=F (hex)", arg2);    // unknown command
                    return;
                 } else {
                    // p now contains a valid page number, cancel the reservation
                     uif_reserve(reserve_clear, p, NULL);        // cancel reservation for Page X
                     return;
                 }

      default:
                 break;
    }

    // if we are here, then this can be a reservation for a Page
    // check for Px with the Page number in hex in arg1
    p = 0;
    res = sscanf(arg1, "%X", &p);            // if one decimal is found then res = 1
    if ((res != 1) | (p > 15) | (p < 4)) {
        // no valid result and no valid command, so get out
        cli_printf("invalid Page number, must be >=4 and <=F (hex)", arg1);    // unknown command
        return;
    }
    // p now contains a valid page number, pass this with the comment
    // if arg2 is NULL, then no comment is given, so pass NULL
    uif_reserve(reserve_page, p, arg2);       // reserve Page p with comment in arg2

}

void onCatCLI(EmbeddedCli *cli, char *args, void *context)
{
    // check for a hex argument in the range 4..F
    const char *arg1 = embeddedCliGetToken(args, 1);        // only one argument now
    const char *arg2 = embeddedCliGetToken(args, 2);        // could be Bank
    int cmd = -1;
    int num_cmds = sizeof(plug_cmds) / sizeof(char *);
    if ((arg1 == NULL)) {
        // no argument given, show all plugged ROMs
        cli_printf("no arguments given, use: cat Page (in hex), see help");   
        uif_cat(0, 0);           // show a summary of the plugged ROMs
        return;
    }
    // check for Px with the Page number in hex
    int p = 0;
    int res = sscanf(arg1, "%X", &p);            // if one decimal is found then res = 1
    if ((res != 1) | (p > 15) | (p < 4)) {
        // no valid result and no valid command, so get out
        cli_printf("invalid Page number, must be >=4 and <=F (hex)", arg1);    // invalid Page
        return;
    }

    // now check for a Bank number as 2nd argument
    int b = 1;
    if (arg2 != NULL) {
        // arg2 is not NULL, so it must be a Bank number
        res = sscanf(arg2, "%d", &b);          // if one decimal is found then res = 1
        if ((res != 1) | (b < 1) | (b > 4)) {
            // no valid result and no valid command, so get out
            cli_printf("invalid Bank number, must be 1..4", arg2);    // invalid Bank
            return;
        }
    } else {
        // no Bank number given, so use Bank 1
        b = 1;
    }
    // p now contains a valid page number, pass this with the ROM file name
    uif_cat(p, b);   
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
    "irtest",           // test the infrared LED
    "irtog",            // toggle infrared LED
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
    "PATTERN",          // program test pattern in FRAM
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
        buffer        shows the trace buffer size\r\n\
        buffer <size> set the tracer buffer size in number of samples\r\n\
                      default is 5000, maximum is about 10.000 samples\r\n\
                      requires a reboot to take effect!\r\n\
        pretrig      shows the pre-trigger buffer size and status\r\n\
        pretrig <size> set the pre-trigger buffer size in number of samples\r\n\
                      default is 32, maximum is 256 samples\r\n\
        trace         toggle tracer enable/disable\r\n\
        sysloop       toggle tracing of system loops (RSTKB, RST05, BLINK01 and debounce)\r\n\
        sysrom        toggle system rom tracing (Page 0 - 5)\r\n\
        ilrom         toggle tracing of Page 6+7\r\n\
        hpil          toggle HP-IL tracing to ILSCOPE USB serial port\r\n\
        pilbox        toggle PILBox serial tracing to ILSCOPE USB serial port\r\n\
        ilregs        toggle tracing of HP-IL registers\r\n\
        save          save tracer settings\r\n"

        #define trace_status      1
        #define trace_buffer      2
        #define trace_pretrig     3
        #define trace_trace       4
        #define trace_sysloop     5    
        #define trace_sysrom      6
        #define trace_ilrom       7
        #define trace_hpil        8
        #define trace_pilbox      9
        #define trace_ilregs     10
        #define trace_save       11
*/

const char* __in_flash()tracer_cmds[] =
// list of arguments for the tracer command
// 
{
    "status",           // get status
    "buffer",           // get/set tracer buffer size
    "pretrig",          // get/set pre-trigger buffer size
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
    const char *arg2 = embeddedCliGetToken(args, 2);        // start address
    // const char *arg3 = embeddedCliGetToken(args, 3);        // end address
    int cmd = -1;
    int num_cmds = sizeof(tracer_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no argument given, show the plugged ROM  
        cli_printf("no arguments given, use: tracer [command], see help");   
        uif_tracer(1, 0); 
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

    // check for arguments buffer and pretrig
    if (i == trace_buffer) {
        // buffer command, check for a size in arg2
        if (arg2 == NULL) {
            // no size given, just show the current buffer size
            uif_tracer(i, 0);
            return;
        } else {
            // check for a valid number in arg2
            int size = 0;
            int res = sscanf(arg2, "%d", &size);          // if one decimal is found then res = 1
            if ((res != 1) | (size < 100) | (size > 10000)) {
                // no valid result and no valid command, so get out
                cli_printf("invalid buffer size %s: must be between 100 and 10000", arg2);    // unknown command
                return;
            }
            uif_tracer(i, size);   // pass the new buffer size
            return;
        }
    } else if (i == trace_pretrig) {
        // pretrig command, check for a size in arg2
        if (arg2 == NULL) {
            // no size given, just show the current pre-trigger buffer size
            uif_tracer(i, 0);
            return;
        } else {
            // check for a valid number in arg2
            int size = 0;
            int res = sscanf(arg2, "%d", &size);          // if one decimal is found then res = 1
            if ((res != 1) | (size < 1) | (size > 256)) {
                // no valid result and no valid command, so get out
                cli_printf("invalid pre-trigger size %s: must be between 1 and 256", arg2);    // unknown command
                return;
            }
            uif_tracer(i, size);   // pass the new pre-trigger size
            return;
        }
    }

    if (i >= 0) {
        // cli_printf("argument %s, %d", arg1, i);
        uif_tracer(i, 0);
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
            cli_printf("no valid input, input a number 1..9");
        }
        else
        {
            uif_blink(blinks);
        }
    } 
}

const char* __in_flash()flash_cmds[] =
// list of arguments for the flash command
// 
{
    "status",           // get status
    "dump",             // dump FLASH contents
    "INIT",             // initialize FLASH file system
    "NUKEALL",          // erase all FLASH pages
};

void onFlashCLI(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);

    int cmd = -1;
    uint32_t addr = 0;
    int num_cmds = sizeof(flash_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no argument given, show status
        cli_printf("no arguments given, use: flash [command], see help");   
        uif_flash(1,0); 
        return;
    }

    if (arg2 != NULL) {
        // there is an arg2, used for a hex address in dump
        int res = sscanf(arg2, "%x", &addr);
        if (res != 1) {
            cli_printf("invalid address %s: address defaults to 0", arg2);    // invalid input
            addr = 0;
        }
    }

    if (arg2 == NULL) {
        addr = 0x40414243;      
    }   

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, flash_cmds[i]);
        i++;
    }

    if (cmd != 0) {
        i = -1;
    }

    if (i >= 0) {
        uif_flash(i, addr);
    }
    else {
        cli_printf("invalid argument %s: use: flash [command], see help", arg1);    // unknown command
    }
}


const char* __in_flash()fram_cmds[] =
// list of arguments for the flash command
// 
{
    "status",           // get status
    "dump",             // dump FLASH contents
    "INIT",             // initialize FRAM file system (old command, use newinit)
    "NUKEALL",          // erase all FRAM pages
    // "newinit",          // initialize FRAM file system
    // "list",             // list all files in FRAM
};

void onFramCLI(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);

    int cmd = -1;
    uint32_t addr = 0;
    int num_cmds = sizeof(fram_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no argument given, show status
        cli_printf("no arguments given, use: fram [command], see help");   
        uif_fram(1,0); 
        return;
    }

    if (arg2 != NULL) {
        // there is an arg2, used for a hex address in dump
        int res = sscanf(arg2, "%x", &addr);
        if (res != 1) {
            cli_printf("invalid address %s: address defaults to 0", arg2);    // invalid input
            addr = 0;
        }
    }

    if (arg2 == NULL) {
        addr = 0x40414243;      
    }   

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, fram_cmds[i]);
        i++;
    }

    if (cmd != 0) {
        i = -1;
    }

    if (i >= 0) {
        uif_fram(i, addr);
    }
    else {
        cli_printf("invalid argument %s: use: fram [command], see help", arg1);    // unknown command
    }
}


const char* __in_flash()list_cmds[] =
// list of arguments for the list command, but none used
// 
// the following command line options are possible:
// list [filename]      shows details of one file, or the first file starting with the filename
// list <no argument>   shows all files in the file system
// list <flash>/<fram>  shows all files in either FRAM or FLASH only
// list <ext>           extended listing with more details per file
// list <all>           list all files, including erased and dummy files

{
    "all",             // list all files, including erased and dummy files
    "ext",             // extended listing with more details per file
    "flash",           // list all files in FLASH
    // "fram",            // list all files in FRAM
};

void onListCLI(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);

    int cmd = -1;
    uint32_t addr = 0;
    int num_cmds = sizeof(list_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no arguments given, show all files
        uif_list(-1, NULL); 
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, list_cmds[i]);
        i++;
    }
    
    if (cmd != 0) {                 // no valid argument found
        i = -1;
    }

    if (i >= 0) {
        uif_list(i, arg2);          // valid argument, arg2 may contain a string
    }

    if ((i == -1) && (arg2 == NULL)) {
        // no valid argument, assume arg1 is a filename
        uif_list(-1, arg1);         // no valid argument, assume it is a filename
    }

    if ((i == -1) && (arg2 != NULL)) {
        // no valid argument, and a filename is given
        // but we never get here, an invalid argument is seen as a filename to list
        cli_printf("invalid argument %s: use: list [all/ext/flash/fram] [filename]", arg1);    // unknown command	
    }

}

/*
#define RTC_HELP_TXT "RTC test functions\r\n\
        [no argument] shows the RTC status\r\n\
        status        shows the RTC status\r\n\
        set           set the RTC to the given date and time\r\n\
        get           get the current date and time from the RTC\r\n\
        test          test the RTC functions\r\n\
        dump          dump the RTC registers\r\n"

#define rtc_status      1
#define rtc_set         2
#define rtc_get         3    
#define rtc_reset       4
#define rtc_dump        5
#define rtc_display     6
*/


const char* __in_flash()rtc_cmds[] =
// list of arguments for the RTC command
// 
{
    "status",           // get status
    "set",              // set RTC date and time
    "get",              // get RTC date and time
    "reset",            // test RTC functions
    "dump",             // dump RTC registers
    "display",          // test the SSD1315 display
};  

void onRTCCLI(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);

    int cmd = -1;
    int num_cmds = sizeof(rtc_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no arguments given, show status
        cli_printf("no arguments given, use: rtc [command], see help");
        uif_rtc(rtc_status, NULL); 
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, rtc_cmds[i]);
        i++;
    }
    
    if (cmd != 0) {                 // no valid argument found
        i = -1;
    }

    if (i >= 0) {
        uif_rtc(i, arg2);          // valid argument, arg2 may contain a string for the set functions
                                    // just pass the string and let the uif_rtc function do the rest
        return;
    }

    if (i == -1) {
        // no valid argument
        cli_printf("invalid argument %s, see help", arg1);    // unknown command	
    }
}

/*
#define emulate_status    1
#define emulate_hpil      2
#define emulate_printer   3
#define emulate_zeprom    4
#define emulate_wand      5
#define emulate_xmem      6
#define emulate_blinky    7
#define emulate_timer     8

*/

const char* __in_flash()emulate_cmds[] =
// list of arguments for the RTC command
// 
{
    "status",        
    "hpil",          
    "printer",
    "zeprom",    
    "wand"      

};  

void onEmulateCLI(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);

    int cmd = -1;
    int num_cmds = sizeof(emulate_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no arguments given, show status
        cli_printf("no arguments given, use: emulate [device], see help");
        uif_emulate(emulate_status, 0); 
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, emulate_cmds[i]);
        i++;
    }
    
    if (cmd != 0) {                 // no valid argument found
        i = -1;
    }

    int p = 0;
    if (i == emulate_zeprom) {
        // emulate zeprom, check for a valid Page number in arg2
        if (arg2 == NULL) {
            // no address given, use 0 to disable
            uif_emulate(i, p);      // default address
            return;
        } else {
            // check for a valid Page address in arg2

            int res = sscanf(arg2, "%X", &p);            // if one decimal is found then res = 1
            if ((res != 1) | (p < 4) | (p > 15)) {
                // no valid result and no valid command, so get out
                cli_printf("invalid Page %s, must be between 4 and F (hex)", arg2);    // unknown command
                return;
            }
            uif_emulate(i, p);       // pass the address
            return;
        }
    }

    if (i >= 0) {
        uif_emulate(i, 0);          // valid argument
        return;
    }

    if (i == -1) {
        // no valid argument
        cli_printf("invalid argument %s, see help", arg1);    // unknown command	
    }
}

/*
#define wand_status    1
#define wand_test      2
#define wand_scan      3
#define wand_list      4
#define wand_send      5


#define WAND_HELP_TXT "wand functions\r\n\
        [no argument]     shows the wand status\r\n\
        status            shows the wand status\r\n\
        scan [filename]   scan the named file \r\n\
        test              performs a wand test sequence, use with WNDTST\r\n"
*/


const char* __in_flash()wand_cmds[] =
// list of arguments for the RTC command
// 
{
    "status",        
    "test",          
    "scan",
    "list",     
    "send",  
};  

void onWandCLI(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);
    const char *arg3 = embeddedCliGetToken(args, 3);
    const char *arg4 = embeddedCliGetToken(args, 4);
    const char *arg5 = embeddedCliGetToken(args, 5);
    const char *arg6 = embeddedCliGetToken(args, 6);
    const char *arg7 = embeddedCliGetToken(args, 7);
    const char *arg8 = embeddedCliGetToken(args, 8);
    const char *arg9 = embeddedCliGetToken(args, 9);

    char instruction[100];

    int cmd = -1;
    int num_cmds = sizeof(wand_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no arguments given, show status
        cli_printf("no arguments given, use: emulate [device], see help");
        uif_wand(wand_status, 0, NULL); 
        return;
    }

    // scan the list of arguments for something known
    int i = 0;
    while (cmd != 0 && i < num_cmds) {
        cmd = strcmp(arg1, wand_cmds[i]);
        i++;
    }
    
    if (cmd != 0) {                 // no valid argument found
        i = -1;
    }

    int p = 0;
    if (i == wand_scan) {
        // emulate zeprom, check for a valid Page number in arg2
        if (arg2 == NULL) {
            // no filename given, show error
            cli_printf("no filename given, use: wand scan [filename");
            return;
        } 
        uif_wand(i, arg2, NULL);       // pass the filename
        return;
    }

    instruction[0] = '\0';

    if ((i == wand_send) && (arg2 != NULL)) {
        // create a new string with the extra arguments after arg1 to pass as one instruction
        snprintf(instruction, CLI_CMD_BUFFER_SIZE, "%s %s %s %s %s %s %s %s", arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }


    if (i >= 0) {
        uif_wand(i, arg2, instruction);              // valid argument
        return;
    }

    if (i == -1) {
        // no valid argument
        cli_printf("invalid argument %s, see help", arg1);    // unknown command	
    }
}

// Wand Paper Keyboard emulation

void onWCLI(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);

    int cmd = -1;
    int num_cmds = sizeof(wand_cmds) / sizeof(char *);

    if ((arg1 == NULL)) {
        // no arguments given, show status
        cli_printf("no arguments given, use: w [HP41 instruction], see documentation");
        return;
    }

    uif_w(arg1);       // pass the instruction

}

// this routine receives one character from the CLI
void receiveCLIchar()
{
    if(cdc_available(ITF_CONSOLE))
    {
        // data is available in the console so read it
        // gpio_put(ONBOARD_LED, true); 
        int c = cdc_read_char(ITF_CONSOLE);
        // tud_task();  // must keep the USB port updated
        embeddedCliReceiveChar(cli, c);
        // gpio_put(ONBOARD_LED, false);
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
    config->maxBindingCount = CLI_BINDING_COUNT;
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
            .help = "clears the console\n",
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onClearCLI
    };

    // Command binding for the led command
    CliCommandBinding led_binding = {
            .name = "blink",
            .help = "blink [b], blink the LED b times, just for testing and fun, 0 toggles the LED status\n",
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onLed
    };

    // Command binding for the dir command
    CliCommandBinding dir_binding = {
            .name = "dir",
            .help = "dir [subdir], shows uSD card directory\n",
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

    // Command binding for the plug command
    CliCommandBinding reserve_binding = {
            .name = "reserve",
            .help = RESERVE_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onReserveCLI
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

    // Command binding for the flash command
    CliCommandBinding flash_binding = {
            .name = "flash",
            .help = FLASH_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onFlashCLI
    };

    // Command binding for the fram command
    CliCommandBinding fram_binding = {
            .name = "fram",
            .help = FRAM_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onFramCLI
    };
    // Command binding for the import command
    CliCommandBinding import_binding = {
            .name = "import",
            .help = IMPORT_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onImportCLI
    };

    // Command binding for the list command
    CliCommandBinding list_binding = {
            .name = "list",
            .help = LIST_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onListCLI
    };

    // Command binding for the rtc command
    CliCommandBinding rtc_binding = {
            .name = "rtc",
            .help = RTC_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onRTCCLI
    };

    // Command binding for the cat command
    CliCommandBinding cat_binding = {
            .name = "cat",
            .help = CAT_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onCatCLI
    };

    // Command binding for the emulate command
    CliCommandBinding emulate_binding = {
            .name = "emulate",
            .help = EMULATE_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onEmulateCLI
    };

    // Command binding for the wand command
    CliCommandBinding wand_binding = {
            .name = "wand",
            .help = WAND_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onWandCLI
    };

    CliCommandBinding w_binding = {
            .name = "w",
            .help = W_HELP_TXT,
            .tokenizeArgs = false,
            .context = NULL,
            .binding = onWCLI
    };

    // Command binding for the delete command
    CliCommandBinding delete_binding = {
            .name = "delete",
            .help = DELETE_HELP_TXT,
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onDeleteCLI
    };

    // Assign character write function
    cli->writeChar = writeCharToCLI;

    // EmbeddedCli *cli = getCliPointer();
    embeddedCliAddBinding(cli, system_binding);
    embeddedCliAddBinding(cli, sdcard_binding);   

    #if (TULIP_HARDWARE == T_MODULE)
        // RTC support only on the TULIP module version
        embeddedCliAddBinding(cli, rtc_binding);
    #endif  

    embeddedCliAddBinding(cli, clear_binding);
    embeddedCliAddBinding(cli, led_binding);
    embeddedCliAddBinding(cli, printer_binding);
    embeddedCliAddBinding(cli, tracer_binding);    
    embeddedCliAddBinding(cli, xmem_binding);    
    embeddedCliAddBinding(cli, flash_binding);    
    embeddedCliAddBinding(cli, fram_binding);    

    embeddedCliAddBinding(cli, dir_binding);
    embeddedCliAddBinding(cli, list_binding);
    embeddedCliAddBinding(cli, delete_binding);  
    embeddedCliAddBinding(cli, import_binding);

    embeddedCliAddBinding(cli, plug_binding);
    embeddedCliAddBinding(cli, unplug_binding);
    embeddedCliAddBinding(cli, reserve_binding);

    embeddedCliAddBinding(cli, cat_binding);
    embeddedCliAddBinding(cli, emulate_binding);
    embeddedCliAddBinding(cli, wand_binding);
    embeddedCliAddBinding(cli, w_binding);

}