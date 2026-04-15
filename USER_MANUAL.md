# TULIP4041 User Manual

Version: 0.98b (software)

## 1. Overview

TULIP4041 is a Raspberry Pi Pico 2 (RP2350) based intelligent peripheral for HP-41 calculators.
It combines:

- HP-41 peripheral and ROM emulation
- ROM storage and plug/unplug management
- Multi-port USB CDC communication
- Real-time tracer and diagnostic output
- Optional printer, HP-IL, and WAND emulation

Supported calculator family:

- HP-41C
- HP-41CV
- HP-41CX
- MAXX

## 2. Hardware Variants

Two firmware targets are provided:

- DevBoard: development-friendly hardware mapping
- Module: integrated hardware variant for module use

The firmware detects power/source conditions and adapts behavior (for example clocking and power mode reporting).

## 3. USB Ports

The firmware exposes multiple USB CDC interfaces.

- Console: primary command-line interface (CLI)
- Tracer: instruction/bus trace stream
- HP-IL: HP-IL related communication stream
- IL Scope: HP-IL tracing/analysis stream
- Printer: printer text stream

Use the Console port for all commands in this manual.

## 4. First-Time Quick Start

1. Connect TULIP4041 and the HP-41 hardware.
2. Connect USB to your PC.
3. Open a serial terminal on the Console CDC port.
4. Run:
   - system
   - sdcard status
   - list
   - cat
5. If needed, mount storage:
   - sdcard mount
6. Test ROM plug without changing runtime state:
   - plug myrom.rom T
7. Plug ROM for actual use:
   - plug myrom.rom E
8. Verify current mapping:
   - cat

## 5. Safety and Operational Rules

Read this section before normal use.

- Do not use flash or fram maintenance commands unless you intentionally do low-level servicing.
- Do not plug or unplug modules while the calculator is actively running.
- Do not enable emulated peripherals when the same physical module is present.
- Do not erase UMEM or XMEM while those modules are in active use.
- Use caution with BOOTSEL/REBOOT actions during active sessions.

## 6. Core Command Groups

### 6.1 System

- system: complete status overview
- system status: detailed status
- system pio: PIO state machine status
- system cdc: CDC link status
- system cdcident: identify CDC interfaces
- system poweron: wake operation (when applicable)
- system calcreset: calculator reset signal action
- system gpio: show all GPIO states
- system owner: program or read the device owner string
- system REBOOT: reboot (with confirmation window)
- system BOOTSEL: enter UF2 bootloader mode
- system configlist: list persistent settings
- system configinit: reset configuration defaults

### 6.2 SD Card and Files

- sdcard status
- sdcard mount
- sdcard unmount
- sdcard connect
- sdcard eject
- dir /
- dir /path/

### 6.3 ROM Library and Slot Mapping

- list: list all files in FLASH and QROM/FRAM
- list flash: list only FLASH files
- list qrom: list only QROM/FRAM files
- list ext: extended listing with more detail per file
- list all: include erased files in the listing
- list <file>: show details of a named file
- list <file> dump: details plus hexdump
- cat
- cat <page>
- cat <page> <bank>

Plugging:

- plug <file>
- plug <file> <page>
- plug <file> <page> <bank>
- plug <file> T   (test only)
- plug printer
- plug hpil
- plug ilprinter

Unplugging:

- unplug <page>
- unplug <page> <bank>
- unplug all
- unplug ALL

Import/export/delete:

Single file:

- import <file>: import file to FLASH
- import <file> compare: compare file with stored version
- import <file> UPDATE: update stored version from SD card
- import <file> qrom: import file into QROM/FRAM space

Batch (directory):

- import <dir> ALL: import all files from a directory to FLASH
- import <dir> compare ALL: compare all files in directory
- import <dir> UPDATE ALL: update all files from directory

Export/delete:

- export <file>: export file from FLASH/FRAM to SD card root
- export <file> <dir>: export to named directory on SD card
- delete <file>

### 6.4 Reservation Management

Reserve pages to avoid conflicts with physical hardware or intended layout.

- reserve <page> <description>
- reserve cx
- reserve timer
- reserve printer
- reserve hpil
- reserve clear <page>

### 6.5 Emulation Controls

- emulate
- emulate status
- emulate hpil
- emulate printer
- emulate wand
- emulate zeprom <page>

### 6.6 Printer Emulation

- printer
- printer status
- printer power
- printer output: cycle output mode (none / serial / IR / both)
- printer trace
- printer norm
- printer man
- printer paper
- printer print
- printer adv
- printer irtest
- printer irtog

### 6.7 Tracer

- tracer
- tracer status
- tracer trace: toggle tracer on/off
- tracer buffer: show current trace buffer size
- tracer buffer <size>: set trace buffer size (100–10000 samples)
- tracer pretrig: show pre-trigger buffer size
- tracer pretrig <size>: set pre-trigger buffer size (1–256 samples)
- tracer sysloop: toggle tracing of system loops (RSTKB, RST05, BLINK01, debounce)
- tracer sysrom: toggle tracing of system ROM pages 0–5
- tracer ilrom: toggle tracing of pages 6 and 7
- tracer hpil: toggle HP-IL tracing to IL Scope port
- tracer pilbox: toggle PILBox serial tracing to IL Scope port
- tracer ilregs: toggle tracing of HP-IL registers
- tracer mnem: cycle disassembly type (none / JDA / HP mnemonics)
- tracer save: save tracer settings to persistent storage

Note: buffer-size changes require a reboot to take effect.

### 6.8 Memory Extensions

XMEM:

- xmem status
- xmem <0..2>
- xmem dump
- xmem ERASE

UMEM:

- umem status
- umem <0..4>: plug 0–4 User Memory modules
- umem quad: plug a Quad Memory module
- umem dump: dump User Memory contents
- umem ERASE

QROM and HEPAX-related control:

- qrom status: show QROM status overview
- qrom enable <page>: toggle read-enable of the named page
- qrom qrom <page> <bank>: toggle QROM status of file in page/bank
- qrom wprot <page>: toggle write protection for the named page
- qrom hepram <page>: toggle HEPAX RAM status for the named page
- qrom polling <page>: toggle polling-interrupt enable for the named page
- qrom CLEAR <page> <bank>: clear QROM in named page and bank
- hepram status
- hepram ramtog <page>: toggle write-protect bit for named page
- hepram reserve <page>: exclude page from the HEPRAM chain
- hepram release <page>: include page back in the HEPRAM chain
- hepram CLRAM <page>: clear HEPRAM for named page
- hepram INIT: initialize HEPRAM chain (erases all non-reserved HEPRAM)
- hepram INITALL: initialize all HEPRAM including reserved pages

### 6.9 RTC (Module Variant Only)

The RTC command is only available on the TULIP Module hardware variant.

- rtc status: show RTC status
- rtc get: read current date and time from RTC
- rtc set <datetime>: set RTC date and time
- rtc reset: reset RTC to defaults
- rtc dump: dump RTC registers
- rtc display: test the SSD1315 display

### 6.10 WAND Functions

- wand status
- wand scan <file>
- wand list
- wand test
- wand send <codes>
- wand sendx <codes>
- w <instruction>

Examples:

- w "PI SIN"
- w "ENTER"
- w 3.14159

## 7. Typical Workflows

### 7.1 Add and Plug a ROM

1. Ensure SD card is mounted.
2. import /roms/myapp.rom
3. list myapp.rom
4. plug myapp.rom E
5. cat

### 7.2 Safe Replace of Existing ROM

1. import /roms/myapp.rom compare
2. import /roms/myapp.rom UPDATE
3. unplug E
4. plug myapp.rom E
5. cat E

### 7.3 Enable and Tune Tracing

1. tracer status
2. tracer buffer 5000
3. tracer pretrig 64
4. tracer trace
5. tracer save
6. system REBOOT (if requested by status/help)

## 8. Troubleshooting

### 8.1 No CLI response

- Check the correct USB CDC Console port.
- Run system cdc from any responding port.
- Reconnect USB and reopen terminal.

### 8.2 ROM not visible after import

- Run list and list flash.
- Verify path and filename extension.
- Use import <file> compare to validate source/target match.

### 8.3 Plug command fails or unexpected behavior

- Verify page reservation with reserve/listing commands.
- Confirm calculator is not in an active conflicting state.
- Avoid page conflicts with physical modules.

### 8.4 Tracer output missing

- Run tracer status.
- Ensure tracer trace is enabled.
- Confirm host app is connected to the Tracer/IL Scope port.

### 8.5 SD card issues

- Run sdcard status.
- Use sdcard mount.
- Check card formatting and seating.
- Prefer reliable card media and clean FAT/exFAT format.

## 9. Maintenance and Recovery

- system configlist: inspect persistent options
- system configinit: reset persistent configuration
- system REBOOT: controlled restart
- system BOOTSEL: firmware update mode (UF2)

Use flash and fram maintenance commands only for advanced servicing or development.

## 10. Notes

- Behavior can differ slightly between DevBoard and Module builds.
- Some advanced/debug command families are intended for service and development workflows.
- Keep backups of important ROM libraries before bulk operations.

---

End of manual.
