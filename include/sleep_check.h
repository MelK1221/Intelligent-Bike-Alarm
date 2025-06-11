#ifndef SLEEP_CHECK_H
#define SLEEP_CHECK_H

#include <stdbool.h>

extern volatile bool in_sleep_mode;

void check_sleep(void);

#endif // SLEEP_CHECK_H