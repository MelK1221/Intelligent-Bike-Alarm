// Microsecond counter header file (used for execution time testing only)

#ifndef MICROS_H
#define MICROS_H

#include <stdint.h>

// Call once during setup to initialize the timer and enable the ISR
void micros_init(void);

// Returns the number of microseconds elapsed since micros_init() was called
uint32_t micros_custom(void);

#endif // MICROS_H