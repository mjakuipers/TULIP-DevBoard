# TULIP4041 Cheat Sheet

Fast command reference for normal daily use.

## 1) Start Here

- system
- sdcard status
- list
- cat

## 2) ROM Workflow (Most Common)

Import (single file):

- import /roms/myrom.rom
- import /roms/myrom.rom compare
- import /roms/myrom.rom UPDATE
- import /roms/myrom.rom qrom

Import (batch from directory):

- import /roms/ ALL
- import /roms/ compare ALL
- import /roms/ UPDATE ALL

Plug:

- plug myrom.rom
- plug myrom.rom E
- plug myrom.rom 8 2
- plug myrom.rom T

Verify:

- cat
- cat E
- list myrom.rom

Unplug:

- unplug E
- unplug E 1
- unplug all
- unplug ALL

## 3) Storage and Files

- sdcard mount
- sdcard unmount
- sdcard connect
- sdcard eject
- dir /
- dir /roms/
- cd
- cd roms
- cd ..
- cd /
- list
- list flash
- list qrom
- list ext
- list all
- list myrom.rom
- list myrom.rom dump
- export myrom.rom
- export myrom.rom /backup/
- delete myrom.rom

## 4) Built-In Modules

- plug printer
- plug hpil
- plug ilprinter

## 5) Reservations (Avoid Conflicts)

- reserve 7 HPIL
- reserve cx
- reserve timer
- reserve printer
- reserve hpil
- reserve clear 7
- reserve clear all

## 6) Emulation Controls

- emulate
- emulate hpil
- emulate printer
- emulate wand
- emulate zeprom 8

## 7) Tracer Quick Use

- tracer status
- tracer trace
- tracer buffer 5000
- tracer pretrig 64
- tracer sysloop
- tracer sysrom
- tracer ilrom
- tracer hpil
- tracer pilbox
- tracer ilregs
- tracer mnem
- tracer save

Note: changing tracer buffer size may require reboot.

## 8) Printer Emulation

- printer status
- printer power
- printer output
- printer norm
- printer trace
- printer man
- printer print
- printer adv
- printer paper
- printer irtest
- printer irtog

## 9) WAND Quick Use

- wand status
- wand list
- wand test
- wand scan mycodes.wnd
- wand send 123 45 67
- wand sendx 7B 2E 07
- w "PI SIN"
- w "ENTER"
- w 3.14159

## 10) Memory Modules

XMEM:

- xmem status
- xmem 0 0
- xmem 1 2
- xmem dump
- xmem ERASE

UMEM:

- umem status
- umem 4
- umem quad
- umem dump
- umem ERASE

QROM/HEPRAM:

- qrom status
- qrom enable 8
- qrom qrom 8
- qrom qrom 8 2
- qrom wprot 8
- qrom hepram 8
- qrom polling 8
- qrom CLEAR 8 1
- hepram status
- hepram ramtog 8
- hepram reserve 9
- hepram release 9
- hepram CLRAM 8
- hepram INIT
- hepram INITALL

## 11) System Controls

- system status
- system pio
- system cdc
- system cdcident
- system poweron
- system calcreset
- system gpio
- system owner
- system configlist
- system configinit
- system REBOOT
- system BOOTSEL

## 12) RTC (Module Variant Only)

- rtc status
- rtc get
- rtc set
- rtc reset
- rtc dump
- rtc display

## 13) Troubleshooting Quick Checks

No CLI response:

- Check the correct Console CDC port
- system cdc

ROM not showing:

- list
- list flash
- import myrom.rom compare

Tracer empty:

- tracer status
- tracer trace

SD problems:

- sdcard status
- sdcard mount

## 14) Safety Notes

- Do not plug/unplug modules while HP-41 is actively running.
- Do not run FLASH/FRAM low-level maintenance commands unless you are servicing/debugging.
- Do not enable emulation for hardware that is physically present.
- Do not run xmem ERASE or umem ERASE while those modules are in use.
