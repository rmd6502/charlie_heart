#include <avr/io.h>
#include "pins.h"

static uint16_t ddr_ports[] = { (uint16_t)&DDRA,(uint16_t)&DDRA,(uint16_t)&DDRA,(uint16_t)&DDRA,(uint16_t)&DDRA,(uint16_t)&DDRA,(uint16_t)&DDRA,(uint16_t)&DDRA,(uint16_t)&DDRB,(uint16_t)&DDRB,(uint16_t)&DDRB,(uint16_t)&DDRB };
static uint16_t pin_ports[] = { (uint16_t)&PINA,(uint16_t)&PINA,(uint16_t)&PINA,(uint16_t)&PINA,(uint16_t)&PINA,(uint16_t)&PINA,(uint16_t)&PINA,(uint16_t)&PINA,(uint16_t)&PINB,(uint16_t)&PINB,(uint16_t)&PINB,(uint16_t)&PINB };
static uint16_t port_ports[] = { (uint16_t)&PORTA,(uint16_t)&PORTA,(uint16_t)&PORTA,(uint16_t)&PORTA,(uint16_t)&PORTA,(uint16_t)&PORTA,(uint16_t)&PORTA,(uint16_t)&PORTA,(uint16_t)&PORTB,(uint16_t)&PORTB,(uint16_t)&PORTB,(uint16_t)&PORTB };

void digitalWrite(uint8_t pin, uint8_t value)
{
    if (value == 0) {
        __asm volatile("cbi %0,%1" ::"s"(port_ports[pin]), "r"(value));
    } else {
        __asm volatile("sbi %0,%1" :: "s"(port_ports[pin]), "r"(value) );
    }
}

uint8_t digitalRead(uint8_t pin)
{
    volatile uint8_t *reg = (volatile uint8_t *)pin_ports[pin];
    return (*reg != 0);
}

void pinMode(uint8_t pin, uint8_t direction)
{
    if (direction == 0) {
        __asm volatile("cbi %0,%1" :: "m"(ddr_ports[pin]), "r"(direction));
    } else {
        __asm volatile("sbi %0,%1" :: "m"(ddr_ports[pin]), "r"(direction));
    }
}
