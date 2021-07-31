# SPC Player

SNES music file player in your browser with original hardware and Arduino.

![arduino-spc](./images/readme-image.png?raw=true "APU and Arduino connected")

## Table of Contents

<!-- vim-markdown-toc GFM -->

* [What is this?](#what-is-this)
* [Structure of the Project](#structure-of-the-project)
* [How to Use](#how-to-use)
  * [Connecting APU to Arduino](#connecting-apu-to-arduino)
  * [Uploading Arduino Code](#uploading-arduino-code)
  * [Browser Frontend](#browser-frontend)
  * [Python Frontend](#python-frontend)
* [Debugging APU Connections](#debugging-apu-connections)
* [How it Works?](#how-it-works)
* [Using Other Arduino Boads](#using-other-arduino-boads)
* [Resources](#resources)
* [Future Work](#future-work)

<!-- vim-markdown-toc -->

## What is this?

Video in action [here](https://www.youtube.com/watch?v=2sleZUMQwSA).

With this project you are able to play original SNES SPC audio files from your
browser with original hardware. All you need is Arduino Micro and original SNES
Audio Processing Unit (APU). Playing songs also works from Python command-line
tool instead of a browser. If you are interested how this project works check
sections [How it Works?](#how-it-works) and [Resources](#resources).

For playing you also need to download your favorite SNES SPC music tracks. These
can be downloaded from [Zophar's](https://www.zophar.net/music) website. Search
for a game and download original music files. Files will end with `.spc` ending.
I've included one Donkey Kong Country 2 song in root of the project that you can
use for quick testing.

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
skip to the section [Python Frontend](#python-frontend).

```
pio run --environment serial
```

### Browser Frontend

Browser based code uses WebUSB for communicating with Arduino. So check your
browser compatibility for it
[here](https://developer.mozilla.org/en-US/docs/Web/API/USB#browser_compatibility).
For example Chrome and supports it but Firefox does not at time of writing.

Browser player can play multiple songs more like a real player compared to the
Python frontend which only can play one song at the time. Browser frontend lives
under `frontend-web` folder. Go to that folder and run following commands under
it.

Application is developed with React and packages are managed with Yarn. So
install yarn first for your system from
[here](https://yarnpkg.com/getting-started/install). After installing install
all required packages with:

```
yarn install
```

After installing run the application with

```
yarn start
```

and go to `localhost:3000` with your browser.

With the player you first need to give permission to your use your USB devices
and select the correct one to connect to. After connecting successfully you are
presented with the player. Player contains some buttons to debug your setup and
then button to upload SPC files. Multiple files can be uploaded at the same
time. One DKC2 track is include in the project root for testing purposes. When
you've uploaded some songs, then just click on any table line to start uploading
the song and it should start playing. After the song ends the next song will be
played automatically. UI could be better but that is the task for the future...

Enjoy some awesome SNES music! If you have some problems. Make sure APU is
connected to Arduino correctly. For this check [Debugging APU
Connections](#debugging-apu-connections).

### Python Frontend

Python frontend can only upload one song at the time. If you want more player
like experience with automatically playing the next song. Have a look at browser
based frontend instead. Python frontend lives under `frontend-python` folder.
Go to that folder and run following commands inside it.

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

APU itself is a standalone CPU with it's own RAM and DSP for producing sound.
Usually referred as SCP700 CPU
([wiki](https://en.wikipedia.org/wiki/Super_Nintendo_Entertainment_System#Audio)).
Then SPC music file is a snapshot of the state of that system just before the
song starts to play. The snapshot includes details like values of all CPU and
DSP register and whole RAM content.

When SPC700 boots up it will run it's Initial Program Loader (IPL) code. This
code is responsible for communicating over parallel lines with SNES main CPU and
transferring bytes to SCP700 RAM. It's also responsible of starting execution of
uploaded code. In other words meaning when song should start playing.

In this project's case Arduino is talking with IPL program instead of SNES main
CPU. Frontend program on PC (Python or browser) is reading SPC700 RAM bytes from
SPC file, transferring them to Arduino over USB serial and Arduino then
transferring them with IPL program to SPC700 CPU RAM over it's parallel lines.
That's in a nutshell how the whole process works. After all RAM bytes and
register values have been transferred. Arduino will inject small custom bootcode
to SPC700 RAM and tell IPL to start execution from there. The bootcode is
responsible for restoring last register and timer values and then jump to actual
song program execution. Bootcode can be found from this file
`backend/src/SpcPlayer.cpp`.  Bootcode is filled up with details when whole RAM
transferring is in progress.  After the song starts playing the parallel line
communication is dependant on the song and game. Only way to stop the song at
this point is just to reset whole APU itself with the reset line.

One interesting detail which change from game to game. When program is running
and music is playing. Program is still reading for values from parallel lines.
If something is written there from SNES CPU side, then some sound effects are
played. For example in Super Mario World Mario's jumping sound is played while
the song is playing.

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

## Future Work

Can play all songs that I've tested but most likely not all SPC files work. At
the moment only supports SPC file format v0.30. In future more formats could be
supported.

If you stumble upon anything please file an issue about it. PRs are also
welcome, thanks!
