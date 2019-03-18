#include "Arduino.h"
#include "SpcPlayer.h"

#define PORT_0_PIN 9
#define PORT_1_PIN 8

#define READ_PIN 10
#define WRITE_PIN 11

#define DATA_0_PIN 12
#define DATA_1_PIN 13
#define DATA_2_PIN 2
#define DATA_3_PIN 3
#define DATA_4_PIN 4
#define DATA_5_PIN 5
#define DATA_6_PIN 6
#define DATA_7_PIN 7

#define RESET_PIN A0


SpcPlayer spcPlyer(READ_PIN, WRITE_PIN, RESET_PIN);

uint8_t data[] = {
  0xC4, 0xF7,       //        Mov [0F5h], A
  0xC4, 0xF6,       //        Mov [0F6h], A
};

static unsigned char DSPdata[] =
{ //For loading the 128 byte DSP ram. DO NOT CHANGE.
  0xC4, 0xF2,       //START:  Mov [0F2h], A
  0x64, 0xF4,       //LOOP:   Cmp A, [0F4h]
  0xD0, 0xFC,       //        Bne LOOP
  0xFA, 0xF5, 0xF3, //        Mov [0F3h], [0F5h]
  0xC4, 0xF4,       //        Mov [0F4h], A
  0xBC,             //        Inc A
  0x10, 0xF2,       //        Bpl START

  0x8F, 0xFF, 0xFC, //      Mov [0FCh], #timer_2
  0x8F, 0xFF, 0xFB, //      Mov [0FBh], #timer_1
  0x8F, 0x4F, 0xFA, //      Mov [0FAh], #timer_0

  0xCD, 0xF5,       //      Mov X, #stack_pointer
  0xBD,             //      Mov SP, X

  0x2F, 0xAB,       //        Bra 0FFC9h  ;Right when IPL puts AA-BB on the IO ports and waits for CC.
};

void setup() {
    Serial.begin(9600);
    spcPlyer.setPortPins(PORT_0_PIN, PORT_1_PIN);
    spcPlyer.setDataPins(DATA_0_PIN, DATA_1_PIN, DATA_2_PIN, DATA_3_PIN, DATA_4_PIN, DATA_5_PIN, DATA_6_PIN, DATA_7_PIN);
    spcPlyer.init();
    //spcPlyer.writeBlock(10, data, 260);
    spcPlyer.writeBlock(0x0002, data, sizeof(data));
    Serial.println("write done");
    //spcPlyer.start(0x0002);
    spcPlyer.start(0xFFC0);
    Serial.println("spc start");
    spcPlyer.write(1, 1);
    spcPlyer.write(2, 0x02);
    spcPlyer.write(3, 0x00);
    spcPlyer.write(0, 0x04);
    //spcPlyer.writeBlock(10, data, 11);
}

void loop() {
    for (int i = 0; i < 4; i++) {
        Serial.print("port");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(spcPlyer.read(i), HEX);
        delay(500);
    }
}
