/**
 * @file utils.cpp
 * @brief Implements hardware abstraction utility functions for cydOS.
 *
 * Provides LED control and other low-level hardware utilities.
 */
#include "utils.h"
#include <Arduino.h>
#include "main.h"


void torchON() { // Let there be light! (And by light I mean blinding LEDs)
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
}

void torchOFF() {
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
}


