#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "charlie.h"

volatile unsigned long timer;

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
}
