#include "SpcHal.hpp"
#include "Arduino.h"

#define PORT_0_PIN 8
#define PORT_1_PIN 9

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

SpcHal::SpcHal() {
    // Read (PB6), write (PB7) and reset (PF7) lines are all active low. So
    // first put read and write to high and last reset line to prevent SPC doing
    // accidental read or writes with data lines. Reset line by default is
    // in tri-state mode.

    // Reset (PF7) pin as output. This causes reset line to go low by default.
    // Thus putting SPC in reset mode and not causing accidental read/writes.
    DDRF |= (1 << PF7);
    // Put read and write lines high. Both signals active low.
    PORTB |= (1 << PB6) | (1 << PB7);
    // Port address 0 (PB4), port address 1 (PB5), read (PB6) and write (PB7)
    // pins as output. Both port address pins will be low by default.
    DDRB |= (1 << PB4) | (1 << PB5) | (1 << PB6) | (1 << PB7);
    // Put reset line high. This will put SPC to normal operation again.
    PORTF |= (1 << PF7);
    dataDirection(false);

    // Equivalent code with Arduino, but a lot slower.
    //pinMode(RESET_PIN, OUTPUT);
    //pinMode(READ_PIN, OUTPUT);
    //pinMode(WRITE_PIN, OUTPUT);
    //pinMode(PORT_0_PIN, OUTPUT);
    //pinMode(PORT_1_PIN, OUTPUT);
    //digitalWrite(READ_PIN, HIGH);
    //digitalWrite(WRITE_PIN, HIGH);
    //digitalWrite(RESET_PIN, HIGH);
    //dataDirection(false);
}

void SpcHal::reset() {
    // Put reset line low and wait a bit until bringing high again.
    PORTF &= ~(1 << PF7);
    for (int i = 0; i < 10; ++i) {
        __asm__ __volatile__("nop");
    }
    PORTF |= (1 << PF7);

    // Equivalent Arduino code.
    // Equivalent code with Arduino, but a lot slower.
    //digitalWrite(RESET_PIN, LOW);
    //digitalWrite(RESET_PIN, HIGH);
}

uint8_t SpcHal::read(uint8_t port) {
    // Set data lines as inputs and port to read from.
    dataDirection(false);
    setPortNumber(port);
    // Set read line low and delay a bit for data to appear in data lines.
    PORTB &= ~(1 << PB6);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    // Read eight data lines.
    uint8_t data = 0;
    data = (((PIND & (1 << PIND6)) >> PIND6) << 0)
         | (((PINC & (1 << PINC7)) >> PINC7) << 1)
         | (((PIND & (1 << PIND1)) >> PIND1) << 2)
         | (((PIND & (1 << PIND0)) >> PIND0) << 3)
         | (((PIND & (1 << PIND4)) >> PIND4) << 4)
         | (((PINC & (1 << PINC6)) >> PINC6) << 5)
         | (((PIND & (1 << PIND7)) >> PIND7) << 6)
         | (((PINE & (1 << PINE6)) >> PINE6) << 7);
    // Set read line high again.
    PORTB |= (1 << PB6);
    return data;

    // Equivalent code with Arduino, but a lot slower.
    //dataDirection(false);
    //setPortNumber(port);
    //digitalWrite(READ_PIN, LOW);
    //uint8_t data = 0;
    //data |= (digitalRead(DATA_0_PIN) << 0);
    //data |= (digitalRead(DATA_1_PIN) << 1);
    //data |= (digitalRead(DATA_2_PIN) << 2);
    //data |= (digitalRead(DATA_3_PIN) << 3);
    //data |= (digitalRead(DATA_4_PIN) << 4);
    //data |= (digitalRead(DATA_5_PIN) << 5);
    //data |= (digitalRead(DATA_6_PIN) << 6);
    //data |= (digitalRead(DATA_7_PIN) << 7);
    //digitalWrite(READ_PIN, HIGH);
    //return data;
}

void SpcHal::write(uint8_t port, uint8_t value) {
    // Set data lines as outputs and port to write to.
    dataDirection(true);
    setPortNumber(port);
    // Put value on data lines.
    // Read registers and zero needed register bits first.
    uint8_t portc = PORTC & ~((1 << PINC6) | (1 << PINC7));
    uint8_t portd = PORTD & ~((1 << PIND0) | (1 << PIND1) | (1 << PIND4) | (1 << PIND6) | (1 << PIND7));
    uint8_t porte = PORTE & ~(1 << PINE6);
    // Write given value bits to correct zeroed bits.
    portc |= (((value >> 1) & 0x01) << PINC7)
           | (((value >> 5) & 0x01) << PINC6);
    portd |= (((value >> 0) & 0x01) << PIND6)
           | (((value >> 2) & 0x01) << PIND1)
           | (((value >> 3) & 0x01) << PIND0)
           | (((value >> 4) & 0x01) << PIND4)
           | (((value >> 6) & 0x01) << PIND7);
    porte |= (((value >> 7) & 0x01) << PINE6);
    // Write modified register values back to real registers.
    PORTC = portc;
    PORTD = portd;
    PORTE = porte;
    // Set write line low and delay a bit to give time for SPC to read it.
    PORTB &= ~(1 << PB7);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    // Set write line high again.
    PORTB |= (1 << PB7);

    // Equivalent code with Arduino, but a lot slower.
    //dataDirection(true);
    //setPortNumber(port);
    //digitalWrite(DATA_0_PIN, value & (1 << 0));
    //digitalWrite(DATA_1_PIN, value & (1 << 1));
    //digitalWrite(DATA_2_PIN, value & (1 << 2));
    //digitalWrite(DATA_3_PIN, value & (1 << 3));
    //digitalWrite(DATA_4_PIN, value & (1 << 4));
    //digitalWrite(DATA_5_PIN, value & (1 << 5));
    //digitalWrite(DATA_6_PIN, value & (1 << 6));
    //digitalWrite(DATA_7_PIN, value & (1 << 7));
    //digitalWrite(WRITE_PIN, LOW);
    //digitalWrite(WRITE_PIN, HIGH);
}

void SpcHal::dataDirection(bool output) {
    if (output) {
        // Set all 8 data lines as output.
        DDRC |= (1 << PC6) | (1 << PC7);
        DDRD |= (1 << PD0) | (1 << PD1) | (1 << PD4) | (1 << PD6) | (1 << PD7);
        DDRE |= (1 << PE6);

        // Equivalent code with Arduino, but a lot slower.
        //pinMode(DATA_0_PIN, OUTPUT);
        //pinMode(DATA_1_PIN, OUTPUT);
        //pinMode(DATA_2_PIN, OUTPUT);
        //pinMode(DATA_3_PIN, OUTPUT);
        //pinMode(DATA_4_PIN, OUTPUT);
        //pinMode(DATA_5_PIN, OUTPUT);
        //pinMode(DATA_6_PIN, OUTPUT);
        //pinMode(DATA_7_PIN, OUTPUT);
    } else {
        // Set all 8 data lines as input.
        DDRC &= ~((1 << PC6) | (1 << PC7));
        DDRD &= ~((1 << PD0) | (1 << PD1) | (1 << PD4) | (1 << PD6) | (1 << PD7));
        DDRE &= ~((1 << PE6));

        // Equivalent code with Arduino, but a lot slower.
        //pinMode(DATA_0_PIN, INPUT);
        //pinMode(DATA_1_PIN, INPUT);
        //pinMode(DATA_2_PIN, INPUT);
        //pinMode(DATA_3_PIN, INPUT);
        //pinMode(DATA_4_PIN, INPUT);
        //pinMode(DATA_5_PIN, INPUT);
        //pinMode(DATA_6_PIN, INPUT);
        //pinMode(DATA_7_PIN, INPUT);
    }
}

void SpcHal::setPortNumber(uint8_t port) {
    uint8_t portb = PORTB & ~((1 << PB4) | (1 << PB5));
    PORTB = portb | ((port & 0x01) << PB4) | (((port >> 1) & 0x01) << PB5);

    // Equivalent code with Arduino, but a lot slower.
    //digitalWrite(PORT_0_PIN, port & 0x01);
    //digitalWrite(PORT_1_PIN, port & 0x02);
}
