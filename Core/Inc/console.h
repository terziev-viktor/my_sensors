
#ifndef MY_SENSORS_CONSOLE_H
#define MY_SENSORS_CONSOLE_H
#include "stm32f3xx_hal.h"

struct Console;
typedef struct Console Console;

void Console_init(UART_HandleTypeDef * huart);
void Console_print(const char * format, ...);

#endif //MY_SENSORS_CONSOLE_H
