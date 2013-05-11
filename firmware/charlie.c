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

    // initialize the timer and interrupt
    TCNT1 = 0;
    OCR1A = 255;
    cli();
    // interrupt at count 16, so 62.5khz interrupt
    TIMSK |= _BV(TOIE1);
    TIFR = _BV(TOV1);
    // Start the timer at PCK/4, so 2 MHz timer clock
    // with 16 shades, that means 7812.5 frames/sec
    // and the interrupt routine has 256 cycles to run, including interrupt and gcc overhead
    TCCR1 = (2 << CS10);

    bufPtr = (uint8_t *)buffer + num_rows * num_columns;
    ledPtr = ledPins + num_rows * num_columns;
    timer = 15;
	cycle_count = 0;

    sei();
}

ISR(TIMER1_OVF_vect) {

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
