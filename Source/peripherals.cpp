/*
 * peripherals.c
 *
 * This file is part of the TULIP4041 project.
 * Copyright (C) 2024 Meindert Kuipers
 * 
 * This file contains the higher level emulation of peripherals:
 *  - IR printing
 *  - HP-IL communication and PILBox emulation
 *  - IO definitions and initialization
 *
 * This is free software: you are free to change and redistribute it.
 * 
 * HP-IL emulation is based on the V41 sources by Christoph Giesselink en
 * EMU41 by Jean-Francois Garnier
 * PILBox emulation is based on the PILBox project by Jean-Francois Garnier
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * USE AT YOUR OWN RISK
 *
 */

#include "peripherals.h"

static uint16_t PrintChar;          // character received from printbuffer
static char PrintLine[200];         // printbuffer to print single lines for Serial Printer Emulation
static int PrintLineLen = 0;        // length of the current line in the printbuffer
static int PrintBufLineLen = 0;     // length of the current line in the printbuffer

const HP82143AChar __in_flash()HP82143A_CHAR_MAP_UTF8[128] = {
    
    {.data = {0xE2, 0x97, 0x86}, .length = 3},              // 0x00: Diamond ◆
    {.data = {0x78},             .length = 1},              // 0x01: Small X ✕
    {.data = {0x78, 0xCC, 0x85}, .length = 3},              // 0x02: X bar above (x with combining overline) x̅
    {.data = {0xE2, 0x86, 0x90}, .length = 3},              // 0x03: Left arrow ←
    {.data = {0xCE, 0xB1},       .length = 2},              // 0x04: Alpha α
    {.data = {0xCE, 0xB2},       .length = 2},              // 0x05: Beta β
    {.data = {0xCE, 0xB3},       .length = 2},              // 0x06: Gamma γ
    {.data = {0xE2, 0x86, 0x93}, .length = 3},              // 0x07: Down arrow ↓
    {.data = {0xCE, 0xB4},       .length = 2},              // 0x08: Delta δ
    {.data = {0xCF, 0x83},       .length = 2},              // 0x09: Small sigma σ
    {.data = {0xE2, 0x97, 0x86}, .length = 3},              // 0x0A: Diamond ◆
    {.data = {0xCE, 0xBB},       .length = 2},              // 0x0B: Lambda λ
    {.data = {0xCE, 0xBC},       .length = 2},              // 0x0C: Mu μ
    {.data = {0xE2, 0x88, 0xA0}, .length = 3},              // 0x0D: Angle ∠
    {.data = {0xCF, 0x84},       .length = 2},              // 0x0E: Tau τ
    {.data = {0xCF, 0x86},       .length = 2},              // 0x0F: Phi φ
    
    {.data = {0xCE, 0xB8},       .length = 2},              // 0x10: Theta θ
    {.data = {0xCE, 0xA9},       .length = 2},              // 0x11: Omega Ω
    {.data = {0xCE, 0xB4},       .length = 2},              // 0x12: Small delta δ
    {.data = {0xC3, 0x85},       .length = 2},              // 0x13: A with ring Å
    {.data = {0xC3, 0xA5},       .length = 2},              // 0x14: a with ring å
    {.data = {0xC3, 0x84},       .length = 2},              // 0x15: A with diaeresis Ä
    {.data = {0xC3, 0xA4},       .length = 2},              // 0x16: a with diaeresis ä
    {.data = {0xC3, 0x9B},       .length = 2},              // 0x17: U with circumflex Û
    {.data = {0xC3, 0x96},       .length = 2},              // 0x18: O with diaeresis Ö
    {.data = {0xC3, 0x9C},       .length = 2},              // 0x19: U with diaeresis Ü
    {.data = {0xC3, 0xBC},       .length = 2},              // 0x1A: u with diaeresis ü
    {.data = {0xC3, 0x86},       .length = 2},              // 0x1B: AE ligature Æ
    {.data = {0xC3, 0xA6},       .length = 2},              // 0x1C: ae ligature æ
    {.data = {0xE2, 0x89, 0xA0}, .length = 3},              // 0x1D: Not equal sign ≠
    {.data = {0xC2, 0xA3},       .length = 2},              // 0x1E: Pound sign £
    {.data = {0xE2, 0x88, 0x97}, .length = 3},              // 0x1F: Starburst ✱ (or asterisk operator ∗)

    {.data = {0x20}, .length = 1},                          // 0x20: Space 
    {.data = {0x21}, .length = 1},                          // 0x21: !  
    {.data = {0x22}, .length = 1},                          // 0x22: "
    {.data = {0x23}, .length = 1},                          // 0x23: #
    {.data = {0x24}, .length = 1},                          // 0x24: $
    {.data = {0x25}, .length = 1},                          // 0x25: %
    {.data = {0x26}, .length = 1},                          // 0x26: &
    {.data = {0x27}, .length = 1},                          // 0x27: '
    {.data = {0x28}, .length = 1},                          // 0x28: (
    {.data = {0x29}, .length = 1},                          // 0x29: )
    {.data = {0x2A}, .length = 1},                          // 0x2A: *
    {.data = {0x2B}, .length = 1},                          // 0x2B: +    
    {.data = {0x2C}, .length = 1},                          // 0x2C: ,
    {.data = {0x2D}, .length = 1},                          // 0x2D: -
    {.data = {0x2E}, .length = 1},                          // 0x2E: .
    {.data = {0x2F}, .length = 1},                          // 0x2F: /
 
    
    {.data = {0x30}, .length = 1},                          // 0x30: 0
    {.data = {0x31}, .length = 1},                          // 0x31: 1
    {.data = {0x32}, .length = 1},                          // 0x32: 2
    {.data = {0x33}, .length = 1},                          // 0x33: 3
    {.data = {0x34}, .length = 1},                          // 0x34: 4
    {.data = {0x35}, .length = 1},                          // 0x35: 5
    {.data = {0x36}, .length = 1},                          // 0x36: 6
    {.data = {0x37}, .length = 1},                          // 0x37: 7
    {.data = {0x38}, .length = 1},                          // 0x38: 8
    {.data = {0x39}, .length = 1},                          // 0x39: 9
    {.data = {0x3A}, .length = 1},                          // 0x3A: :
    {.data = {0x3B}, .length = 1},                          // 0x3B: ;
    {.data = {0x3C}, .length = 1},                          // 0x3C: <
    {.data = {0x3D}, .length = 1},                          // 0x3D: =
    {.data = {0x3E}, .length = 1},                          // 0x3E: >
    {.data = {0x3F}, .length = 1},                          // 0x3F: ?
    
    {.data = {0x40}, .length = 1},                          // 0x40: @
    {.data = {0x41}, .length = 1},                          // 0x41: A
    {.data = {0x42}, .length = 1},                          // 0x42: B
    {.data = {0x43}, .length = 1},                          // 0x43: C
    {.data = {0x44}, .length = 1},                          // 0x44: D
    {.data = {0x45}, .length = 1},                          // 0x45: E
    {.data = {0x46}, .length = 1},                          // 0x46: F
    {.data = {0x47}, .length = 1},                          // 0x47: G
    {.data = {0x48}, .length = 1},                          // 0x48: H
    {.data = {0x49}, .length = 1},                          // 0x49: I
    {.data = {0x4A}, .length = 1},                          // 0x4A: J
    {.data = {0x4B}, .length = 1},                          // 0x4B: K
    {.data = {0x4C}, .length = 1},                          // 0x4C: L
    {.data = {0x4D}, .length = 1},                          // 0x4D: M
    {.data = {0x4E}, .length = 1},                          // 0x4E: N
    {.data = {0x4F}, .length = 1},                          // 0x4F: O
    
    {.data = {0x50}, .length = 1},                          // 0x50: P
    {.data = {0x51}, .length = 1},                          // 0x51: Q
    {.data = {0x52}, .length = 1},                          // 0x52: R
    {.data = {0x53}, .length = 1},                          // 0x53: S
    {.data = {0x54}, .length = 1},                          // 0x54: T
    {.data = {0x55}, .length = 1},                          // 0x55: U
    {.data = {0x56}, .length = 1},                          // 0x56: V
    {.data = {0x57}, .length = 1},                          // 0x57: W
    {.data = {0x58}, .length = 1},                          // 0x58: X
    {.data = {0x59}, .length = 1},                          // 0x59: Y
    {.data = {0x5A}, .length = 1},                          // 0x5A: Z
    {.data = {0x5B}, .length = 1},                          // 0x5B: [
    {.data = {0x5C}, .length = 1},                          // 0x5C: backslash
    {.data = {0x5D}, .length = 1},                          // 0x5D: ]
    {.data = {0x5E}, .length = 1},                          // 0x5E: ^ (caret, shown as circumflex)
    {.data = {0x5F}, .length = 1},                          // 0x5F: _ (underscore) 
    
    {.data = {0x60}, .length = 1},                          // 0x60: ` (grave accent, shown as backtick/grave)
    {.data = {0x61}, .length = 1},                          // 0x62: a
    {.data = {0x62}, .length = 1},                          // 0x62: b
    {.data = {0x63}, .length = 1},                          // 0x63: c
    {.data = {0x64}, .length = 1},                          // 0x64: d
    {.data = {0x65}, .length = 1},                          // 0x65: e
    {.data = {0x66}, .length = 1},                          // 0x66: f
    {.data = {0x67}, .length = 1},                          // 0x67: g
    {.data = {0x68}, .length = 1},                          // 0x68: h
    {.data = {0x69}, .length = 1},                          // 0x69: i
    {.data = {0x6A}, .length = 1},                          // 0x6A: j
    {.data = {0x6B}, .length = 1},                          // 0x6B: k
    {.data = {0x6C}, .length = 1},                          // 0x6C: l
    {.data = {0x6D}, .length = 1},                          // 0x6D: m
    {.data = {0x6E}, .length = 1},                          // 0x6E: n
    {.data = {0x6F}, .length = 1},                          // 0x6F: o
    
    {.data = {0x70}, .length = 1},                          // 0x70: p
    {.data = {0x71}, .length = 1},                          // 0x71: q
    {.data = {0x72}, .length = 1},                          // 0x72: r
    {.data = {0x73}, .length = 1},                          // 0x73: s
    {.data = {0x74}, .length = 1},                          // 0x74: t
    {.data = {0x75}, .length = 1},                          // 0x75: u
    {.data = {0x76}, .length = 1},                          // 0x76: v
    {.data = {0x77}, .length = 1},                          // 0x77: w
    {.data = {0x78}, .length = 1},                          // 0x78: x
    {.data = {0x79}, .length = 1},                          // 0x79: y
    {.data = {0x7A}, .length = 1},                          // 0x7A: z
    {.data = {0xCF, 0x80},           .length = 2},          // 0x7B: Pi π
    {.data = {0x7C},                 .length = 1},          // 0x7C: | (pipe/vertical bar)
    {.data = {0xE2, 0x86, 0x92},     .length = 3},          // 0x7D: Right arrow →
    {.data = {0xCE, 0xA3},           .length = 2},          // 0x7E: Sigma Σ
    {.data = {0xCB, 0xAB},           .length = 2}           // 0x7F: Append/Sleeping
};


const HP82143AChar __in_flash()HP82143A_CHAR_MAP_ASCII[128] = {
    {.data = {0x2A}, .length = 1},                          // 0x00: Diamond ◆, shown as *
    {.data = {0x78}, .length = 1},                          // 0x01: Small X, shown as x
    {.data = {0x2E}, .length = 1},                          // 0x02: X bar above, shown as .
    {.data = {0x3C}, .length = 1},                          // 0x03: Left arrow ←, shown as <
    {.data = {0x2E}, .length = 1},                          // 0x04: Alpha α, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x05: Beta β, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x06: Gamma γ, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x07: Down arrow ↓, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x08: Delta δ, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x09: Small sigma σ, shown as .
    {.data = {0x2A}, .length = 1},                          // 0x0A: Diamond ◆, shown as *
    {.data = {0x2E}, .length = 1},                          // 0x0B: Lambda λ, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x0C: Mu μ, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x0D: Angle ∠, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x0E: Tau τ, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x0F: Phi φ, shown as .
    
    {.data = {0x2E}, .length = 1},                          // 0x10: Theta θ, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x11: Omega Ω, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x12: Small delta δ, shown as .
    {.data = {0x2E}, .length = 1},                          // 0x13: A with ring Å
    {.data = {0x2E}, .length = 1},                          // 0x14: a with ring å
    {.data = {0x2E}, .length = 1},                          // 0x15: A with diaeresis Ä
    {.data = {0x2E}, .length = 1},                          // 0x16: a with diaeresis ä
    {.data = {0x2E}, .length = 1},                          // 0x17: U with circumflex Û
    {.data = {0x2E}, .length = 1},                          // 0x18: O with diaeresis Ö
    {.data = {0x2E}, .length = 1},                          // 0x19: U with diaeresis Ü
    {.data = {0x2E}, .length = 1},                          // 0x1A: u with diaeresis ü
    {.data = {0x2E}, .length = 1},                          // 0x1B: AE ligature Æ
    {.data = {0x2E}, .length = 1},                          // 0x1C: ae ligature æ
    {.data = {0x23}, .length = 1},                          // 0x1D: Not equal sign ≠, shown #
    {.data = {0x2E}, .length = 1},                          // 0x1E: Pound sign £, shown as £
    {.data = {0x23}, .length = 1},                          // 0x1F: Starburst, shown as #

    {.data = {0x20}, .length = 1},                          // 0x20: Space 
    {.data = {0x21}, .length = 1},                          // 0x21: !  
    {.data = {0x22}, .length = 1},                          // 0x22: "
    {.data = {0x23}, .length = 1},                          // 0x23: #
    {.data = {0x24}, .length = 1},                          // 0x24: $
    {.data = {0x25}, .length = 1},                          // 0x25: %
    {.data = {0x26}, .length = 1},                          // 0x26: &
    {.data = {0x27}, .length = 1},                          // 0x27: '
    {.data = {0x28}, .length = 1},                          // 0x28: (
    {.data = {0x29}, .length = 1},                          // 0x29: )
    {.data = {0x2A}, .length = 1},                          // 0x2A: *
    {.data = {0x2B}, .length = 1},                          // 0x2B: +    
    {.data = {0x2C}, .length = 1},                          // 0x2C: ,
    {.data = {0x2D}, .length = 1},                          // 0x2D: -
    {.data = {0x2E}, .length = 1},                          // 0x2E: .
    {.data = {0x2F}, .length = 1},                          // 0x2F: /

    {.data = {0x30}, .length = 1},                          // 0x30: 0
    {.data = {0x31}, .length = 1},                          // 0x31: 1
    {.data = {0x32}, .length = 1},                          // 0x32: 2
    {.data = {0x33}, .length = 1},                          // 0x33: 3
    {.data = {0x34}, .length = 1},                          // 0x34: 4
    {.data = {0x35}, .length = 1},                          // 0x35: 5
    {.data = {0x36}, .length = 1},                          // 0x36: 6
    {.data = {0x37}, .length = 1},                          // 0x37: 7
    {.data = {0x38}, .length = 1},                          // 0x38: 8
    {.data = {0x39}, .length = 1},                          // 0x39: 9
    {.data = {0x3A}, .length = 1},                          // 0x3A: :
    {.data = {0x3B}, .length = 1},                          // 0x3B: ;
    {.data = {0x3C}, .length = 1},                          // 0x3C: <
    {.data = {0x3D}, .length = 1},                          // 0x3D: =
    {.data = {0x3E}, .length = 1},                          // 0x3E: >
    {.data = {0x3F}, .length = 1},                          // 0x3F: ?

    {.data = {0x40}, .length = 1},                          // 0x40: @
    {.data = {0x41}, .length = 1},                          // 0x41: A
    {.data = {0x42}, .length = 1},                          // 0x42: B
    {.data = {0x43}, .length = 1},                          // 0x43: C
    {.data = {0x44}, .length = 1},                          // 0x44: D
    {.data = {0x45}, .length = 1},                          // 0x45: E
    {.data = {0x46}, .length = 1},                          // 0x46: F
    {.data = {0x47}, .length = 1},                          // 0x47: G
    {.data = {0x48}, .length = 1},                          // 0x48: H
    {.data = {0x49}, .length = 1},                          // 0x49: I
    {.data = {0x4A}, .length = 1},                          // 0x4A: J
    {.data = {0x4B}, .length = 1},                          // 0x4B: K
    {.data = {0x4C}, .length = 1},                          // 0x4C: L
    {.data = {0x4D}, .length = 1},                          // 0x4D: M
    {.data = {0x4E}, .length = 1},                          // 0x4E: N
    {.data = {0x4F}, .length = 1},                          // 0x4F: O

    {.data = {0x50}, .length = 1},                          // 0x50: P
    {.data = {0x51}, .length = 1},                          // 0x51: Q
    {.data = {0x52}, .length = 1},                          // 0x52: R
    {.data = {0x53}, .length = 1},                          // 0x53: S
    {.data = {0x54}, .length = 1},                          // 0x54: T
    {.data = {0x55}, .length = 1},                          // 0x55: U
    {.data = {0x56}, .length = 1},                          // 0x56: V
    {.data = {0x57}, .length = 1},                          // 0x57: W
    {.data = {0x58}, .length = 1},                          // 0x58: X
    {.data = {0x59}, .length = 1},                          // 0x59: Y
    {.data = {0x5A}, .length = 1},                          // 0x5A: Z
    {.data = {0x5B}, .length = 1},                          // 0x5B: [
    {.data = {0x5C}, .length = 1},                          // 0x5C: \ backslash
    {.data = {0x5D}, .length = 1},                          // 0x5D: ]
    {.data = {0x5E}, .length = 1},                          // 0x5E: up arrow ^ (caret, shown as circumflex)
    {.data = {0x5F}, .length = 1},                          // 0x5F: _ (underscore) 
        
    {.data = {0x60}, .length = 1},                          // 0x60: ` (grave accent, shown as backtick/grave)
    {.data = {0x61}, .length = 1},                          // 0x61: a
    {.data = {0x62}, .length = 1},                          // 0x62: b
    {.data = {0x63}, .length = 1},                          // 0x63: c
    {.data = {0x64}, .length = 1},                          // 0x64: d
    {.data = {0x65}, .length = 1},                          // 0x65: e
    {.data = {0x66}, .length = 1},                          // 0x66: f
    {.data = {0x67}, .length = 1},                          // 0x67: g
    {.data = {0x68}, .length = 1},                          // 0x68: h
    {.data = {0x69}, .length = 1},                          // 0x69: i
    {.data = {0x6A}, .length = 1},                          // 0x6A: j
    {.data = {0x6B}, .length = 1},                          // 0x6B: k
    {.data = {0x6C}, .length = 1},                          // 0x6C: l
    {.data = {0x6D}, .length = 1},                          // 0x6D: m
    {.data = {0x6E}, .length = 1},                          // 0x6E: n
    {.data = {0x6F}, .length = 1},                          // 0x6F: o

    {.data = {0x70}, .length = 1},                          // 0x70: p
    {.data = {0x71}, .length = 1},                          // 0x71: q
    {.data = {0x72}, .length = 1},                          // 0x72: r
    {.data = {0x73}, .length = 1},                          // 0x73: s
    {.data = {0x74}, .length = 1},                          // 0x74: t
    {.data = {0x75}, .length = 1},                          // 0x75: u
    {.data = {0x76}, .length = 1},                          // 0x76: v
    {.data = {0x77}, .length = 1},                          // 0x77: w
    {.data = {0x78}, .length = 1},                          // 0x78: x
    {.data = {0x79}, .length = 1},                          // 0x79: y
    {.data = {0x7A}, .length = 1},                          // 0x7A: z
    {.data = {0x2E}, .length = 1},                          // 0x7B: pi (shown as .)
    {.data = {0x7C}, .length = 1},                          // 0x7C: |
    {.data = {0x3E}, .length = 1},                          // 0x7D: right arrow (shown as >)
    {.data = {0x53}, .length = 1},                          // 0x7E: sigma (shown as S)
    {.data = {0x27}, .length = 1},                          // 0x7F: hanging t (shown as single quote ')
};


/*
 * Helper function to send a character to output
 * Handles both single-byte and multi-byte UTF-8 characters

inline void hp82143a_send_char(uint8_t hp_code, int (*putchar_func)(int)) {
    const HP82143AChar* ch = &HP82143A_CHAR_MAP_UTF8[hp_code];
    for (int i = 0; i < ch->length; i++) {
        putchar_func(ch->data[i]);
    }
}
*/

/*
  HP82143 charmap must be mapped to regular serial terminal output, no fancy stuff
    special cases:
    printer  char               ascii (dec)   ascii (hex)   char
    -------  --------           -----------   -----------  -------
    0x00     diamond            207           0xCF         general currency sign
    0x01     small x            158           0x9E         small x
    0x02     x bar above        158           0x9E         small x
    0x03     left arrow         174           0xAE         left arrow
    0x04     alpha              166           0xA6         alpha
    0x05     beta               225           0xE1         beta
    0x06     gamma              218           0xDA         gamma
    0x07     down arrow         207           0xCF         currency sign
    0x08     delta              68            0x44         D
    0x09     small sigma        207           0xCF  
    0x0A     diamond            207           0xCF         diamond
    0x0B     lambda             207           0xCF         lambda
    0x0C     mu                 230           0xE6         mu
    0x0D     angle              207           0xCF         angle
    0x0E     tau                207           0xCF         tau
    0x0F     phi                207           0xCF         phi

    0x10     theta              207           0xCF         theta
    0x11     Omega              207           0xCF         Omega
    0x12     small delta        100           0x64         d
    0x13     A dot Å            143           0x8F         A with dot above
    0x14     a dot å            134           0x86         a with dot above
    0x15     A 2 dots Ä         142           0x8E         A with diaeresis
    0x16     a 2 dots ä         132           0x84         a with diaeresis
    0x17     U circumflex Û     234           0xEA         U with circumflex
    0x18     O double dot Ö     153           0x99         O with diaeresis
    0x19     U double dot Ü     154           0x9A         U with diaeresis
    0x1A     u double dot ü     129           0x81         u with diaeresis
    0x1B     AE ligature Æ      146           0x92         AE ligature
    0x1C     ae ligature æ      145           0x91         ae ligature
    0x1D     not equal sign #    35           0x23         #
    0x1E     pound sign £       156           0x9C         pound sign
    0x1F     starburst          177           0xB1         starburst

    0x20..0x7F     regular ASCII characters, but with the special cases above

    0x60    hanging t           96            0x60         `  
    0x7B    pi                                0xF4         ¶
    0x7C    vertical bar                                   | 
    0x7D    right arrow        175            0xAF         » 
    0x7E    sigma               83            0x53         S
    0x7F    append, sleeping T 195            0xC3         ├        

    0xA1..0xB7, skip 1..23 characters, insert spaces
    0xE0    End of Line Left Justify, empty buffer EOLL
    0xE8    End of Line Right Justify, add spaces at start of string end empty buffer EOLR

*/




static uint16_t ir_code;
static uint32_t ir_frame;

static bool irframe_pending = false;   // true when there is an IR frame ready to be sent, but the PIO is still busy sending the previous frame
static bool serial_pending = false;    // true when there is a serial character ready to be sent
static bool char_pending = false;      // a char was read from the printbuffer, but not completely sent yet

queue_t PrintBuffer;            // printbuffer between cores
const int PrintBufSize = 100;   // size of printbuffer

queue_t ILscopeBuffer;           // buffer for IL Scope, to decouple the HP-IL communication from the printing of the frames
const int ILscopeBufSize = 100;  // size of ILscope buffer

bool ILScope_send_pending = false;           // flag to indicate that there is a frame or char pending to be sent, but the USB buffer was full, so we have to wait until it is empty before sending

bool HPIL_firstconnect = false;
bool ILScope_firstconnect = false;
bool Print_firstconnect = false;
bool ilscope_enabled = true;

// HP-IL variables
const int HPIL_BufSize = 10;        // size of HP-IL buffer for both sending and receiving

bool HPIL_closed    = false;        // indicates if the HP-IL loop is closed at the HP82160A
                                    // this is like the cables on the HP-IL module are connected together without any device
bool PILBox_closed  = false;        // indicates if the HP-IL loop is closed at the HP82160A
                                    // loop is closed by default on the real PILBox if it was not initialized
bool m_bLoopClosed  = true;         // loop is closed  
bool enable_AUTOIDY = false;        // if system is in AUTO IDY mode, default when power is applied

uint16_t PIL_rx_lo;                 // PILBox lo byte previously received   
uint16_t PIL_rx_hi;                 // PILBox hi byte previously received
uint16_t PIL_tx_lo;                 // PILBox lo byte previously sent   
uint16_t PIL_tx_hi;                 // PILBox hi byte previously sent
uint16_t PIL_rx_frame;              // PILBox frame just received
uint16_t PIL_rx_prevframe;          // PILBox previous frame received
uint16_t PIL_tx_prevframe;          // PILBox previous frame sent

uint16_t PIL_CMD_frame = 0;         // PILBox CMD frame last sent
uint16_t PIL_FRC_frame = 0;         // PILBox RFC frame last sent

bool PIL_rx_pending = false;        // true is a lo byte is read but not yet the hi byte
bool PIL_tx_pending = false;        // true if the hi byte still has to be sent
bool PILmode8 = true;               // PILBox transfer mode, true when in 8-bit mode, false when in 7-bit mode
uint16_t PILBox_mode = TDIS;        // PILBOx mode (TDIS, CON, COFF, COFI)
uint16_t PILBox_prevmode = 0;       // PILBOx mode to detect a change (TDIS, CON, COFF, COFI)

ILScope_line IL_Line;               // struct for the IL Scope queue, to decouple the HP-IL communication from the printing of the frames
ILScope_line ILScopeLine;           // struct for the IL Scope queue, to decouple the HP-IL communication from the printing of the frames

bool ILScope_Overflow = false;      // becomes true when the ILScope buffer is full and lines are lost

extern uint8_t HP41_powermode;

// enum MODE {eNone = 0, eController = 1 , eDevice = 2};
// enum MODE m_eMode = eNone;          // controller/device mode

#define m_eNone 0
#define m_eController 1
#define m_eDevice 2

int m_eMode = 0;

uint16_t m_wLastFrame;              // last sent frame
uint16_t m_wLastCmd;                // last CMD frame
uint16_t loopbackFrame = 0xFFFF;    // used for loopback frames for PILBox emulation      

volatile bool HPIL_timeout = false;
absolute_time_t t_IDY_timer;                // 10 ms timer for sending AUTO IDY frames

// forward declaration
void HPIL_SendFrame(uint16_t wFrame);

// for the HP-IL Scope
char  ILScopePrint[200];
int   ILScopePrintLen = 0;

struct ILScope_struct {
    int16_t ILcode;
    int16_t ILmask;
    const char* ILmnemonic;
} ; 


// const ILScope_struct __in_flash() IL_mnemonics[] = 
const ILScope_struct IL_mnemonics[] = 
// list of HP-IL mnemonics
// forced in FLASH to maximize SRAM
{   // opcode, mask, mnemonic
    {0x000, 0x700, "DAB"},      // DATA frame        // element #0
    {0x100, 0x700, "DSR"},
    {0x200, 0x700, "END"},      // End Byte
    {0x300, 0x700, "ESR"},
    {0x400, 0x7FF, "NUL"},      // NULL
    {0x401, 0x7FF, "GTL"},      // Go To Local
    {0x404, 0x7FF, "SDC"},      // Selected Device Clear
    {0x405, 0x7FF, "PPD"},      // Parellel Poll Disable
    {0x408, 0x7FF, "GET"},      // Group Execute Trigger
    {0x40F, 0x7FF, "ELN"},      // Enable Listener Not Readey For Data
    {0x410, 0x7FF, "NOP"},      // No Operation        // element #10
    {0x411, 0x7FF, "LLO"},      // Local Lock Out
    {0x414, 0x7FF, "DCL"},      // Device Clear
    {0x415, 0x7FF, "PPU"},      // Parallel Poll Unconfigure
    {0x418, 0x7FF, "EAR"},      // Enable Asynchronous Request
    {0x43F, 0x7FF, "UNL"},      // Unlisten
    {0x420, 0x7E0, "LAD"},      // Listener Address
    {0x45F, 0x7FF, "UNT"},      // Untalk
    {0x440, 0x7E0, "TAD"},      // Talker Address
    {0x460, 0x7E0, "SAD"},      // Seconday Address
    {0x480, 0x7F0, "PPE"},      // Parellel Poll Enable        // element #20
    {0x490, 0x7FF, "IFC"},      // Interface Clear
    {0x492, 0x7FF, "REN"},      // Remote Enable
    {0x493, 0x7FF, "NRE"},      // Not Remote Enable
    {0x494, 0x7FF, "*TDIS"},    // PILBox Translator Disable
    {0x495, 0x7FF, "*COFI"},    // PILBox Controller
    {0x496, 0x7FF, "*CON"},     // PILBox Controller On
    {0x497, 0x7FF, "*COFF"},    // PILBox Controller Off with IDY
    {0x49A, 0x7FF, "AAU"},      // Auto Address Unconfigure
    {0x49B, 0x7FF, "LPD"},      // Loop Power Down
    {0x4A0, 0x7E0, "DDL"},      // Device Dependent Listener Command         // element #30
    {0x4C0, 0x7E0, "DDT"},      // Device Dependent Talker Command
    {0x400, 0x700, "CMD"},      // All commands filter 0x400-0x4C0
    {0x500, 0x7FF, "RFC"},      // Ready For Command
    {0x540, 0x7FF, "ETO"},      // End Of Transmission, OK
    {0x541, 0x7FF, "ETE"},      // End Of Transmission, Error
    {0x542, 0x7FF, "NRD"},      // Not Ready For Data
    {0x560, 0x7FF, "SDA"},      // Send Data
    {0x561, 0x7FF, "SST"},      // SSP, Send Serial Poll ??
    {0x562, 0x7FF, "SDI"},      // Send Device ID
    {0x563, 0x7FF, "SAI"},      // Send Accessory ID        // element #40
    {0x564, 0x7FF, "TCT"},      // Take Control
    {0x580, 0x7E0, "AAD"},      // Auto Address 0-30
    {0x5A0, 0x7E0, "AEP"},      // Auto Extended Primary
    {0x5C0, 0x7E0, "AES"},      // Auto Extended Secondary
    {0x5E0, 0x7E0, "AMP"},      // Auto Multiple Primary
    {0x500, 0x700, "RDY"},      // Ready
    {0x600, 0x700, "IDY"},      // Identify
    {0x700, 0x700, "ISR"},      //                          //  element #48
    };



// General functions for GPIO control

// function to toggle a GPIO signal
void gpio_toggle(uint signal)
{
    gpio_put(signal, !gpio_get(signal));
}

// function to toggle a GPIO signal multiple times, mainly for debugging
void gpio_pulse(uint signal, uint numtimes)
{
    uint i;
    for (i = 0 ; i < numtimes ; i ++)
    {
       gpio_toggle(signal);
       gpio_toggle(signal);
    }    
} 


// initialize the UART0 on pins 0 and 1
void serialport_init()
{
    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);
    uart_set_hw_flow(UART_ID, false, false);

    // Set the TX and RX pins by using the function select on the GPIO
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Send out a string, with CR/LF conversions
    uart_puts(UART_ID, " UART initialized!\n");
    // Send out a character without any conversions
    // uart_putc_raw(UART_ID, 'A');

    // Send out a character but do CR/LF conversions
    // uart_putc(UART_ID, 'B');

}

void bus_init()
{
    gpio_init(ONBOARD_LED);

    gpio_set_dir(ONBOARD_LED, GPIO_OUT);
    gpio_put(ONBOARD_LED, 1);               // light the led

    //  HP41 inputs
    gpio_init(P_CLK1);
    gpio_set_dir(P_CLK1, GPIO_IN);
    gpio_init(P_CLK2);
    gpio_set_dir(P_CLK2, GPIO_IN);
    gpio_init(P_SYNC);
    gpio_set_dir(P_SYNC, GPIO_IN);
    gpio_init(P_ISA);
    gpio_set_dir(P_ISA, GPIO_IN);
    gpio_init(P_DATA);
    gpio_set_dir(P_DATA, GPIO_IN);
    gpio_init(P_PWO);
    gpio_set_dir(P_PWO, GPIO_IN);

    gpio_init(P_FI);                    // FI input on GPIO2, on DevBoard only!
    gpio_set_dir(P_FI, GPIO_IN);

    // helper outputs
    gpio_init(P_T0_TIME);
    gpio_set_dir(P_T0_TIME, GPIO_OUT);
    gpio_init(P_DEBUG);
    gpio_set_dir(P_DEBUG, GPIO_OUT);
    gpio_init(P_SYNC_TIME);
    gpio_set_dir(P_SYNC_TIME, GPIO_OUT);

    gpio_init(P_DATA_OE);
    gpio_set_dir(P_DATA_OE, GPIO_OUT);
    gpio_put(P_DATA_OE, 0);                 // set to low to disable output driver

    gpio_init(P_FI_OE);    
    gpio_set_dir(P_FI_OE, GPIO_OUT);
    // gpio_set_pulls(P_FI_OE, false, true);   // enable pullup, but not needed
    gpio_put(P_FI_OE, 0);                   // set to low to disable output driver

    gpio_init(P_ISA_OE);  
    gpio_set_dir(P_ISA_OE, GPIO_OUT);
    gpio_put(P_ISA, 0);                     // set to low to disable output driver

    gpio_init(P_IR_LED);  
    gpio_set_dir(P_IR_LED, GPIO_OUT);       // IR led and PWO_OE output driver
    gpio_set_drive_strength(P_IR_LED, GPIO_DRIVE_STRENGTH_12MA);
    gpio_put(P_IR_LED, 0);                  // set low

    gpio_init(PICO_VBUS_PIN);
    gpio_set_dir(PICO_VBUS_PIN, GPIO_IN);
    
    #if (TULIP_HARDWARE == T_MODULE)
        // PWO_OE is shared with SPARE1 on the module version
        gpio_init(P_PWO_OE);  
        gpio_set_dir(P_PWO_OE, GPIO_OUT);
        gpio_put(P_PWO_OE, 0);               // set to low to disable output driver
    #endif


    //prepare the IRQ for the PWO rising and falling edge
    gpio_set_irq_enabled_with_callback(P_PWO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &pwo_callback);
}

void WandBuffer_init()
{
    queue_init(&WandBuffer, sizeof(uint16_t), WandBufSize);
}


// functions for IR output

// Calculate Frame Payload with Parity
// from https://github.com/vogelchr/avr-redeye/blob/master/avr-redeye.c
   
   /*   And one frame consists of <start> 12*(<one>|<zero>), the order for
    the 12 payload bits in a frame is:

          d c b a 7 6 5 4 3 2 1 0

   7..0 are data, a,b,c,d are parity bits, so that, if the frame payload
   is written as a 12-bit integer the following subsets of bits must have
   a even numbers of bits set:

   +---+---+---+---++---+---+---+---++---+---+---+---++--------+
   : d : c : b : a :: 7 : 6 : 5 : 4 :: 3 : 2 : 1 : 0 ::   Hex  :
   +---+---+---+---++---+---+---+---++---+---+---+---++--------+
   : X :   :   :   ::   : X : X : X :: X :   :   :   :: 0x0878 :
   +---+---+---+---++---+---+---+---++---+---+---+---++--------+
   :   : X :   :   :: X : X : X :   ::   : X : X :   :: 0x04e6 :
   +---+---+---+---++---+---+---+---++---+---+---+---++--------+
   :   :   : X :   :: X : X :   : X ::   : X :   : X :: 0x02d5 :
   +---+---+---+---++---+---+---+---++---+---+---+---++--------+
   :   :   :   : X :: X :   :   :   :: X :   : X : X :: 0x018b :
   +---+---+---+---++---+---+---+---++---+---+---+---++--------+

   */


/*
char parity(uint8_t byte)
{
	byte = byte ^ (byte >> 4); // merge 7..4 -> 3..0 
	byte = byte ^ (byte >> 2); // merge 3..2 -> 1..0 
	byte = byte ^ (byte >> 1); // merge    1 ->    0 
	return byte & 0x01;
}

static uint16_t calculate_frame_payload_slow(uint8_t data)
{
    int16_t frame = data;
    if (parity(data & 0x78)) frame |= 0x800;
    if (parity(data & 0xe6)) frame |= 0x400;
    if (parity(data & 0xd5)) frame |= 0x200;
    if (parity(data & 0x8b)) frame |= 0x100;
    return frame;
}

static uint32_t construct_frame_slow(uint16_t data)
{
    uint32_t frame = 0b111;                 // start bits

    for (int i = 0; i < 12 ; i++) {
        // go through all 12 bits and construct new frame
        // a 0 bit expands to 10
        // a 1 bit expands to 01
        frame = frame << 2;
        if ((data & 0x800) == 0x800) {
            // msb is set
            frame = frame | 0x02;             // OR with 0b10
        } else {
            // msb is not set
            frame = frame | 0x01;             // OR with 0b01
        }
        data = data << 1;
    }

    // frame now contains the following for character A for example:
    // 0000.0111.1010.0110.0110.0101.0101.0110     or hex 0x07A66556
    //       ^^^ start bits
    //           ^^^........................^^  payload 24 bits
    // this must now be left aligned for proper sending, else the first 0's are sent
    // ensure in the state machine that only 24+3=27 bits are transitted !
    frame = frame << 5;
    return frame;
}
*/

/*
// array below generated by AI, but not used
static const uint16_t __in_flash("flash_constants") BYTE_PAYLOAD_LUT[256] = {
    0x000, 0x301, 0x502, 0x603, 0x604, 0x505, 0x306, 0x007, 0x908, 0xA09, 0xC0A, 0xF0B, 0xF0C, 0xC0D, 0xA0E, 0x90F,     // 0x00..0x0F
    0xA10, 0x911, 0xF12, 0xC13, 0xC14, 0xF15, 0x916, 0xA17, 0x318, 0x019, 0x61A, 0x51B, 0x51C, 0x61D, 0x01E, 0x31F,     // 0x10..0x1F
    0xC20, 0xF21, 0x922, 0xA23, 0xA24, 0x925, 0xF26, 0xC27, 0x528, 0x629, 0x02A, 0x32B, 0x32C, 0x02D, 0x62E, 0x52F,     // 0x20..0x2F
    0x630, 0x531, 0x332, 0x033, 0x034, 0x335, 0x536, 0x637, 0xF38, 0xC39, 0xA3A, 0x93B, 0x93C, 0xA3D, 0xC3E, 0xF3F,     // 0x30..0x3F, ascii 01234567890
    0xE40, 0xD41, 0xB42, 0x843, 0x844, 0xB45, 0xD46, 0xE47, 0x748, 0x449, 0x24A, 0x14B, 0x14C, 0x24D, 0x44E, 0x74F,     // 0x40..0x4F, ascii @ABCDEFGHIJKLMNO
    0x450, 0x751, 0x152, 0x253, 0x254, 0x155, 0x756, 0x457, 0xD58, 0xE59, 0x85A, 0xB5B, 0xB5C, 0x85D, 0xE5E, 0xD5F,     // 0x50..0x5F
    0x260, 0x161, 0x762, 0x463, 0x464, 0x765, 0x166, 0x267, 0xB68, 0x869, 0xE6A, 0xD6B, 0xD6C, 0xE6D, 0x86E, 0xB6F,
    0x870, 0xB71, 0xD72, 0xE73, 0xE74, 0xD75, 0xB76, 0x877, 0x178, 0x279, 0x47A, 0x77B, 0x77C, 0x47D, 0x27E, 0x17F,
    0x780, 0x481, 0x282, 0x183, 0x184, 0x285, 0x486, 0x787, 0xE88, 0xD89, 0xB8A, 0x88B, 0x88C, 0xB8D, 0xD8E, 0xE8F,
    0xD90, 0xE91, 0x892, 0xB93, 0xB94, 0x895, 0xE96, 0xD97, 0x498, 0x799, 0x19A, 0x29B, 0x29C, 0x19D, 0x79E, 0x49F,
    0xBA0, 0x8A1, 0xEA2, 0xDA3, 0xDA4, 0xEA5, 0x8A6, 0xBA7, 0x2A8, 0x1A9, 0x7AA, 0x4AB, 0x4AC, 0x7AD, 0x1AE, 0x2AF,
    0x1B0, 0x2B1, 0x4B2, 0x7B3, 0x7B4, 0x4B5, 0x2B6, 0x1B7, 0x8B8, 0xBB9, 0xDBA, 0xEBB, 0xEBC, 0xDBD, 0xBBE, 0x8BF,
    0x9C0, 0xAC1, 0xCC2, 0xFC3, 0xFC4, 0xCC5, 0xAC6, 0x9C7, 0x0C8, 0x3C9, 0x5CA, 0x6CB, 0x6CC, 0x5CD, 0x3CE, 0x0CF,
    0x3D0, 0x0D1, 0x6D2, 0x5D3, 0x5D4, 0x6D5, 0x0D6, 0x3D7, 0xAD8, 0x9D9, 0xFDA, 0xCDB, 0xCDC, 0xFDD, 0x9DE, 0xADF,
    0x5E0, 0x6E1, 0x0E2, 0x3E3, 0x3E4, 0x0E5, 0x6E6, 0x5E7, 0xCE8, 0xFE9, 0x9EA, 0xAEB, 0xAEC, 0x9ED, 0xFEE, 0xCEF,
    0xFF0, 0xCF1, 0xAF2, 0x9F3, 0x9F4, 0xAF5, 0xCF6, 0xFF7, 0x6F8, 0x5F9, 0x3FA, 0x0FB, 0x0FC, 0x3FD, 0x5FE, 0x6FF
};
*/


// array below generated by AI for fast lookup of the 24-bit frame to send for each 8-bit character
static const uint32_t __in_flash("flash_constants") BYTE_FRAME_LUT[256] = {
    0xEAAAAAA0, 0xEB4AAAC0, 0xECCAAB20, 0xED2AAB40, 0xED2AACA0, 0xECCAACC0, 0xEB4AAD20, 0xEAAAAD40, 0xF2CAB2A0, 0xF32AB2C0, 0xF4AAB320, 0xF54AB340, 0xF54AB4A0, 0xF4AAB4C0, 0xF32AB520, 0xF2CAB540,
    0xF32ACAA0, 0xF2CACAC0, 0xF54ACB20, 0xF4AACB40, 0xF4AACCA0, 0xF54ACCC0, 0xF2CACD20, 0xF32ACD40, 0xEB4AD2A0, 0xEAAAD2C0, 0xED2AD320, 0xECCAD340, 0xECCAD4A0, 0xED2AD4C0, 0xEAAAD520, 0xEB4AD540,
    0xF4AB2AA0, 0xF54B2AC0, 0xF2CB2B20, 0xF32B2B40, 0xF32B2CA0, 0xF2CB2CC0, 0xF54B2D20, 0xF4AB2D40, 0xECCB32A0, 0xED2B32C0, 0xEAAB3320, 0xEB4B3340, 0xEB4B34A0, 0xEAAB34C0, 0xED2B3520, 0xECCB3540,
    0xED2B4AA0, 0xECCB4AC0, 0xEB4B4B20, 0xEAAB4B40, 0xEAAB4CA0, 0xEB4B4CC0, 0xECCB4D20, 0xED2B4D40, 0xF54B52A0, 0xF4AB52C0, 0xF32B5320, 0xF2CB5340, 0xF2CB54A0, 0xF32B54C0, 0xF4AB5520, 0xF54B5540,
    0xF52CAAA0, 0xF4CCAAC0, 0xF34CAB20, 0xF2ACAB40, 0xF2ACACA0, 0xF34CACC0, 0xF4CCAD20, 0xF52CAD40, 0xED4CB2A0, 0xECACB2C0, 0xEB2CB320, 0xEACCB340, 0xEACCB4A0, 0xEB2CB4C0, 0xECACB520, 0xED4CB540,
    0xECACCAA0, 0xED4CCAC0, 0xEACCCB20, 0xEB2CCB40, 0xEB2CCCA0, 0xEACCCCC0, 0xED4CCD20, 0xECACCD40, 0xF4CCD2A0, 0xF52CD2C0, 0xF2ACD320, 0xF34CD340, 0xF34CD4A0, 0xF2ACD4C0, 0xF52CD520, 0xF4CCD540,
    0xEB2D2AA0, 0xEACD2AC0, 0xED4D2B20, 0xECAD2B40, 0xECAD2CA0, 0xED4D2CC0, 0xEACD2D20, 0xEB2D2D40, 0xF34D32A0, 0xF2AD32C0, 0xF52D3320, 0xF4CD3340, 0xF4CD34A0, 0xF52D34C0, 0xF2AD3520, 0xF34D3540,
    0xF2AD4AA0, 0xF34D4AC0, 0xF4CD4B20, 0xF52D4B40, 0xF52D4CA0, 0xF4CD4CC0, 0xF34D4D20, 0xF2AD4D40, 0xEACD52A0, 0xEB2D52C0, 0xECAD5320, 0xED4D5340, 0xED4D54A0, 0xECAD54C0, 0xEB2D5520, 0xEACD5540,
    0xED52AAA0, 0xECB2AAC0, 0xEB32AB20, 0xEAD2AB40, 0xEAD2ACA0, 0xEB32ACC0, 0xECB2AD20, 0xED52AD40, 0xF532B2A0, 0xF4D2B2C0, 0xF352B320, 0xF2B2B340, 0xF2B2B4A0, 0xF352B4C0, 0xF4D2B520, 0xF532B540,
    0xF4D2CAA0, 0xF532CAC0, 0xF2B2CB20, 0xF352CB40, 0xF352CCA0, 0xF2B2CCC0, 0xF532CD20, 0xF4D2CD40, 0xECB2D2A0, 0xED52D2C0, 0xEAD2D320, 0xEB32D340, 0xEB32D4A0, 0xEAD2D4C0, 0xED52D520, 0xECB2D540,
    0xF3532AA0, 0xF2B32AC0, 0xF5332B20, 0xF4D32B40, 0xF4D32CA0, 0xF5332CC0, 0xF2B32D20, 0xF3532D40, 0xEB3332A0, 0xEAD332C0, 0xED533320, 0xECB33340, 0xECB334A0, 0xED5334C0, 0xEAD33520, 0xEB333540,
    0xEAD34AA0, 0xEB334AC0, 0xECB34B20, 0xED534B40, 0xED534CA0, 0xECB34CC0, 0xEB334D20, 0xEAD34D40, 0xF2B352A0, 0xF35352C0, 0xF4D35320, 0xF5335340, 0xF53354A0, 0xF4D354C0, 0xF3535520, 0xF2B35540,
    0xF2D4AAA0, 0xF334AAC0, 0xF4B4AB20, 0xF554AB40, 0xF554ACA0, 0xF4B4ACC0, 0xF334AD20, 0xF2D4AD40, 0xEAB4B2A0, 0xEB54B2C0, 0xECD4B320, 0xED34B340, 0xED34B4A0, 0xECD4B4C0, 0xEB54B520, 0xEAB4B540,
    0xEB54CAA0, 0xEAB4CAC0, 0xED34CB20, 0xECD4CB40, 0xECD4CCA0, 0xED34CCC0, 0xEAB4CD20, 0xEB54CD40, 0xF334D2A0, 0xF2D4D2C0, 0xF554D320, 0xF4B4D340, 0xF4B4D4A0, 0xF554D4C0, 0xF2D4D520, 0xF334D540,
    0xECD52AA0, 0xED352AC0, 0xEAB52B20, 0xEB552B40, 0xEB552CA0, 0xEAB52CC0, 0xED352D20, 0xECD52D40, 0xF4B532A0, 0xF55532C0, 0xF2D53320, 0xF3353340, 0xF33534A0, 0xF2D534C0, 0xF5553520, 0xF4B53540,
    0xF5554AA0, 0xF4B54AC0, 0xF3354B20, 0xF2D54B40, 0xF2D54CA0, 0xF3354CC0, 0xF4B54D20, 0xF5554D40, 0xED3552A0, 0xECD552C0, 0xEB555320, 0xEAB55340, 0xEAB554A0, 0xEB5554C0, 0xECD55520, 0xED355540
};

/*
uint16_t calculate_frame_payload(uint8_t data)
{
    // this payload has the msb as the first bit to be sent!
    return BYTE_PAYLOAD_LUT[data];
}

*/

uint32_t construct_frame(uint16_t data)
{
    // data contains a 12 bit value:
    //  d c b a 7 6 5 4 3 2 1 0          (dcba= checksum, 76543210 is the data payload)
    return BYTE_FRAME_LUT[(uint8_t)(data & 0xFF)];

    /*
    // Fast path for payloads generated by calculate_frame_payload().
    uint8_t data_byte = (uint8_t)(data & 0xFF);
    if (BYTE_PAYLOAD_LUT[data_byte] == data) {
        return BYTE_FRAME_LUT[data_byte];
    }
    */
    // return construct_frame_slow(data);
}

//  to send a frame, the bits need to be split into half-bits:
//      start frame :  3 hi-lo transitions
//      0-bit       :  1 hi-lo, 1 lo-lo 
//      1-bit       :  1 lo-lo, 1 hi-lo
//  the irout state machine sends the following:
//      input 0-bit :  send lo-lo frame
//      input 1-bit :  send hi-lo frame
//
// for sending a 1 bit: put 01 in the output frame (lsb sent first)
// for sending a 0 bit: put 10 in the output frame (lsb sent first)


void PrintBuffer_init()
{
    queue_init(&PrintBuffer, sizeof(uint16_t), PrintBufSize);
}


void Print_task()
{
    // check for a first connection
    if (cdc_connected_now[ITF_PRINT]) {
        // only if the Print CDC interface is connected
        if (cdc_connected_changed[ITF_PRINT]) {
            // this is now a new connection, report that to the CLI
            // we cannot report to the device itself
            cli_printf("  CDC Port 5 [printer] connected");
            cdc_connected_changed[ITF_PRINT] = false;   // clear the changed flag, as we have reported the change
        }
    } else {
        // not connected, so we can check if there was a change in the connection status
        if (cdc_connected_changed[ITF_PRINT]) {
            // this is now a disconnection, report that to the CLI
            cli_printf("  CDC Port 5 [printer] disconnected");
            cdc_connected_changed[ITF_PRINT] = false;   // clear the changed flag, as we have reported the change
        }

    }

    // if there is a pending ir character to be sent, try that first before reading the next character from the printbuffer
    if (irframe_pending) {
        // there is a pending IR frame to be sent, so try that first
        if (!ir_busy()) {
            // the PIO is not busy anymore, so we can send the pending frame
            // and construct the frame for the IR LED
            // ir_code = calculate_frame_payload((uint8_t)PrintChar);
            // ir_frame = construct_frame(ir_code);
            // send_ir_frame(ir_frame);
            send_ir_frame(BYTE_FRAME_LUT[(uint8_t)PrintChar]);
            irframe_pending = false;   // clear the pending flag
        } 
    }

    // absorb any input from the printer serial port to prevent the port from being blocked
    while (cdc_available(ITF_PRINT) > 0) {
        // read the next character from the USB CDC port
        char dunp = cdc_read_char(ITF_PRINT);
    }


    if (serial_pending) {
        // there is a pending serial character to be sent, so try that first
        if (((globsetting.get(PRT_emu_mode) == 1) || (globsetting.get(PRT_emu_mode) == 2)) && cdc_connected_now[ITF_PRINT]) {
            // emulation to a serial printer, no graphics, translate to ASCII or UTF-8
            // collect the character in the PrintLine buffer until a newline is received, then send the line to the USB CDC port
            if (PrintChar < 128) {
                // only add printable characters to the line buffer, ignore control characters except for newline
                // translate the chacater using the HP82143A table
                if (globsetting.get(PRT_emu_mode) == 1) {
                    // HP82143A character set translates to pure ASCII
                    // always 1 character per code, so we can directly add the first character of the mapping to the line buffer
                    PrintLine[PrintLineLen] = HP82143A_CHAR_MAP_ASCII[(int)PrintChar].data[0];
                    PrintLineLen++;
                    PrintBufLineLen++;
                } else if (globsetting.get(PRT_emu_mode) == 2) {
                    // HP82143A character set translated to UTF-8 character set
                    for (int i = 0; i < HP82143A_CHAR_MAP_UTF8[(int)PrintChar].length; i++) {
                        PrintLine[PrintLineLen] = HP82143A_CHAR_MAP_UTF8[(int)PrintChar].data[i];
                        PrintLineLen++;                    
                    }
                    PrintBufLineLen++;
                }
                serial_pending = false;   // clear the pending flag, as we have processed the character and added it to the line buffer
            }

            if ((PrintChar > 0xA0) && (PrintChar < 0xB7)) {
                // add 1..23 space characters to the line buffer, as the HP82143A printer uses the codes 0xA1..0xB6 for 1..23 spaces
                for (int i = 0; i < (PrintChar - 0xA0); i++) {
                    PrintLine[PrintLineLen] = ' ';
                    PrintLineLen++;
                    PrintBufLineLen++;
                }
                serial_pending = false;   // clear the pending flag, as we have processed the character and added it to the line buffer
            }

            if (PrintChar == 0xE0) {
                // EOLL character fo left justify the line, so we can send the line to the USB CDC port
                // add CR and LF and terminate with NULL character
                PrintLine[PrintLineLen] = '\r';   // add carriage return
                PrintLine[PrintLineLen + 1] = '\n';   // add line feed
                PrintLine[PrintLineLen + 2] = '\0';   // null terminate the line
                cdc_sendbuf(ITF_PRINT, PrintLine, PrintLineLen + 2);   // send the line to the USB CDC port
                cdc_flush(ITF_PRINT);              // flush the USB CDC port to ensure the
                serial_pending = false;           // clear the pending flag
                PrintLineLen = 0;                 // reset the line length for the next line
                PrintBufLineLen = 0;              // reset the line length in the print buffer for the next line
                return;
            }

            if (PrintChar == 0xE8) {
                // EOLR character for right justify the line, so we can send the line to the USB CDC port
                // for right justification we can add spaces at the beginning of the line until we reach the desired line length, for example 80 characters
                int line_length = 24;                     // desired line length for right justification, can be adjusted as needed
                int num_spaces = line_length - PrintBufLineLen;        // calculate the number of spaces to add at the beginning of the line
                if (num_spaces < 0) num_spaces = 0;       // if the line is already longer than the desired line length, we don't add any spaces

                // shift the existing line to the right by adding spaces at the beginning
                for (int i = PrintLineLen - 1; i >= 0; i--) {
                    PrintLine[i + num_spaces] = PrintLine[i];
                }
                // add spaces at the beginning of the line
                for (int i = 0; i < num_spaces; i++) {
                    PrintLine[i] = ' ';
                }

                // add CR and LF and terminate with NULL character
                PrintLine[PrintLineLen + num_spaces] = '\r';   // add carriage return
                PrintLine[PrintLineLen + num_spaces + 1] = '\n';   // add line feed
                PrintLine[PrintLineLen + num_spaces + 2] = '\0';   // null terminate the line

                cdc_sendbuf(ITF_PRINT, PrintLine, PrintLineLen + num_spaces + 2);   // send the line to the USB CDC port
                cdc_flush(ITF_PRINT);               // flush the USB CDC port to ensure the line is sent immediately
                serial_pending = false;             // clear the pending flag
                PrintLineLen = 0;                   // reset the line length for the next line
                PrintBufLineLen = 0;                // reset the line length in the print buffer for the next line
                return;
            }

            // now print the line if the line length of 24 chars is reached, 
            // as the HP82143A printer has a line length of 24 characters
            // this is mainly a a guard against a missing EOL
            if (PrintBufLineLen > 30) {
                PrintLine[PrintLineLen] = '\r';         // add carriage return
                PrintLine[PrintLineLen + 1] = '\n';     // add line feed
                PrintLine[PrintLineLen + 2] = '\0';     // null terminate the line
                cdc_sendbuf(ITF_PRINT, PrintLine, PrintLineLen + 2);   // send the line to the USB CDC port
                cdc_flush(ITF_PRINT);                   // flush the USB CDC port to ensure the line is sent immediately
                PrintLineLen = 0;                       // reset the line length for the next line
                serial_pending = false;                 // clear the pending flag
                PrintBufLineLen = 0;                    // reset the line length in the print buffer for the next line
                return;
            }

            // need to ensure that in Serial mode the CO (Column Out) mode is detected to ignore everything after that
            // and enable when CO mode is switched ogg again to be able to ignore all graphics

            // for now just ignore anything else
            serial_pending = false;   // clear the pending flag, as we have processed the character and added it to the line buffer

        } else if (cdc_connected_now[ITF_PRINT]) {
            // only if connected and serial printing enabled
            // intended for HP82143A printer emulation
            cdc_send_printport(PrintChar);                // send the character to the USB printport with flush
            serial_pending = false;                       // clear the pending flag
        } else {
            // not connected anymore, so clear the pending flag
            serial_pending = false;                       
        }
    }

    if (serial_pending || irframe_pending) {
        // there is still a pending character to be sent, so we cannot read the next character from the printbuffer yet
        return;
    }

    // we get here only if there is no pending IR frame or serial character, so we can read the next character from the printbuffer and process it


    // if there is something pending in the printbuffer, read the character 
    // printbuffer must be processed in case nothing is connected just to empty it
    if (!queue_is_empty(&PrintBuffer)) {
        queue_remove_blocking(&PrintBuffer, &PrintChar);

        // check which output is selected
        if ((globsetting.get(PRT_output_mode) == 1) || (globsetting.get(PRT_output_mode) == 3)) {
            // serial printing is enabled
            serial_pending = true;
        }

        if ((globsetting.get(PRT_output_mode) == 2) || (globsetting.get(PRT_output_mode) == 3)) {
            // IR printing is enabled
            irframe_pending = true;
        }

        // if output mode was 0 (no output) the frame is now simply discarded

        // line below for debugging the construction of the IR frame
        // printf("IR char = %02X, code = %04X, frame = %08X\n", PrintChar, ir_code, ir_frame);
    }
}

void PrintIRchar(uint8_t c)
{
    // send a character to the IR printer, for testing the IR LED
    // ir_code = calculate_frame_payload(c);
    // ir_frame = construct_frame(ir_code);
    // send_ir_frame(ir_frame);

    // the fast way with the LUTs is about 100 times faster than the slow way, so we can easily send the character at the baudrate of the IR printer (about 2400 baud)
    // keep in mind that this function is blocking
    // better use in combination with ir_busy() and a queue
    send_ir_frame(BYTE_FRAME_LUT[c]);
}

// HP-IL tasks and PIL-box emulation

// function to get the HP-IL menmonic from a given frame
void getIL_mnemonic(uint16_t wFrame, char *mnem)
{
    // char mnem[5];
    // first find the mnemonic
    int i = 0;
    while (i < numILmnemonics) {   
        if ((wFrame & IL_mnemonics[i].ILmask) == IL_mnemonics[i].ILcode) break;
        i++;
    }
    sprintf(mnem, "%s", IL_mnemonics[i].ILmnemonic);
}


void ILscope_task()
{
    // this task is for monitoring the HP-IL communication, it prints the frames to the USB CDC port in a human readable format
    // it can be enabled/disabled with the ilscope_enabled variable and the settings in the globsetting for IL_scope

    // replaces the routines in the HPIL_task() to decouple the HP-IL communication from the printing of the frames, 
    // so that the HP-IL communication is not slowed down by the printing to the USB CDC port

    // check the status of the IL SCOPE, and if there is a first connect
    // also check for an input char to pause the scope

    if (cdc_connected_now[ITF_ILSCOPE]) {
        // only if the IL SCOPE CDC interface is connected

        // was this a first connection?
        if (cdc_connected_changed[ITF_ILSCOPE]) {
            // there was a change in the connection status and we are connected
            // this is a new connection, so we can display the welcome/status message
            cdc_connected_changed[ITF_ILSCOPE] = false;     // reset the flag, we have processed the change in connection status
            ILScopePrintLen = 0;
            cli_printf("  CDC Port 4 [IL Scope] connected");
            ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen, "IL Scope CDC PORT connected\r\n");
            cdc_sendbuf(ITF_ILSCOPE, ILScopePrint, ILScopePrintLen);
            cdc_flush(ITF_ILSCOPE);
        } else {
            // no change in the connection status, so we can check if there is any input from the host
            if (cdc_read_char(ITF_ILSCOPE) != 0) {
                ILScopePrintLen = 0;
                ilscope_enabled = !ilscope_enabled;
                globsetting.set(ilscope_enabled, !globsetting.get(ilscope_enabled));
                ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen, "IL Scope is %s\n\r", ilscope_enabled ? "enabled":"disabled");
                cdc_sendbuf(ITF_ILSCOPE, ILScopePrint, ILScopePrintLen);
                cdc_flush(ITF_ILSCOPE);
            }
        }
    } else {
        // not connected, so we can check if there was a change in the connection status
        if (cdc_connected_changed[ITF_ILSCOPE]) {
            // there was a change in the connection status and we are now disconnected
            // this is a disconnection, so we can display a message
            cdc_connected_changed[ITF_ILSCOPE] = false;     // reset the flag, we have processed the change in connection status
            ILScopePrintLen = 0;
            cli_printf("  CDC Port 4 [IL Scope] disconnected");
            ilscope_enabled = false;          // disable the IL Scope when disconnected
        }
    }


    if (ILScope_send_pending) {
        // if there is a pending traceline to be sent, try to send it again
        if (cdc_connected_now[ITF_ILSCOPE]) {
            ILScope_send_pending = !cdc_sendbuf_available(ITF_ILSCOPE, ILScopePrint, ILScopePrintLen);

            if (ILScope_send_pending) {
                // still not able to send, so we will try again later
                return;
            }
        } else {
            // not connected, so we cannot send, we will try again later
            // should not happen
            ILScope_send_pending = false; 
        }
    }

    // Struct for the IL Scope queue
    /*
    struct __attribute__((packed))ILScope_line {
    uint32_t    cycle_number;       // cycle number of the frame to match with the mcode tracer cycle number
    uint16_t    frame;              // HP-IL frame                             2 bytes
    uint16_t    PILframe;           // PILBox frame (2 serial bytes)           2 bytes
    uint8_t     frame_type;         // type of the frame
                                    // bit 0: direction, 0 out, 1 = in (viewed from TULIP)
                                    // bit 1: frame type, 0 = IL frame, 1 = PILBox frame
    }; */


    // check if there is something in the IL Scope queue, if so print it to the USB CDC port
    if (!queue_is_empty(&ILscopeBuffer)) {
        // only do this if something is connected or enabled, otherwise just wasting cycles
        tud_task();
        
        // get the line from the queue
        queue_try_remove(&ILscopeBuffer, &ILScopeLine);

        if (!cdc_connected(ITF_ILSCOPE)) {
            // no serial port connected, do nothing
            return;
        }

        if (!ilscope_enabled) {
            // scope is paused, do nothing
            return;
        }

        // now check the type of the line, if bit 1 of frame_type is set it is a PILBox line, otherwise it is a normal HP-IL line
        bool out = (ILScopeLine.frame_type & 0x1) == 0 ? false : true;   // bit 0 indicates direction, 0 out, 1 in (viewed from TULIP)
        bool pilbox = (ILScopeLine.frame_type & 0x2) == 0 ? false : true; // bit 1 indicates frame type, 0 = IL frame, 1 = PILBox frame

        // check which type of tracing is enabled in the settings, if not enabled do nothing
        if (pilbox && !globsetting.get(ilscope_PIL_enabled)) return;
        if (!pilbox && !globsetting.get(ilscope_IL_enabled)) return;

        // now prepare the traceline
        // first find the mnemonic
        int i = 0;
        while (i < numILmnemonics) {   
            if ((ILScopeLine.frame & IL_mnemonics[i].ILmask) == IL_mnemonics[i].ILcode) break;
            i++;
        } 

        if (pilbox) {
            // build the ILScope string for the PILBox frame
            ILScopePrintLen = 0;
            ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen,"  PILBox %s %04X %s   ", out ? ">":"<", ILScopeLine.frame, IL_mnemonics[i].ILmnemonic);
            ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen,"hi: %02X lo: %02X  [%8d]", (ILScopeLine.PILframe >> 8) & 0xFF, ILScopeLine.PILframe & 0xFF, ILScopeLine.cycle_number);
        } else {
            // build the ILScope string for the HP-IL frame
            ILScopePrintLen = 0;
            ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen," %s %03X %s                           [%8d]",  out ? ">":"<", ILScopeLine.frame, IL_mnemonics[i].ILmnemonic, ILScopeLine.cycle_number);

        }

        // check for a possible overflow of the ILScope buffer, if there was an overflow print a warning message
        if (ILScope_Overflow) {
            ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen,"    possible ILScope overflow, frames may be lost\n\r");
        } else {
            ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen,"\n\r");
        }

        if (cdc_connected(ITF_ILSCOPE)) {
            // only send if something is connected, otherwise this will stall
            // better move this test to earlier in the tracer task for performance
            ILScope_send_pending = !cdc_sendbuf_available(ITF_ILSCOPE, ILScopePrint, ILScopePrintLen);
        }
        // cdc_sendbuf(ITF_ILSCOPE, ILScopePrint, ILScopePrintLen);
        // cdc_flush(ITF_ILSCOPE);
    }
}

void HPIL_scope(uint16_t wFrame, bool out, bool traceIDY)
{
    // HPIL tracer, shoing outgoing and incoming frames
    // output is sent to its own USB CDC port
    //  out         - when true this is outgoing traffic, otherwise it is incoming
    // traceIDY is now always enabled

    // if (ilscope_enabled && globsetting.get(ilscope_IL_enabled)) {
        // only do this if something is connected or enabled, otherwise just wasting cycles

        // prepare the packet for the ILScope queue
        IL_Line.cycle_number = cycles();
        IL_Line.frame        = wFrame;
        IL_Line.PILframe     = 0;   // not a PILBox frame
        IL_Line.frame_type   = 0;
        if (out) IL_Line.frame_type |= 0x1;   // set bit 0 for outgoing frames
                                              // bit 1 is already 0 for HP-IL frames

        // do a non-blocking add to the queue, if there are overflows find a
        ILScope_Overflow = !queue_try_add(&ILscopeBuffer, &IL_Line);              
    // } 
}

void PILBox_scope(uint16_t wFrame, uint8_t bhi, uint8_t blo, bool out)
{
    // PILBox tracer, showing outgoing and incoming frames and bytes
    // output is sent to the USB CDC port that is also showing the HP-IL scope
    //  out         - when true this is outgoing traffic, otherwise it is incoming

    // only job is to prepare the struct and put it in the queue for the IL Scope task
    // if (ilscope_enabled && globsetting.get(ilscope_PIL_enabled)) {
        // only do this if enabled, otherwise just wasting cycles

        // prepare the ILscope trace struct

        IL_Line.cycle_number = cycles();
        IL_Line.frame        = wFrame;
        IL_Line.PILframe     = (bhi << 8) | blo;
        IL_Line.frame_type   = 0;
        if (out) IL_Line.frame_type |= 0x1;   // set bit 0 for outgoing frames
        IL_Line.frame_type  |= 0x2;            // set bit 1 for PILBox frames

        // do a non-blocking add to the queue
        ILScope_Overflow = !queue_try_add(&ILscopeBuffer, &IL_Line);  
    // }
}

void HPIL_init()
{
    // initialize all HP-IL registers to their default state
    // initialize the HP-IL buffer to communicate with the core1 function

    // initialize send and receive buffer, HPIL frame is a 16-bit word
    queue_init(&HPIL_SendBuffer, sizeof(uint16_t), HPIL_BufSize);
    queue_init(&HPIL_RecvBuffer, sizeof(uint16_t), HPIL_BufSize);

    // Struct for the IL Scope queue
    /*
    struct __attribute__((packed))ILScope_line {
        uint32_t    cycle_number;       // cycle number of the frame to match with the mcode tracer cycle number
        uint16_t    frame;              // HP-IL frame input                            2 bytes
        uint8_t     frame_type;         // type of the frame
                                        // bit 0: direction, 0 out, 1 = in (viewed from TULIP)
                                        // bit 1: frame type, 0 = IL frame, 1 = PILBox frame
    }; */

    // initialize the IL Scope buffer to communicate with the IL Scope task
    queue_init(&ILscopeBuffer, sizeof(ILScope_line), ILscopeBufSize);

    // preset HP-IL registers for HP-IL module hot plugging
    // ASSERT(m_pHpil != NULL);
    // memset(m_pHpil->HPIL_REG,0,sizeof(m_pHpil->HPIL_REG));

    HPIL_REG[0] = 0x81;    // SC=1 MCL=1
    HPIL_REG[1] = 0x00;    // SC=1 MCL=1
    HPIL_REG[2] = 0x00;    // SC=1 MCL=1
    HPIL_REG[3] = 0x00;    // SC=1 MCL=1
    HPIL_REG[7] = 0x00;    // SC=1 MCL=1
    HPIL_REG[8] = 0x00;    // SC=1 MCL=1

    // HP-IL scratch pad registers
    HPIL_REG[4] = 0x01;    // selected loop address
    HPIL_REG[5] = 0x01;    // current device number
    HPIL_REG[6] = 0x01;    // starting device number      

}


void PILBox_sendframe(uint16_t frame)
{
    // This is the sender of the PILBox emulation
    // sends the frame to the designated USB CDC port
    uint16_t outframe;

    loopbackFrame = 0xFFFF;

    if (!cdc_connected(ITF_HPIL) || (PILBox_mode == TDIS)) {
        // PILBox disabled (TDIS) or no serial connection
        // defaults to loopback
        loopbackFrame = frame;
    } else {
        /* ignore RFC handling for now
        if ((frame & 0x700) == 0x400)
        {
            // this is a CMD frame, save it for later
            // when an RFC comes
            PIL_CMD_frame = frame;       // save it for later
            outframe = frame;
        } 
        // else if (((frame & 0x700) == 0x600) && (PILBox_mode == COFI))
        // {
            // this is an IDY frame, only forward in COFI mode
            // loopbackFrame = frame;  // ignore for testing

        // }
        else if (frame == RFC)
        {
            // RFC frame from HP-IL
            outframe = PIL_CMD_frame;              // send previous CMD frame
        }
        */
       outframe = frame;

        // PILBox emulator has an existing connection
        // send data to the CDC output

        // we send the full frame here
        // normally we can optimize traffic by not sending the hi byte if it is the same as the previous hi byte
        // with the high speed USB connection this is not an issue anymore
        // to be implemented later
        if (PILmode8) {
            // 8-bit transfer mode
            PIL_tx_lo = (outframe & 0x007F) | 0x80;             // lower 7 data bits, msb = 1
            PIL_tx_hi = ((outframe >> 6) & 0x1E) | 0x20;        // PILBox hi byte previously sent
            cdc_send_char(ITF_HPIL, PIL_tx_hi);                 // send both chars
            cdc_send_char_flush(ITF_HPIL, PIL_tx_lo);           // flush after the 2nd byte
        } else {
            // 7-bit transfer mode
            PIL_tx_lo = (outframe & 0x003F) | 0x40;             // lower 6 data bits, msb = 1
            PIL_tx_hi = ((outframe >> 6) & 0x1F) | 0x20;        // higher byte
            cdc_send_char(ITF_HPIL, PIL_tx_hi);                 // send both chars
            cdc_send_char_flush(ITF_HPIL, PIL_tx_lo);           // flush after the 2nd byte
        }     
    }
    PILBox_scope(outframe, PIL_tx_hi, PIL_tx_lo, true);
}

uint16_t PILBox_revcframe()
{
    // This is the receiving end of the PILBox emulation
    // cheks if a frame is available and returns the frame if it is
    // returns 0xFFFF if no frame is available
    // also handles the PILBox initialization frame

    // info below from Christoph Giesselink:
    // PILBox commands
    //  #define TDIS   0x494  // TDIS
    //  #define COFI   0x495  // COFF with IDY, firmware >= v1.6, not used if no IDY frames are supported
    //  #define CON    0x496  // Controller ON, not used, HP41 can only be controller (check HP-IL DEVELOPMENT ROM Scope function!)
    //  #define COFF   0x497  // Controller OFF
    //  #define SSRQ   0x49C  // Set Service Request, obsolete, not used on HP41
    //  #define CSRQ   0x49D  // Clear Service Request, obsolete, not used on HP41

    //  most we don't need when we limit the HP41 always be the controller (not supporting the Scope in the HP-IL DEVEL ROM).
    //  and we don't need COFI when we don't support IDY frames on the device side on first instance.
    //  Another special is the way to transfer 11 HP-IL bits over a byte oriented interface with an acknowlege when using 9600 baud.
    //  And finally the CMD/RFC frame handling on the controller side, it's important to know that the RFC frame is not transferred to the device side

    // frames are sent/received in the following format (ducument by JF Garnier):
    //  7-bit transfers:        001cccbb    hi byte: higher 5 bits (3 control and data bits 7 and 6)
    //                          01bbbbbb    lo byte: lower 6 data bits
    //  8-bit transfers:        001cccb0    hi byte: higher 4 bits (3 control bits and data bit 7);
    //                          1bbbbbbb    lo byte: lower 7 data bits
    //                          this allows quicker retransmit of ASCII data frames (control bits 000, only 7 data bits)
    //
    // To use the 7/8 bit flexibility:
    //      If a frame is to be transmitted to the serial link, check if the high byte is the same as the previous transmitted frame. 
    //      If so, it is not retransmitted and only the lo byte is sent.
    //      If the receiver gets only the low byte, it will use the last received hi byte to rebuild the frame
    //
    // For example, when sending the PILBox initialization, bytes 0x32 and 0x97 are sent for the frame 0x497 (COFF command):
    //      0x32 = 0b00110010 (hi byte), 0x97 = 0b10010111 (lo byte)
    //               001cccb0                     1bbbbbbb 

    // other considerations:
    //  When driven by a controller on the HP-IL side (which is the case with the HP41) IDY frames are locally retransmitted
    //  Also RFC frames are retransmitted and not sent to the computer
    //  All other frames are sent out and received frames are returned to the HP-IL process

    // CMD/RFC handshake
    // CMD/RFC handshake is managed by the PILBox. In DEVICE mode (COFF, COFI) the handsahe is done as follows:
    //      when PILBox receives a CMD fram from HP-IL it keeps a copy and transmits it to the host PC
    //      when an RFC is then received from HP-IL, the PILBox sends the previous CMD frame to the host
    //      when the CMD frame comes back from the host, the OILBox retransmits the RFC to HP-IL
    //  In CONTROLLER mode (CON) (not applicable to the HP41 for now), the handshake is done as follows:
    //      when the PILBox receives a CMD frame from HP-IL it keeps a copy and transmits RFC to HP-IL
    //      when the RFC comes back from HP-IL the previous CMD is sent to the serial link

    // uint16_t PIL_rx_lo;                 // PILBox lo byte previously received   
    // uint16_t PIL_rx_hi;                 // PILBox hi byte previously received
    // uint16_t PIL_tx_lo;                 // PILBox lo byte previously sent   
    // uint16_t PIL_tx_hi;                 // PILBox hi byte previously sent
    // uint16_t PIL_rx_prevframe;          // PILBox previous frame received
    // uint16_t PIL_tx_prevframe;          // PILBox previous frame sent
    // uint16_t PIL_CMD_frame;             // PILBox CMD frame last sent
    // uint16_t PIL_FRC_frame;             // PILBox RFC frame last sent
    // bool PIL_rx_pending = false;        // true is a lo byte is read but not yet the hi byte
    // bool PIL_tx_pending = false;        // true if the hi byte still has to be sent
    // bool PILmode8 = true;               // PILBox transfer mode, true when in 8-bit mode, false when in 7-bit mode

    uint16_t frame;
    int16_t pil_recv;
    bool returnframe = false;               // true if a frame needs to be returned
                                            // false if no data available or only part of a frame received
    
    if (!cdc_connected(ITF_HPIL)) {
        // no valid serial link, loopback mode 
        frame = loopbackFrame;
        loopbackFrame = 0xFFFF;             // to indicate no new frame is available
        return frame;                       // and get out
    } else if (cdc_available(ITF_HPIL) == 0) {
        // no bytes available
        return 0xFFFF;                      // return no data and get out
    } else {
        // we get here when:
        // - there is a valid serial link
        // - and there is data available in the serial buffer
        // if a frame arrives we must check for a PILBox command first
        pil_recv = cdc_read_byte(ITF_HPIL);

        // PILBox emulation received a byte from the PILBox designated serial port
        // pil_recv contains the returned byte
        if ((pil_recv & 0xE0) == 0x20) {
            // this is the higher byte of a transfer
            PIL_tx_hi = pil_recv;       // save until the lower byte arrives
            return 0xFFFF;              // and return with no data
        }
        if ((pil_recv & 0x80) == 0x80) {
            // this is the lower byte of an 8-bit transfer
            PILmode8 = true;                    // set the correct mode to 8 bits
            PIL_rx_lo = pil_recv;               

            // this completes the 2-byte transfer, complete the frame
            PIL_rx_frame = (pil_recv & 0x7F) | ((PIL_tx_hi & 0x1E) << 6);
        }
        if ((pil_recv & 0xC0) == 0x40) {
            // this is the lower byte of a 7-bit transfer
            PILmode8 = false;            // set the correct mode
            PIL_rx_lo = pil_recv;       
              
            // this completes the 2-byte transfer, complete the frame
            PIL_rx_frame = (pil_recv & 0x3F) | ((PIL_tx_hi & 0x1F) << 6);
        }

        // The frame is now received, first process the PILBox commands
        // send to our scope for debugging
        PILBox_scope(PIL_rx_frame, PIL_tx_hi, pil_recv, false);

        switch (PIL_rx_frame) {
            case TDIS:                          // TDI: Translator DIsabled
                PILBox_mode = TDIS;             // set mode to disabled
                                                // frame is not forwarded to the HP-IL emulation
                cdc_send_char_flush(ITF_HPIL, pil_recv);       // return command for confirmation
                // return 0xFFFF;                  // and return with no data
                break;
            case CON:                           // CON: Controller ON
                PILBox_mode = CON;              // set mode to controller ON
                                                // default on the HP41
                                                // frame is not forwarded to the HP-IL emulation
                cdc_send_char_flush(ITF_HPIL, pil_recv);       // return command for confirmation
                return 0xFFFF;                  // and return with no data
                break;
            case COFF:                          // Controller OFF
                PILBox_mode = COFF;             // set mode to controller OFF
                                                // the PILBox is now a device
                                                // not used on the HP41
                                                // frame is not forwarded to the HP-IL emulation
                cdc_send_char_flush(ITF_HPIL, pil_recv);       // return command for confirmation
                return 0xFFFF;               // and return with no data
                break;
            case COFI:                          // Controller OFf with IDY 
                PILBox_mode = COFI;             // set mode to COFI
                                                // device with sending IDY frame
                                                // frame is not forwarded to the HP-IL emulation
                cdc_send_char_flush(ITF_HPIL, pil_recv);       // return command for confirmation
                return 0xFFFF;                  // and return with no data
                break;
            // default:

                // all other frames are sent on to the HP-IL loop
                // only need to check for a CMD frame
                // should do a check if the mode is TDIS, any traffic should be ignored
                // if (PIL_rx_frame == m_wLastFrame)
                // {
                    // this is a previous CMD frame, this is returned as an RFC frame
                //     PIL_rx_frame = RFC;
                // }
                // else if (PIL_rx_frame == RFC)
                // {
                //     PIL_rx_frame = m_wLastFrame;
                // }
        }
        // if we get here the frame is complete
        return PIL_rx_frame;
    }
}

/****************************/
// check for frame in transfer with 10s timeout (10*5780)
/****************************/

// probably do not need this, the time-out is handled in the HP-IL module itself
bool IsFrameInTransfer(uint16_t dwIncr)
{
  bool bInTransfer = (m_eMode != 0);
  if (bInTransfer) {
    // m_dwCpuCycles += dwIncr;
    // if (m_dwCpuCycles >= 10*5780)           // check for timeout
    // {
    //   bInTransfer = false;
    //   m_eMode = eNone;
    // }
  }
  return bInTransfer;
} 

// code for HP-IL loop timeout (10 seconds)
// probably do not need this, the time-out is handled in the HP-IL module itself
int64_t HPIL_timeout_callback(alarm_id_t id, void *user_data) {
    HPIL_timeout = true;

    return 0;
}


// code for sending IDY frames every 10ms when the HP41 is in light sleep
// this is not interrupt controller but done with a simple Timer
// routine must be called by the 
void HPIL_AutoIDYTask()
{
    absolute_time_t t_now;
    int32_t ms_elapsed;
    int64_t us_elapsed;
    // task to send AUTO IDY frames
    // this must be done every 10 ms when the HP41 is in light sleep, and if AUTO IDY is enabled
    // routine must be called by the HPIL_task
    // the following table is from Christoph Giesselink V41 source (hpil.cpp)
    //      light sleep, CA, AUTO IDY -> 10ms Timer, IDY frame
    //      0            0   0           0           0
    //      0            0   1           0           0
    //      0            1   0           0           0
    //      0            1   1           0           0
    //      1            0   0           0           0
    //      1            0   1           1           0
    //      1            1   0           0           0
    //      1            1   1           1           1

    // loop is entered on the following conditions:
    //   - powermode is Light Sleep (STANDBY)
    //   - AutoIDY is enabled (bit 6 in HP-IL register 3)
    //   - HP-IL is in Controller mode (CA bit, bit 6 in HP-IL register 0) 
    // loop checks if 10 ms are expired before sending AutoIDY frame 0x6C0

    if (((HPIL_REG[3] & 0x40) != 0) && (HP41_powermode == PowerMode_LightSleep) && ((HPIL_REG[0] & 0x40) != 0)) {
        t_now = get_absolute_time();
        us_elapsed = absolute_time_diff_us(t_IDY_timer, t_now);
        if (us_elapsed > (10*1000)) {
            // 10 ms have passed so send a new IDY frame and reset the timer
            t_IDY_timer = t_now;
            HPIL_SendFrame(IDY_C0);   // frame to send is 0x6C0
        }
    }
}

void HPIL_SendFrame(uint16_t wFrame)
{
    // sends a frame to the HPIL bus
    // in this case the frame will be sent to the PILBox emulator
    HPIL_scope(wFrame, true, true);

    if (wFrame != RFC) {            // not a RFC frame
        m_wLastFrame = wFrame;      // remember last send frame
                                    // but not really used ??
    }

    PILBox_sendframe(wFrame);       // send to the PILBox emulation
}

void HPIL_RecvFrame(uint16_t wFrame)
{
    // receives a frame from the HPIL bus
    // in this case the frame is received from the PILBox emulator
    // modelled after functions in V41 (Christoph Giesselink)
    // for the HP-IL Scope
    HPIL_scope(wFrame, false, true);

    // CMD frame and CA (controller active) and adding RFC frame enabled
    // CMD/RFC handshaking is done in the PILBox emulation
    if (((wFrame & 0x700) == 0x400) && (HPIL_REG[0] & 0x40) != 0) {
        m_wLastCmd = wFrame;                            // remember last CMD frame
        HPIL_SendFrame(RFC);                            // send the RFC frame
        return;
    }

    // CA (controller active) and RFC frame
    // CMD/RFC handshaking done in the PILBox emulation
    if (((HPIL_REG[0] & 0x40) != 0) && (wFrame == RFC)) {
        wFrame = m_wLastCmd;                            // use the last CMD frame as answer
    } 
        
    queue_try_add(&HPIL_RecvBuffer, &wFrame);           // and put the frame in the bufffer
}


void HPIL_task()
{
    uint16_t HPIL_sendframe;
    uint16_t HPIL_recvframe;
    // called constantly from the main loop in core0
    // handle all HP-IL communication and PIL-Box emulation
    //

    // tasks to be implemented:
    //  - check if a frame is ready to be sent by the HP41 (write to DATA OUTPUT register)
    //  - check if a frame was received from the bus (PILBox emulator) and process
    //  - update the flag register and the FI flags depending on the status (done in send/receive routines)
    //  - check for PILBox emulator handling needed (initialization, data received, data to be sent)
    //  - call the HPIL_AutoIDYTask function 

    // first of all check if HP-IL is enabled and active at all
    // if (!globsetting.get(HP82160A_enabled)) return;

    // check for a first connection from the HP-IL CDC
    if (cdc_connected(ITF_HPIL)) {
        // only if the HP-IL CDC interface is connected
        if (!HPIL_firstconnect) {
            // HPIL_firstconnect was false, so this is now a new CDC connection
            HPIL_firstconnect = true;
            cli_printf("  CDC Port 3 [HPIL] connected");
            if (PILBox_mode == TDIS || PILBox_mode == 0) {
                // show a warning that there is no HP-IL connection
                // only show a warning if there is no PilBox connection with 1 second?
                cli_printf("  WARNING: No virtual HP-IL device connected, HP-IL loop may be open");
            }
        }
    }

    // check for disconnection of HP-IL
    if ((HPIL_firstconnect) && (!cdc_connected(ITF_HPIL))) {
        // CDC interface is disconnected
        cli_printf("  CDC Port 3 [HPIL] disconnected");
        HPIL_firstconnect = false;
    }

    // check for a change in the PILBox mode and report this
    if (PILBox_mode != PILBox_prevmode) {
        // there is a change in the PILBox mode, report to the console
        switch(PILBox_mode) {
            case TDIS:  cli_printf("  PILBox mode changed to TDIS / disconnected            - HP-IL loop may be open!"); break;
            case CON :  cli_printf("  PILBox mode changed to CON  / Controller ON           - HP-IL loop is closed"); break;
            case COFF:  cli_printf("  PILBox mode changed to COFF / Controller OFF          - HP-IL loop is closed"); break;
            case COFI:  cli_printf("  PILBox mode changed to COFI / Controller OFF with IDY - HP-IL loop is closed"); break;
            default  :  cli_printf("  oops, unknown PILBox mode, try to re-connect or reboot the TULIP"); break;
        }
        PILBox_prevmode = PILBox_mode;
    }


    // HP-IL emulation is active, first check if a frame should be sent
    if (!queue_is_empty(&HPIL_SendBuffer))
    {
        // data in the queue, so send it out
        queue_remove_blocking(&HPIL_SendBuffer, &HPIL_sendframe);   // get frame from the queue
        HPIL_SendFrame(HPIL_sendframe);                             // and send it
    }

    // check if a frame is received by the PILBox

    if ((HPIL_recvframe = PILBox_revcframe()) != 0xFFFF ) {
        // frame is received, now process
        HPIL_RecvFrame(HPIL_recvframe);
    }

    if (HP41_powermode == PowerMode_LightSleep)
    {
        // HP41 in Light Sleep, may need to send AutoIDY frames every 10ms
        // AUTO IDY bit is checked here
        // enable_AUTOIDY can be used to overrule the AUTOIDY bit
        if (enable_AUTOIDY) HPIL_AutoIDYTask();
    }
}
