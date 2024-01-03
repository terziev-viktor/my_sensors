#include <assert.h>
#include "hcsr04.h"
#include "stm32f3xx_hal_gpio.h"
#include "cmsis_os2.h"

typedef struct HCSR04 {
    uint32_t trig_pin;
    uint16_t _elapsed;
    GPIO_TypeDef * GPIOx;
    TIM_HandleTypeDef * htim; // timer to use for delay of <= 11 microseconds
    HCSR04_External_Dependency_t getHumidity;
    HCSR04_External_Dependency_t getTemperature;
    bool _elapsed_last_value_flag;
    bool initialized;
} HCSR04;

static HCSR04 self;

void HCSR04_Init(GPIO_TypeDef  *GPIOx, uint16_t trig_pin, uint16_t echo_pin,  TIM_HandleTypeDef * htim, HCSR04_External_Dependency_t getHumidity, HCSR04_External_Dependency_t getTemperature) {
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

float HCSR04_MeasureDistanceInMeters(void) {
    assert(self.initialized);
    const bool old_flag = self._elapsed_last_value_flag;
    HCSR04_Trigger();
    while (old_flag == self._elapsed_last_value_flag) {
        // wait for the interrupt to update the time elapsed, blocking
        // this takes at most 38 milliseconds
    }
    const float speed_of_sound_ms = (331.4f + (0.606f * self.getTemperature()) + (0.0124f * self.getHumidity()));
    const float dist = (to_seconds(self._elapsed) * speed_of_sound_ms) / 2;
    return dist;
}

static float to_cm(float meters) {
    return meters * 100.0f;
}

bool HCSR04_IsValidDistance(float distance_m) {
    assert(self.initialized);
    return to_cm(distance_m) >= 2.0f && to_cm(distance_m) <= 400.0f;
}

void HCSR04_ElapsedTimeMeasuredCallback(uint16_t time) {
    // assert(self.initialized);
    self._elapsed = time;
    self._elapsed_last_value_flag = !self._elapsed_last_value_flag;
}