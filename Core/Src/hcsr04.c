#include "hcsr04.h"
#include "stm32f3xx_hal_gpio.h"
#include <assert.h>
#include <math.h>

typedef struct HCSR04 {
    uint32_t trig_pin;
    uint16_t _elapsed;
    GPIO_TypeDef *GPIOx;
    TIM_HandleTypeDef *htim; // timer to use for delay of <= 10 microseconds
    HCSR04_External_Dependency_t getHumidity;
    HCSR04_External_Dependency_t getTemperature;
    struct {
        uint32_t then;
        bool old_flag;
    } non_blocking_state_memory;
    bool _elapsed_last_value_flag;
    bool initialized;
} HCSR04;

static HCSR04 self = {.initialized = false};

void HCSR04_Init(GPIO_TypeDef *GPIOx, uint16_t trig_pin, uint16_t echo_pin, TIM_HandleTypeDef *htim,
                 HCSR04_External_Dependency_t getHumidity, HCSR04_External_Dependency_t getTemperature) {
    assert(!self.initialized);
    (void) echo_pin;

    self.trig_pin = trig_pin;
    self.GPIOx = GPIOx;
    self.htim = htim;
    self.getHumidity = getHumidity;
    self.getTemperature = getTemperature;
    self._elapsed_last_value_flag = false;
    self.initialized = true;
}

static float to_seconds(float microseconds) {
    return microseconds / 1000000.0f;
}

static void HCSR04_Trigger() {
    HAL_GPIO_WritePin(self.GPIOx, self.trig_pin, GPIO_PIN_SET);
    const uint32_t then = __HAL_TIM_GET_COUNTER(self.htim);
    while (__HAL_TIM_GET_COUNTER(self.htim) - then < 11) {
        // wait for at least 11 microseconds, blocking
    }
    HAL_GPIO_WritePin(self.GPIOx, self.trig_pin, GPIO_PIN_RESET);
}

static float HCSR04_SpeedOfSound_Ms(void) {
    return (331.3f + (0.606f * floorf(self.getTemperature())) + (0.0124f * floorf(self.getHumidity())));
}

float HCSR04_MeasureDistanceInMeters(void) {
    assert(self.initialized);
    const bool old_flag = self._elapsed_last_value_flag;
    HCSR04_Trigger();
    while (old_flag == self._elapsed_last_value_flag) {
        // wait for the interrupt to update the time elapsed, blocking
        // this takes at most 38 milliseconds
    }
    const float dist = (to_seconds(self._elapsed) * HCSR04_SpeedOfSound_Ms()) / 2;
    return dist;
}

HCSR04_Execution_State_t HCSR04_MeasureDistanceInMetersNonBlocking(float *out_distance_m, HCSR04_Execution_State_t current_state) {
    assert(self.initialized);

    switch (current_state) {
        case HCSR04_PRE_TRIGGER: {
            self.non_blocking_state_memory.then = __HAL_TIM_GET_COUNTER(self.htim);
            self.non_blocking_state_memory.old_flag = self._elapsed_last_value_flag;
            HAL_GPIO_WritePin(self.GPIOx, self.trig_pin, GPIO_PIN_SET);
        }
        case HCSR04_TRIGGER: {
            if (__HAL_TIM_GET_COUNTER(self.htim) - self.non_blocking_state_memory.then < 11) {
                return HCSR04_TRIGGER;
            }
        }
        case HCSR04_POST_TRIGGER:
            HAL_GPIO_WritePin(self.GPIOx, self.trig_pin, GPIO_PIN_RESET);
        case HCSR04_WAIT_FOR_ECHO:
            if (self._elapsed_last_value_flag == self.non_blocking_state_memory.old_flag) {
                return HCSR04_WAIT_FOR_ECHO;
            }
        case HCSR04_CALCULATE_DISTANCE: {
            *out_distance_m = (to_seconds(self._elapsed) * HCSR04_SpeedOfSound_Ms()) / 2;
            return HCSR04_DONE;
        }
        default:
            assert(false);
    }
}

static float to_cm(float meters) {
    return meters * 100.0f;
}

bool HCSR04_IsValidDistance(float distance_m) {
    assert(self.initialized);
    return to_cm(distance_m) >= 2.0f && to_cm(distance_m) <= 400.0f;
}

void HCSR04_ElapsedTimeMeasuredCallback(uint16_t time) {
    self._elapsed = time;
    self._elapsed_last_value_flag = !self._elapsed_last_value_flag;
}

// Overrides the weak HAL_TIM_IC_CaptureCallback function
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    static uint16_t first = 0;
    static uint32_t second = 0;
    static bool first_edge = true;

    if (htim != self.htim) {
        return;
    }

    if (first_edge) {
        first = HAL_TIM_ReadCapturedValue(self.htim, TIM_CHANNEL_1);
    } else {
        second = HAL_TIM_ReadCapturedValue(self.htim, TIM_CHANNEL_1);
        if (second < first) {
            // the timer counter is 16 bit, so it will overflow at UINT16_MAX + 1
            second += UINT16_MAX;
        }
        HCSR04_ElapsedTimeMeasuredCallback(second - first);
    }
    first_edge = !first_edge;
}