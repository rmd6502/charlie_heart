#include <avr/io.h>
#include "pins.h"

#if __AVR_ATtiny841__
static uint8_t BITNUM[] = { 1<<0,1<<1,1<<4,1<<0,1<<1,1<<2,1<<3,1<<5,1<<6,1<<7,1<<2,1<<3};
static uint16_t DDR_REG[] = { (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRB, (uint16_t)&DDRB, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRB, (uint16_t)&DDRB, };
static uint16_t PIN_REG[] = { (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINB, (uint16_t)&PINB, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINB, (uint16_t)&PINB, };
static uint16_t PORT_REG[] = { (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTB, (uint16_t)&PORTB, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTB, (uint16_t)&PORTB, };
static uint16_t PUE_REG[] = { (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEB, (uint16_t)&PUEB, (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEB, (uint16_t)&PUEB, };
#else
static uint8_t BITNUM[] = { 1<<0, 1<<1,1<<2,1<<3,1<<4, };
static uint16_t DDR_REG[] = { (uint16_t)&DDRB, (uint16_t)&DDRB, (uint16_t)&DDRB, (uint16_t)&DDRB, (uint16_t)&DDRB };
static uint16_t PIN_REG[] = { (uint16_t)&PINB, (uint16_t)&PINB, (uint16_t)&PINB, (uint16_t)&PINB, (uint16_t)&PINB };
static uint16_t PORT_REG[] = { (uint16_t)&PORTB, (uint16_t)&PORTB, (uint16_t)&PORTB, (uint16_t)&PORTB, (uint16_t)&PORTB, };
static uint16_t PUE_REG[] = { };
#endif
static uint8_t pinModes[sizeof(BITNUM)] = {0};


void pinMode(uint8_t pin, enum Directions direction)
{
#if defined(PUEA)
    if (direction != INPUT_PULLUP && pinModes[pin] == INPUT_PULLUP) {
        PUE_REG[pin] &= ~BITNUM[pin];
    }
#endif
    if (direction == OUTPUT) {
        DDR_REG[pin] |= BITNUM[direction];
    } else {
        DDR_REG[pin] &= ~BITNUM[direction];
#if defined(PUEA)
        if (direction == INPUT_PULLUP) {
            PUE_REG[pin] |= BITNUM[pin];
        }
#endif
    }
    pinModes[pin] = direction;
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    if (value) {
        PORT_REG[pin] |= BITNUM[pin];
    } else {
        PORT_REG[pin] &= ~BITNUM[pin];
    }
}

uint8_t digitalRead(uint8_t pin)
{
    return (PIN_REG[pin] & BITNUM[pin]) ? 1 : 0;
}

