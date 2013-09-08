// Copyright (c) 2012 Robert M. Diamond All Rights Reserved.
//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "charlie.h"

// buffer bytes, only lower 4 bits are significant
static volatile uint8_t *buffer;

static volatile uint8_t timer = 0;
static volatile uint8_t num_rows, num_columns;
static volatile uint8_t *bufPtr = 0;
static volatile LedPins *ledPtr = 0;
volatile uint16_t cycle_count;
volatile uint8_t button_state = 0;
static volatile uint8_t button_pin = 0;

static volatile LedPins *ledPins;

void charlie_init(uint8_t _num_rows, uint8_t _num_columns, LedPins *led_pins, volatile uint8_t *_buffer,uint8_t _button_pin) {
    // setup the buffer
    num_rows = _num_rows;
    num_columns = _num_columns;
    ledPins = led_pins;
    buffer = _buffer;
    button_pin = _button_pin;

    bufPtr = (uint8_t *)buffer + num_rows * num_columns;
    ledPtr = ledPins + num_rows * num_columns;
    timer = 15;
	cycle_count = 0;

    PRR = ~(_BV(PRTIM0));
    MCUCR &= ~(1 << PUD);

    cli();
    // initialize the timer and interrupt
    TCNT0 = 0;
    OCR0A = 40;
    // interrupt at count 63, so 32khz interrupt
    TIMSK |= _BV(OCIE0A);
    TIFR = _BV(OCF0A);
    // Start the timer at PCK/8, so 2 MHz timer clock
    // with 16 shades, that means 7812.5 frames/sec
    // and the interrupt routine has 256 cycles to run, including interrupt and gcc overhead
    TCCR0A = (2 << WGM00);
    TCCR0B = (2 << CS00);

    // Set up the button
    GIMSK = _BV(PCIE);
    PCMSK = _BV(PCINT2);
    GIFR = _BV(PCIF);

    sei();
}

void prepare_for_sleep()
{
    timer = 16;
    TCCR0B = 0;
}

void return_from_sleep()
{
    TCCR0B = (2 << CS00);
}

ISR(TIMER0_COMPA_vect) {

    --bufPtr;
    --ledPtr;

    if (timer == 16) {
        button_state = (PINB >> button_pin) & 1;
        --timer;
        return;
    }
    if (*bufPtr >= timer) {
        DDRB = _BV(ledPtr->highpin) | _BV(ledPtr->lowpin);
        PORTB = _BV(ledPtr->highpin);
    } else {
        DDRB = 0;
        PORTB = 0;
    }

    if (bufPtr == buffer) {
        bufPtr = (uint8_t *)buffer + num_rows * num_columns;
        ledPtr = ledPins + num_rows * num_columns;
        --timer;
        if (timer == 0) {
	        timer = 16;
	        ++cycle_count;
            DDRB = 0;
            PORTB = 1 << button_pin;
        }
    }
}

ISR(PCINT0_vect)
{
    if (timer != 16) {
        return;
    }
    button_state = (PINB >> button_pin) & 1;
}
