// Copyright (c) 2012 Robert M. Diamond All Rights Reserved.
//

static const unsigned int SHADES = 16;

typedef struct _LedPins {
    uint8_t highpin;
    uint8_t lowpin;
} LedPins;

void charlie_init(uint8_t _num_rows, uint8_t _num_columns, LedPins *led_pins, volatile uint8_t *_buffer,uint8_t _button_pin);
void prepare_for_sleep();
void return_from_sleep();

extern volatile uint16_t cycle_count;
extern volatile uint8_t button_state;
