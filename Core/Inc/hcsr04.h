#ifndef MY_SENSORS_HCSR04_H
#define MY_SENSORS_HCSR04_H

#include <stdbool.h>
#include <stdint-gcc.h>
#include "stm32f3xx_hal.h"

typedef float (*HCSR04_External_Dependency_t)(void);

typedef enum {
    HCSR04_DONE = 0,
    HCSR04_BEGIN = HCSR04_DONE,
    HCSR04_PRE_TRIGGER = HCSR04_BEGIN,
    HCSR04_TRIGGER,
    HCSR04_POST_TRIGGER,
    HCSR04_WAIT_FOR_ECHO,
    HCSR04_CALCULATE_DISTANCE,
} HCSR04_Execution_State_t;

void HCSR04_Init(GPIO_TypeDef  *GPIOx, uint16_t trig_pin, uint16_t echo_pin,  TIM_HandleTypeDef * htim, HCSR04_External_Dependency_t getHumidity, HCSR04_External_Dependency_t getTemperature);
float HCSR04_MeasureDistanceInMeters(void);
HCSR04_Execution_State_t HCSR04_MeasureDistanceInMetersNonBlocking(float * out_distance_m, HCSR04_Execution_State_t current_state);
bool HCSR04_IsValidDistance(float distance_m);
void HCSR04_ElapsedTimeMeasuredCallback(uint16_t time);

#endif //MY_SENSORS_HCSR04_H
