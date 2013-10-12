#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <string.h>
#include <util/delay.h>

#include "charlie.h"

#define NUM_PINS 20
#define PULSE_COUNTS_PER_STATE 8
#define SCANNER_COUNTS_PER_STATE 8
#define ALT_COUNTS_PER_STATE 25
#define BUTTON_PIN 2
int16_t countsPerState[] = { 0, SCANNER_COUNTS_PER_STATE, PULSE_COUNTS_PER_STATE, ALT_COUNTS_PER_STATE };

typedef enum _States {
    POWEROFF = 0, SCANNER, PULSE, ALTERNATE, NUM_STATES
} States;

volatile uint8_t buffer[NUM_PINS] = {0};

void initialize(void);

int8_t mod(int8_t q, int8_t d) {
    int8_t r = q % d;
    if (r < 0) r += d;
    return r;
}

int main() {
    uint16_t last_cycle_count = 0;
    int8_t state = 0;
    int8_t dir = 1;
    int16_t count = 0;
    int16_t counts_per_state = SCANNER_COUNTS_PER_STATE;
    uint8_t last_button=0;
    States runState = SCANNER;
    initialize();
    while(1) {
        switch(runState) {
            case SCANNER:
		// state goes from 0 to 19
		if (state > 2) buffer[mod(state - 4,20)] = 0;
               	if (state > 1) buffer[mod(state - 3,20)] = 1;
               	if (state > 0) buffer[mod(state - 2,20)] = 4;
		buffer[mod(state - 1,20)] = 15;
		if (state < 20) buffer[mod(state,20)] = 4;
                if (state < 19) buffer[state+1] = 1;
                if (state < 18) buffer[state+2] = 0;
	
                state += dir;
		if (state == 0 || state == 20) {
                    dir = -dir;
                }
                break;
            case PULSE:
                // state goes from 0 to 29
                // 0-4 up, 5-9 down, 10-14 up, 15-19 down, 20-29 idle
                if (state < 5 || (state > 9 && state < 15)) {
                    memset((void *)buffer, (1 + (state % 5)) * 3, sizeof(buffer));
                } else if (state < 10 || (state > 14 && state < 20)) {
                    memset((void *)buffer, (5 - (state % 5)) * 3, sizeof(buffer));
                } else {
                    memset((void *)buffer, 0, sizeof(buffer));
                }
                ++state;
                if (state == 30) {
                    state = 0;
                }
                break;
            case ALTERNATE: {
                for (int ledNum = 0; ledNum < NUM_PINS-1; ledNum += 2) {
                    buffer[ledNum] = state;
                    buffer[ledNum+1] = 15 - state;
                }
                state = 15 - state;
            }
            break;
            case POWEROFF:
            default:
                memset((void *)buffer, 0, sizeof(buffer));
                break;
        }

        count = 0;
        while (count < counts_per_state || counts_per_state == 0) {
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

                if (button_state != last_button) {
                    last_button = button_state;
                    if (!last_button) {
                        state = 0;
                        dir = 1;
                        runState++;
                        if (runState == NUM_STATES) {
                            runState = POWEROFF;
                        }
                        counts_per_state = countsPerState[runState];
                        if (runState == POWEROFF) {
                            prepare_for_sleep();
                            memset((void *)buffer, 0, sizeof(buffer));
                            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                            last_button = button_state;
                        } else {
                            return_from_sleep();
                            set_sleep_mode(SLEEP_MODE_IDLE);
                        }
                    }
                    _delay_ms(1);
                }
            }
            sei();
            ++count;
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
}
