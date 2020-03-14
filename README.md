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

![APU Pinout](/images/apu-pinout.png?raw=true "APU pinout")

## TODO

* Can play some songs but not all because of the fixed boot code location.
* Uploading RAM takes really long, try to see for bottlenecks and remove them.
* Comment code with Doxygen when cleaned up.
* Add description of the project to this repository.
* Add schematics.

