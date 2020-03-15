# SPC Player

Arduino UNO based SNES SPC file music player with the original Audio Processing Unit
(APU).

<p align="center">
  <img src="https://github.com/Kazhuu/spc-player/blob/master/images/arduino-apu-connected.jpg?raw=true" alt="APU and Arduino connected"/>
</p>

## Table of Contents

<!-- vim-markdown-toc GFM -->

* [About](#about)
* [Structure of the Project](#structure-of-the-project)
* [How to Use](#how-to-use)
  * [Connecting APU to Arduino](#connecting-apu-to-arduino)
  * [Uploading Arduino Code](#uploading-arduino-code)
  * [Uploading Song With Python](#uploading-song-with-python)
* [Links](#links)
* [TODO](#todo)

<!-- vim-markdown-toc -->

## About

This project is heavily based on these two projects:
* https://github.com/emukidid/SNES_APU_SD and
* https://www.caitsith2.com/snes/apu.htm.

This project is a work in progress. Works on some DKC2 songs if you want to
test. Tested and working songs include Stickerbrush Symphony and Crocodile
Cacophony. Original SPC songs can be downloaded from
[Zophar's](https://www.zophar.net/music) website.

So what does this project provide? What other projects are lacking in my opinion
is a good documentation. That is the gap that I want to fill.  Thus this project
code does not aim for the SPC upload speed, instead it aims for the readability
and documenting the code. For instance pin writings and readings are done with
`digitalWrite()` and `digitalRead()` instead of using the port registers
directly. This makes it easier for the reader to understand how APU parallel bus
works.

## Structure of the Project

Project consist of frontend Python code and backend C++ code for Arduino.
Source codes are located under `frontend` and `backend` respectively. `images`
folder contains images where SVG files are drawn with InkScape. `schema` folder
contains KiCad schema for the project.

Python code is responsible of reading SPC file and uploading it's data to
Arduino over UART serial line. Arduino code is responsible of reading the data
from the serial line and transferring it to APU over it's parallel data lines.
After uploading APU's RAM and it's registers, Python instructs Arduino to tell
APU to start executing the song code. After this APU will keep playing the song.

## How to Use

### Connecting APU to Arduino

Here is APU pinout looking from the top side:

<p align="center">
  <img src="https://github.com/Kazhuu/spc-player/blob/master/images/apu-pinout.png?raw=true" alt="APU Pinout"/>
</p>

Signal and symbol explanations:
* ~ means an active low signal.
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
* MUTE is active low mute output from DSP, it's 0V when muted and 5V when not.

Connect Arduino to APU according to following diagram:

<p align="center">
  <img src="https://github.com/Kazhuu/spc-player/blob/master/images/schema.png?raw=true" alt="Schema"/>
</p>

### Uploading Arduino Code

Coming soon!

### Uploading Song With Python

Coming soon!

## Links

* SNES schematics: https://wiki.superfamicom.org/schematics-ports-and-pinouts
* SPC700 reference: https://wiki.superfamicom.org/spc700-reference
* SPC file ID666 file format: http://www.snesmusic.org/files/spc_file_format.txt

## TODO

* Can play some songs but not all because of the fixed boot code location.
* Comment code with Doxygen when cleaned up.
* Add description of the project to this repository.
* Add schematics.
