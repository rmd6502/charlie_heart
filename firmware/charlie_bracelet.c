#include <avr/io.h>
#include <avr/sleep.h>

#include "charlie.h"

void initialize(void);

void main() {
    initialize();
    while(1) {
        cli();
        // test condition requiring userland access
        // if (need_to_do_something) do_something();
        sleep_enable();
        sei();
        sleep_cpu();
        sleep_disable();
    }
}

void initialize(void) {
    // initialize charlieplexing
    LedPins ledPins[20];
    charlie_init(1,20,ledPins);
}
