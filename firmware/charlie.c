// Copyright (c) 2012 Robert M. Diamond All Rights Reserved.
//
#include <avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "charlie.h"
#include "pins.h"

// buffer bytes, only lower 4 bits are significant
static volatile uint8_t *buffer;

static volatile uint8_t timer = 0;
static volatile uint8_t num_rows, num_columns;
static volatile uint8_t *bufPtr = 0;
static volatile LedPins *ledPtr = 0;
volatile uint16_t cycle_count;
volatile uint8_t button_state = 0;
static volatile uint8_t button_pin = 0;
static volatile uint8_t sleep_state = 0;
volatile uint16_t val = 0;
static volatile uint8_t numPins = 0;

static volatile LedPins *ledPins;

void charlie_init(uint8_t _num_rows, uint8_t _num_columns, LedPins *led_pins, volatile uint8_t *_buffer,uint8_t _button_pin)
{
	// setup the buffer
	num_rows = _num_rows;
	num_columns = _num_columns;
    numPins = num_rows * num_columns;
	ledPins = led_pins;
	buffer = _buffer;
	button_pin = _button_pin;
	pinMode(button_pin, INPUT_PULLUP);

	bufPtr = (uint8_t *)buffer + num_rows * num_columns;
	ledPtr = ledPins + num_rows * num_columns;
	timer = 15;
	cycle_count = 0;

	PRR = ~(_BV(PRTIM0) | _BV(PRADC));
    #if defined(PUD)
	MCUCR &= ~(1 << PUD);
    #endif

	cli();
	// interrupt at 50kHz with 1/2MHz clock
	OCR0A = (F_CPU/400000);
	// initialize the timer and interrupt
	TCNT0 = 0;
	// Start the timer at PCK/8, so 2 MHz timer clock
	// with 16 shades, that means 3125 frames/sec
	// and the interrupt routine has 320 cycles to run, including interrupt and gcc overhead
#if __AVR_ATtiny841__
	TIMSK0 |= _BV(OCIE0A);
	TIFR0 = _BV(OCF0A);

	// Set up the button
	GIMSK = _BV(PCIE0);
	PCMSK0 = _BV(3);
#else
	TIMSK |= _BV(OCIE0A);
	TIFR = _BV(OCF0A);

	// Set up the button
	GIMSK = _BV(PCIE);
	PCMSK = _BV(PCINT2);
	GIFR = _BV(PCIF);
#endif
	TCCR0A = (2 << WGM00);
	TCCR0B = (2 << CS00);

	sei();
}

void prepare_for_sleep()
{
	sleep_state = 1;
	timer = 16;
	TCCR0B = 0;
#if defined(DDRA)
    DDRA = 0;
#endif
	DDRB = 0;
    pinMode(button_pin, INPUT_PULLUP);
}

void return_from_sleep()
{
	sleep_state = 0;
	TCCR0B = (2 << CS00);
}

ISR(TIMER0_COMPA_vect) {

    if (ledPtr - ledPins < numPins) {
        pinMode(ledPtr->lowpin, INPUT);
        pinMode(ledPtr->highpin, INPUT);
    }
	--bufPtr;
	--ledPtr;

	if (timer == 16) {
		button_state = digitalRead(button_pin);
		--timer;
		return;
	}
	if (*bufPtr >= timer) {
        pinMode(ledPtr->lowpin, OUTPUT);
        pinMode(ledPtr->highpin, OUTPUT);
        digitalWrite(ledPtr->highpin, 1);
        digitalWrite(ledPtr->lowpin, 0);
	} else {
        pinMode(ledPtr->lowpin, INPUT);
        pinMode(ledPtr->highpin, INPUT);
	}

	if (bufPtr == buffer) {
		bufPtr = (uint8_t *)buffer + numPins;
		ledPtr = ledPins + numPins;
		--timer;
		if (timer == 0) {
			timer = 16;
			++cycle_count;
            pinMode(button_pin, INPUT_PULLUP);
		}
	}
}

ISR(PCINT0_vect)
{
	if (!sleep_state) {
		return;
	}
    button_state = digitalRead(button_pin);
}
