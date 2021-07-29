# SPC Player

SNES SPC music file player in your browser with original hardware and Arduino.

TODO: Update picture.
![arduino-spc](./images/arduino-apu-connected.jpg?raw=true "APU and Arduino connected")

## Table of Contents

<!-- vim-markdown-toc GFM -->

* [What is this?](#what-is-this)
* [Structure of the Project](#structure-of-the-project)
* [How to Use](#how-to-use)
  * [Connecting APU to Arduino](#connecting-apu-to-arduino)
  * [Uploading Arduino Code](#uploading-arduino-code)
  * [Browser SPC Player](#browser-spc-player)
  * [Uploading Song With Python](#uploading-song-with-python)
* [Debugging APU Connections](#debugging-apu-connections)
* [How it Works?](#how-it-works)
* [Using Other Arduino Boads](#using-other-arduino-boads)
* [Resources](#resources)

<!-- vim-markdown-toc -->

## What is this?

With this project you are able to play original SNES SPC audio files from your
browser with original hardware. All you need is Arduino Micro and original SNES
Audio Processing Unit (APU). Playing songs also works from Python command-line
tool instead of a browser.

You also need to download your favorite SNES SPC music tracks. These can be
downloaded from [Zophar's](https://www.zophar.net/music) website. Search for a
game and download original music files. Files will end with `.spc` ending.  I've
included one Donkey Kong Country 2 song in root of the project that you can use
for quick testing.

Project is develop on Linux machine and not tested on other platforms. Although
it should work because of multiplatform tools used. If you stumble upon bugs,
please file an issue here in GitHub.

## Structure of the Project

Project consist of two music track uploading frontend tools (browser and Python)
and Arduino code for communicating with APU and with your computer. Browser
based frontend code lives under `frontend-web` folder and Python code under
`frontend-python` folder. Arduino code lives under `backend` folder.

Project also contains InkScape SVG image files under `images` folder and KiCad
schemas under `schemas` folder.

## How to Use

### Connecting APU to Arduino

First you need to connect APU to Arduino. Here is APU pinout looking from the
top side of the unit:

![api-pinout](./images/apu-pinout.png?raw=true "APU Pinout")

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
* **RESET** is active low reset line. When held low SPC is in reset mode.
* **SMPCK** is a clock output from DSP with average clock rate of 2.23 MHz.
* **MUTE** is active low mute output from DSP, it's 0V when DSP is muted and 5V when
    not.

Here is how you need to connect APU to Arduino:

![scema](./images/schema.png?raw=true "Schema")

### Uploading Arduino Code

Arduino based backend code uses [PlatformIO CLI](https://platformio.org/) tool
for managing the project and its dependencies instead of Arduino IDE. So to
compile and upload the code you need to download PlatformIO and build the
project using it. First download and install it from aforementioned website.
Installing either IDE or CLI is fine. Generally I prefer CLI over IDE, so the
following instructions are written with CLI.


After installing PlatformIO go to `backend` folder. If you are using browser
frontend, you can simply run:

```
pio run
```

This will download needed tool and libraries. Compile the code and upload it to
the board. Also you should see popup window from Chrome that supported USB
device is connected.

By default code uses WebUSB serial to communicate with the browser. Both WebUSB
and normal serial cannot be used at the same time. If you are using Python
frontend or normal serial, then you need to use following command instead and
skip to the section [Uploading Song With Python](#uploading-song-with-python).

```
pio run --environment serial
```

### Browser SPC Player

TODO

### Uploading Song With Python

Python frontend lives under `frontend-python` folder. Go to that folder and run
following commands inside it.

Project uses version 3.x of Python and uses pySerial library to talk to Arduino
over USB serial line. First install pySerial with pip by running following
command. Remember to use `pip3` and `python3` commands if you are using Debian
based system like Ubuntu.

```
pip install pyserial
```

To upload the song with Python you need to know your serial port to which
Arduino is connected to. In Linux it will be something similar like
`/dev/ttyACM0` and on Windows `COM4`. To list available serial ports run:

```
python main.py -l
```

Copy the serial port and replace the following command `<serial-port>` with it.
Now upload the song with:

```
python main.py <serial-port> ../dkc2-stickerbrush-symphony.spc
```

If everything is working correctly Python should print following and song will
start playing.

```
opened port /dev/ttyACM0
write CPU registers successful
write DSP registers successful
write first page RAM successful
write second page RAM successful
100%
write rest of the RAM successful
SPC execution started successfully, uploading took 9.22s
```

Enjoy some awesome SNES music! If you have some problems. Make sure APU is
connected to Arduino correctly. For this check [Debugging APU
Connections](#debugging-apu-connections) below.

## Debugging APU Connections

You can test that APU is connected to Arduino correctly. You need to use normal
serial line, not WebUSB serial. After uploading the code go to backend folder
and run:

```
pio run --target monitor
```

This will open serial monitor. Press `R` to send R character. Arduino should
respond with `1` if it's able to reset the APU successfully and `0` when it
fails. In that case check your connections and try again.

## How it Works?

Frontend code like Python is responsible of reading SPC file and uploading it's
data to Arduino over serial line. Arduino code is responsible of reading the
data from the serial line and transferring it to APU over it's parallel data
lines.  After uploading APU's RAM and it's registers, Python instructs Arduino
to tell APU to start executing the song code. After this APU will keep playing
the song until reset.

PlatformIO environments are used to determine between WebUSB and normal
serial. You can check `backend/platformio.ini` file how it's done. For example
environment `webusb` defines `USE_WEBUSB_SERIAL`. Which in turn is used in code to
determine which serial to use if it's defined.

## Using Other Arduino Boads

You can also port this code to work on other Arduino boards. Keep in mind that
WebUSB serial cannot be used with all boards. For WebUSB serial support check
[this](https://github.com/webusb/arduino).

For porting the code to different Arduino board. Take a look at file
`backend/src/SpcHal.cpp`. Which is Hardware Abstraction Layer (HAL) for the
platform. Here you can find the low level data communication functions.
Functions are directly using pin IO registers for speed purposes. But the
equivalent Arduino code is also there commented out which uses `digitalRead` and
`digitalWrite` functions. Which is much easier to understand. Note that using
`digitalRead` and `digitalWrite` functions the song upload will be a lot slower!

## Resources

Here are the most important resources what I've collected over time when studying
for this project. I put them here in hope that someone else working on the same
issues find them useful and don't have to waste time finding them.

Most important source of information being original SNES development manual part
1 which can be downloaded from this link:
http://folk.uio.no/sigurdkn/snes/snes_manual1.pdf.
SNES development manual most important pages include:
* 153-179 for general sound system and DSP,
* 180-188 for SPC700 CPU and
* 237-240 for IPL ROM boot code procedure.

Development manual can be quite hard to read for the first time so that's why
it's recommended to read following resources alongside with the manual to put
beef around the bones:

* SPC700 reference: https://wiki.superfamicom.org/spc700-reference
* SPC file ID666 file format: http://www.snesmusic.org/files/spc_file_format.txt
* SPC2 v1.1 format: http://blog.kevtris.org/blogfiles/spc2_file_specification_v1.txt
* SPC2 v1.3 format: https://www.caitsith2.com/snes/spc2_file_specification_v1.3.txt
* ASCII based APU manual: https://web.archive.org/web/20090106230547/http://www.alpha-ii.com/snesmusic/files/spc700_apu_manual.txt
* SPC 700 Documentation by Gau of the Veldt: http://emureview.ztnet.com/developerscorner/SoundCPU/spc.htm
* SPC 700 programming tutorial: https://en.wikibooks.org/wiki/Super_NES_Programming/Loading_SPC700_programs
* APU hardware: http://www.snesmusic.org/files/spc700.html
* Anomie's in depth SPC700 documentation: http://www.romhacking.net/documents/197/
* SNES schematics: https://wiki.superfamicom.org/schematics-ports-and-pinouts

Also when I studied for this project I used following project source code for
studying purposes https://www.caitsith2.com/snes/apu.htm. Which can also be
found from GitHub: https://github.com/emukidid/SNES_APU_SD.
