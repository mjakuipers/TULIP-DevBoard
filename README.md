# TULIP-DevBoard
README for the TULIP4041 DevBoard
![IMG_20240925_110722543](https://github.com/user-attachments/assets/33102d0d-1736-4b8a-81de-e93384321606)

[TUP-Devboard Schematics V1.1.pdf](https://github.com/user-attachments/files/16324529/TUP-Devboard.Schematics.V1.1.pdf)

[TUP-Module Adapter Schematics V1.0.pdf](https://github.com/user-attachments/files/16324675/TUP-Module.Adapter.Schematics.V1.0.pdf)

If you want to purchase the PCB's please contact me via PM at the HPmuseum forum or lwave an issue with your contact information.

Version history

- jul 21 2024   - initial version, schematics updated, not yet verified in hardware!
- sep 25 2024   - DevBoard PCB's are verified and ready to ship
                - Will change to the RP2350 and Pico2 after evaluating the Pico2
                - BOM file updated with the RP2350 Pico2
                - Documentation uploaded: TULIP description, usage, assembly and testing of DevBoard
- sep 26 2024   - The directory TULIP4041-RP2040-Final contains the archived 2040 sources and the latest binary (.uf2)
- sep 26 2024   - The direcory TULIP-P2 contains the sources and build for the RP2350 version
- oct 11 2024   - BOM adapted, line 11, R6 must be 200k resistor (pulldown)
                - added component references
- oct 21 2024   - I mussed have messed up something. Source directory is deleted and will be up when fixed
                - a new binary (.uf2 file) is now available. Pleas use this
                - fixed FI driving (to enable HP-IL emulation)
                - tracing of the FI line is disabled, will be fixed soon
                - firmware now running at 125 MHz

All files are open source. Use of the hardware and software AT YOUR OWN RISK, no warranty
