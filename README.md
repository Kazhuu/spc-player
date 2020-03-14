# SPC Player

Arduino based SNES SPC file music player with the original SNES hardware.

![Arduino and SNES APU connected](/images/arduino-apu-connected.jpg?raw=true "Arduino and SNES APU connected")

This project is heavily based on these two projects:
https://github.com/emukidid/SNES_APU_SD and
https://www.caitsith2.com/snes/apu.htm.

This project is a work in progress. Works on some DKC2 songs if you want to
test. Tested and working songs include Stickerbrush Symphony and Crocodile
Cacophony.

## Audio Processing Unit (APU) Pinout

<p align="center">
  <img src="https://github.com/Kazhuu/spc-player/blob/master/images/apu-pinout.png?raw=true" alt="APU Pinout"/>
</p>

* ~ means active low signal.
* PA7 is port address bit 7, connected to active low chip select (~CS) pin on
    the SPC700 chip. Pull this low to GND.
* PA6 is port address bit 6, connected to chip select (CS) pin on the SPC700 chip.
    Pull this high to 5V.
* PA0 and PA1 are port address bits 0 and 1. Used to select which port from 0 to
    3 to write or read.
* From D0 to D7 are data lines for input and output a single byte.
* RD is active low read, APU will output data to lines D0 to D7 when pulled low.
* WR is active low write, APU will input data from lines D0 to D7 when pulled low.
* SMPCK is a clock output from DSP with avarage clock rate of 2.23 MHz.
* MUTE is active low mute output from DSP.

## TODO

* Can play some songs but not all because of the fixed boot code location.
* Comment code with Doxygen when cleaned up.
* Add description of the project to this repository.
* Add schematics.
