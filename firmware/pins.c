#include <avr/io.h>
#include "pins.h"

static uint8_t BITNUM[] = { 1<<0, 1<<1,1<<2,1<<3,1<<4,1<<5,1<<6,1<<7, 1<<0, 1<<1,1<<2,1<<3 };
static uint16_t DDR_REG[] = { (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRA, (uint16_t)&DDRB, (uint16_t)&DDRB, (uint16_t)&DDRB, (uint16_t)&DDRB };
static uint16_t PIN_REG[] = { (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINA, (uint16_t)&PINB, (uint16_t)&PINB, (uint16_t)&PINB, (uint16_t)&PINB };
static uint16_t PORT_REG[] = { (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTA, (uint16_t)&PORTB, (uint16_t)&PORTB, (uint16_t)&PORTB, (uint16_t)&PORTB };


void pinMode(uint8_t pin, enum Directions direction)
{
    if (direction) {
        DDR_REG[pin] |= BITNUM[direction];
    } else {
        DDR_REG[pin] &= ~BITNUM[direction];
    }
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

