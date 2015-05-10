#include "debug.h"
#include "param.h"

void debug_init(void) {
    gpio_output(BK_DBG1, PIN_DBG1);
    gpio_output(BK_DBG2, PIN_DBG2);
    gpio_output(BK_DBG3, PIN_DBG3);
    gpio_output(BK_DBG4, PIN_DBG4);
    gpio_output(BK_DBG5, PIN_DBG5);
}