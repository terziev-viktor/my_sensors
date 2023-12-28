#include "console.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#define BUFFER_SIZE (100)

struct Console
{
    UART_HandleTypeDef * huart;
    bool initialized;
    uint8_t buffer[BUFFER_SIZE];
};

Console console;

void Console_Init(UART_HandleTypeDef *huart)
{
    assert(console.initialized == false);
    console.huart = huart;
    console.initialized = true;
}

void Console_Print(const char *format, ...)
{
    assert(console.initialized == true);
    memset(console.buffer, '\0', sizeof console.buffer);

    va_list args;
    va_start(args, format);
    vsnprintf((char*)console.buffer, BUFFER_SIZE, format, args);

    const size_t len = strlen((char*)console.buffer);
    if(HAL_UART_Transmit(console.huart, (uint8_t *)console.buffer, len, HAL_MAX_DELAY) != HAL_OK)
    {
        va_end(args);
        assert(false);
    }
    va_end(args);
}
