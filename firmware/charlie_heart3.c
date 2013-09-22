#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>

#include "charlie.h"

#define NUM_PINS 20
#define BUTTON_PIN 2

volatile uint8_t buffer[NUM_PINS] = {0};

void initialize(void);

uint16_t map(uint32_t val, uint16_t minval, uint16_t maxval, uint16_t minout, uint16_t maxout)
{
    if (maxval != minval && val >= minval) {
        return ((val - minval) * (maxout - minout + 1) / (maxval - minval)) + minout;
    } else {
        return 0;
    }
}

int main() {
    uint16_t last_cycle_count = 0;
    uint16_t min = 1023, max = 0;
    uint8_t measured = 0;
    initialize();
    for (int i=0; i < NUM_PINS; ++i) buffer[i] = 8;
    while(1) {
        if (cycle_count != last_cycle_count) {
            if (!measured) {
                if (val > max) max = val;
                if (val < min) min = val;
                uint16_t newval = map(val, min, max, 0, 15);
                for (int i=0; i < NUM_PINS; ++i) buffer[i] = newval;
                measured = 1;
                last_cycle_count = cycle_count;
            }
        } else {
            measured = 0;
        }
    }
}

void initialize(void) {
    // initialize charlieplexing
    static LedPins ledPins[NUM_PINS];
    uint8_t count = 0;
    for (uint8_t i=0; i < 4; ++i) {
        for (uint8_t j = i+1; j < 5; ++j) {
            ledPins[count].highpin = j;
            ledPins[count].lowpin = i;
            ledPins[19 - count].highpin = i;
            ledPins[19 - count].lowpin = j;
            ++count;
        }
    }
    charlie_init(1,NUM_PINS,ledPins,buffer,BUTTON_PIN);
    set_sleep_mode(SLEEP_MODE_IDLE);
    // set up the ADC
}
