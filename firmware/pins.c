#include <avr/pgmspace.h>
#include <avr/io.h>
#include "pins.h"

#if __AVR_ATtiny841__
static const uint8_t PROGMEM BITNUM[] = { 1<<0,1<<1,1<<4,1<<0,1<<1,1<<2,1<<3,1<<5,1<<6,1<<7,1<<2,1<<3};
static const uint16_t PROGMEM DDR_REG[] = { (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRB, (uint16_t)&DDRB, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRB, (uint16_t)&DDRB, };
static const uint16_t PROGMEM PIN_REG[] = { (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINB, (uint16_t)&PINB, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINB, (uint16_t)&PINB, };
static const uint16_t PROGMEM PORT_REG[] = { (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTB, (uint16_t)&PORTB, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTB, (uint16_t)&PORTB, };
static const uint16_t PROGMEM PUE_REG[] = { (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEB, (uint16_t)&PUEB, (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEA, (uint16_t)&PUEB, (uint16_t)&PUEB, };
#else
static const uint8_t PROGMEM BITNUM[] = { 1<<0, 1<<1,1<<2,1<<3,1<<4, };
static const uint16_t PROGMEM DDR_REG[] = { (uint16_t)&DDRB, (uint16_t)&DDRB, (uint16_t)&DDRB, (uint16_t)&DDRB, (uint16_t)&DDRB };
static const uint16_t PROGMEM PIN_REG[] = { (uint16_t)&PINB, (uint16_t)&PINB, (uint16_t)&PINB, (uint16_t)&PINB, (uint16_t)&PINB };
static const uint16_t PROGMEM PORT_REG[] = { (uint16_t)&PORTB, (uint16_t)&PORTB, (uint16_t)&PORTB, (uint16_t)&PORTB, (uint16_t)&PORTB, };
static const uint16_t PROGMEM PUE_REG[] = { };
#endif
static uint8_t pinModes[sizeof(BITNUM)] = {0};

#define REG(x) (*(volatile uint8_t *)(x))

void pinMode(uint8_t pin, enum Directions direction)
{
#if defined(PUEA)
    if (direction != INPUT_PULLUP && pinModes[pin] == INPUT_PULLUP) {
        REG(PUE_REG[pin]) &= ~pgm_read_byte(&BITNUM[pin]);
    }
#endif
    if (direction == OUTPUT) {
        REG(DDR_REG[pin]) |= pgm_read_byte(&BITNUM[direction]);
    } else {
        REG(DDR_REG[pin]) &= ~pgm_read_byte(&BITNUM[direction]);
#if defined(PUEA)
        if (direction == INPUT_PULLUP) {
            REG(PUE_REG[pin]) |= pgm_read_byte(&BITNUM[pin]);
        }
#endif
    }
    pinModes[pin] = direction;
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    if (value) {
        REG(PORT_REG[pin]) |= pgm_read_byte(&BITNUM[pin]);
    } else {
        REG(PORT_REG[pin]) &= ~pgm_read_byte(&BITNUM[pin]);
    }
}

uint8_t digitalRead(uint8_t pin)
{
    return (REG(PIN_REG[pin]) & pgm_read_byte(&BITNUM[pin])) ? 1 : 0;
}

