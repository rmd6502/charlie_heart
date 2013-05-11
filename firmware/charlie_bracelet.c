#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "charlie.h"

#define NUM_PINS 20

volatile uint8_t buffer[NUM_PINS] = {0};

void initialize(void);

int8_t mod(int8_t q, int8_t d) {
    int8_t r = q % d;
    if (r < 0) r += d;
    return r;
}

int main() {
    uint16_t last_cycle_count = 0;
    // state goes from 0 to 21
    int8_t state = 0;
    int8_t dir = 1;
    int8_t count = 0;
    initialize();
    while(1) {
        // test condition requiring userland access
        
            buffer[mod(state - 4,20)] = 0;
            if (state > 3) buffer[mod(state - 3,20)] = 4;
            if (state > 2) buffer[mod(state - 2,20)] = 9;
            if (state > 1) buffer[mod(state - 1,20)] = 15;
            if (state < 20) buffer[mod(state,20)] = 9;
            if (state < 19) buffer[state+1] = 4;
            if (state < 18) buffer[state+2] = 0;

            state += dir;
            if (state == -1 || state == 20) {
                dir = -dir;
            }
        
        //buffer[0] = state;
        while (count < 2) {
			uint16_t shadow_cycle_count = cycle_count;
			last_cycle_count = shadow_cycle_count;
			
            cli();
            while (last_cycle_count == shadow_cycle_count) {
                sleep_enable();
                sei();
                sleep_cpu();
                sleep_disable();
				
                cli();
				shadow_cycle_count = cycle_count;
            }
            sei();
            ++count;
        }
        count = 0;

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
    charlie_init(1,NUM_PINS,ledPins,buffer);
}
