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
* [Resources](#resources)
* [Things to Do](#things-to-do)

<!-- vim-markdown-toc -->

## About

This project is heavily based on these two projects:
* https://github.com/emukidid/SNES_APU_SD and
* https://www.caitsith2.com/snes/apu.htm.

This project is a work in progress. Works on some DKC2 songs if you want to
test. Tested and working songs include Stickerbrush Symphony and Crocodile
Cacophony. These two songs are included in the repository already.  Other
original SPC songs can be downloaded from
[Zophar's](https://www.zophar.net/music) website.

So what does this project provide? What other projects are lacking in my opinion
is a good documentation. That is the gap that I want to fill.  Thus this project
code does not aim for the SPC upload speed, instead it aims for the readability
and understandability the code. For instance pin writings and readings are done
with `digitalWrite()` and `digitalRead()` instead of using the port registers
directly. This makes it easier for the reader to understand how APU parallel bus
works for example.

Project is develop on Linux machine and not tested on Windows. If someone finds
something to fix please create issue about it. PRs are also welcome.

## Structure of the Project

Project consist of frontend Python code and backend C++ code for Arduino.
Source codes are located under `frontend` and `backend` respectively. `images`
folder contains images where SVG files are drawn with InkScape. `schemas` folder
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
* **~** means an active low signal.
* **PA7** is port address bit 7, connected to active low chip select (~CS) pin on
    the SPC700 chip. Pull this low to GND to enable the chip.
* **PA6** is port address bit 6, connected to chip select (CS) pin on the SPC700 chip.
    Pull this high to 5V to enable the chip.
* **PA0** and **PA1** are port address bits 0 and 1. Used to select which port from 0 to
    3 to write or read.
* From **D0** to **D7** are bidirectional data lines for input and output a single byte.
* **RD** is active low read, APU will write data to lines D0 to D7 when pulled low.
* **WR** is active low write, APU will read data from lines D0 to D7 when pulled low.
* **SMPCK** is a clock output from DSP with average clock rate of 2.23 MHz.
* **MUTE** is active low mute output from DSP, it's 0V when DSP is muted and 5V when
    not.

Connect Arduino to APU according following diagram:

<p align="center">
  <img src="https://github.com/Kazhuu/spc-player/blob/master/images/schema.png?raw=true" alt="Schema"/>
</p>

### Uploading Arduino Code

Arduino backend code uses [PlatformIO CLI](https://platformio.org/) tool for
managing the project and its dependencies instead of Arduino IDE. So to compile
and upload the code you need to download PlatformIO and build the project using
it. First download and install it from aforementioned website. Both IDE and CLI
are fine. Generally I prefer CLI over IDE, so the following is also written
using it.

After installing PlatformIO go to backend folder and simply run:
```
pio run
```
This should install needed Arduino toolchain, compile the source code and upload
it to the Arduino board.

Before moving to use the Python code you can test that everything is connected
and working correctly by running following command in the backend folder:
```
pio run --target monitor
```
This will open serial monitor. Arduino code should print `1` on the serial if
it's able to reset the APU successfully and `0` when it fails. In that case
check your connections and try again.

### Uploading Song With Python

Project uses version 3.x of Python and uses pySerial library to talk to Arduino
over USB serial line. First install pySerial with pip by running following
command. Remember to use pip3 if you are using Debian based system like Ubuntu.
```
pip install pyserial
```

I've included two Donkey Kong Country 2 songs in the frontend folder that are
confirmed to work. You can use these to quickly test that your setup is working.
To upload the song with Python you need to know your serial port to which
Arduino is connected to. In Linux it will be something similar like
`/dev/ttyACM0` and on Windows `COM4`. To upload the song in the frontend folder
run following
```
python main.py <serial-port> dkc2-stickerbrush-symphony.spc
```

If working correctly it should print something following and song should start
playing.
```
opened port /dev/ttyACM0
write CPU registers successful
write DSP registers successful
write first page RAM successful
write second page RAM successful
100%
write rest of the RAM successful
SPC exexution started successfully, uploading took 17.71s
```

Enjoy some awesome SNES music!

## Resources

Here are the most important resources what I've collected over time when studying
for this project. I put them here in hope that someone else working on the same
issues find them useful and don't have to waste time finding them.

Most important source of information being original SNES development manual part
1 which can be downloaded from
[here](http://folk.uio.no/sigurdkn/snes/snes_manual1.pdf).
On the SNES development manual most important pages are 153-179 for general sound system
and DSP, 180-188 for SPC700 CPU and 237-240 for IPL ROM boot code procedure.

Rest of the resources to be read alongside the development manual:

* SPC700 reference: https://wiki.superfamicom.org/spc700-reference
* SNES schematics: https://wiki.superfamicom.org/schematics-ports-and-pinouts
* SPC file ID666 file format: http://www.snesmusic.org/files/spc_file_format.txt
* SPC2 v1.1 format: http://blog.kevtris.org/blogfiles/spc2_file_specification_v1.txt
* SPC2 v1.3 format: https://www.caitsith2.com/snes/spc2_file_specification_v1.3.txt
* ASCII based APU manual: https://web.archive.org/web/20090106230547/http://www.alpha-ii.com/snesmusic/files/spc700_apu_manual.txt
* SPC 700 Documentation by Gau of the Veldt: http://emureview.ztnet.com/developerscorner/SoundCPU/spc.htm
* SPC 700 programming tutorial: https://en.wikibooks.org/wiki/Super_NES_Programming/Loading_SPC700_programs
* APU hardware: http://www.snesmusic.org/files/spc700.html
* Anomie's in depth SPC700 documentation: http://www.romhacking.net/documents/197/

## Things to Do

* Can play some songs but not all because of the fixed boot code location.
* Comment code with Doxygen when cleaned up.
* Add support to newer SPC formats.
