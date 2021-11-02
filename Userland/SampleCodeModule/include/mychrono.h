#ifndef __CHRONOMETER_H
#define __CHRONOMETER_H

#include <userstdlib.h>
#include <stdint.h>

void restart(char * chrono);
uint8_t modify_chrono(char * chrono, uint8_t ms_ticks);

#endif /* __CHRONOMETER_H */