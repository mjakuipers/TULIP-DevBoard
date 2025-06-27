/*
 * module.c
 *
 * This file is part of the TULIP4041 project.
 * Copyright (C) 2024 Meindert Kuipers
 * 
 * This file contains the higher level emulation of peripherals:
 *  - IR printing
 *  - HP-IL communication and PILBox emulation
 *  - IO definitions and initialization
 * 
 * REVISION HISTORY
 * Jan 2024 - Initial version
 * Jan 2025 - Added MAXX instruction 0C0
 * 
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

#include "module.h"

CModules TULIP_Pages;




void fram_rommap_init() {
    // initialize the ROM map in FRAM if that was not initialized yet
    if (!TULIP_Pages.is_rommmap_inited()) {
        // initialize the ROM map in FRAM
        TULIP_Pages.init_rommap();
    }

    TULIP_Pages.retrieve(); // retrieve the ROM map from FRAM
}

// end of file module.c