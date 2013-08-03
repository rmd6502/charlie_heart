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

static volatile LedPins *ledPins;

void charlie_init(uint8_t _num_rows, uint8_t _num_columns, LedPins *led_pins, volatile uint8_t *_buffer) {
    // setup the buffer
    num_rows = _num_rows;
    num_columns = _num_columns;
    ledPins = led_pins;
    buffer = _buffer;

    bufPtr = (uint8_t *)buffer + num_rows * num_columns;
    ledPtr = ledPins + num_rows * num_columns;
    timer = 15;
	cycle_count = 0;

    PRR = ~(_BV(PRTIM0));

    cli();
    // initialize the timer and interrupt
    TCNT0 = 0;
    OCR0A = 63;
    // interrupt at count 63, so 32khz interrupt
    TIMSK |= _BV(OCIE0A);
    TIFR = _BV(OCF0A);
    // Start the timer at PCK/4, so 2 MHz timer clock
    // with 16 shades, that means 7812.5 frames/sec
    // and the interrupt routine has 256 cycles to run, including interrupt and gcc overhead
    TCCR0A = (2 << WGM00);
    TCCR0B = (2 << CS00);

    sei();
}

ISR(TIMER0_COMPA_vect) {

    --bufPtr;
    --ledPtr;

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
	        timer = 15;
	        ++cycle_count;
        }
    }
}
