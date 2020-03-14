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

PA0 - PA1 = Port address from port 0 to port 3
D0 - D7 = Data lines for input and output
RD = Read, APU will output data to lines D0 to D7
WR = Write, APU will input data from lines D0 to D7
PA7 = Connected to active low chip select (CS), connect to GND
PA6 = Connected to chip select (CS), connect to VCC (5V)
SMPCK = Clock output from DSP, 2.23 MHz
MUTE = Mute output from DSP

## TODO

* Can play some songs but not all because of the fixed boot code location.
* Uploading RAM takes really long, try to see for bottlenecks and remove them.
* Comment code with Doxygen when cleaned up.
* Add description of the project to this repository.
* Add schematics.

