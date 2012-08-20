// Copyright (c) 2012 Robert M. Diamond All Rights Reserved.
//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include "charlie.h"

// buffer bytes, only lower 4 bits are significant
volatile uint8_t *buffer;

volatile uint8_t timer = 0;
uint8_t num_rows, num_columns;

LedPins *ledPins;

void charlie_init(uint8_t _num_rows, uint8_t _num_columns, LedPins *led_pins) {
    // setup the buffer
    num_rows = _num_rows;
    num_columns = _num_columns;
    ledPins = led_pins;
    buffer = (uint8_t *)calloc(num_rows,  num_columns);

    // initialize the timer and interrupt
    TCNT1 = 0;
    OCR1A = 255;
    cli();
    // interrupt at count 256, so 7812.5 hz interrupt
    TIMSK |= _BV(TOIE1);
    TIFR = _BV(TOV1);
    // Start the timer at PCK/4, so 2 MHz timer clock
    // with 16 shades, that means ~488 frames/sec
    // and the interrupt routine has 1024 cycles to run, including interrupt and gcc overhead
    TCCR1 = (3 << CS10);
    sei();
}

ISR(TIMER1_OVF_vect) {
    volatile uint8_t *bufPtr = buffer;
    LedPins *ledPtr = ledPins;
    for (uint8_t i=0; i < num_rows; ++i) {
        for (uint8_t j=0; j < num_columns; ++j) {
            if (*bufPtr >= timer) {
                DDRB = _BV(ledPtr->highpin) | _BV(ledPtr->lowpin);
                PORTB = _BV(ledPtr->highpin);
            } else {
                DDRB = 0;
                PORTB = 0;
            }
            ++bufPtr;
            ++ledPtr;
        }
    }
    ++timer;
    if (timer == 16) timer = 0;
}
