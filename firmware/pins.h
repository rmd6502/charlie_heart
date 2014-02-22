enum Directions {
    INPUT, OUTPUT, INPUT_PULLUP
};
void pinMode(uint8_t pin, enum Directions direction);
void digitalWrite(uint8_t pin, uint8_t value);
uint8_t digitalRead(uint8_t pin);
