#include "af_motor_shield.h"
#include <stdbool.h>
#include <assert.h>
#include "stm32f3xx_hal_gpio.h"
#include "main.h"

#define BV(bit) (1 << (bit))

struct AFMotorShield {
    uint8_t bitPosA;
    uint8_t bitPosB;
    MOTOR_t num;
    uint32_t freq;
    AFMotorShieldPeripheral peripheral;
    bool initialized;
} AFMotorShield_DCMotor_1, AFMotorShield_DCMotor_2, AFMotorShield_DCMotor_3, AFMotorShield_DCMotor_4;

typedef uint8_t latch_state_t;
typedef struct AFMotorController {
    latch_state_t latch_state;
    bool initialized;
} AFMotorController;

static AFMotorController MC;

void AFMotorController_LatchTx() {
    HAL_GPIO_WritePin(MOTORLATCH_GPIO_Port, MOTORLATCH_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTORDATA_GPIO_Port, MOTORDATA_Pin, GPIO_PIN_RESET);

    for (uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(MOTORCLK_GPIO_Port, MOTORCLK_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTORDATA_GPIO_Port, MOTORDATA_Pin, (MC.latch_state & (1 << (7 - i))) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTORCLK_GPIO_Port, MOTORCLK_Pin, GPIO_PIN_SET);
    }

    HAL_GPIO_WritePin(MOTORLATCH_GPIO_Port, MOTORLATCH_Pin, GPIO_PIN_SET);
}

void AFMotorShield_SetSpeed(AFMotorShield * self, uint8_t speed) {
    assert(false && "Not Implemented");
}

static void AFMotorController_Enable() {
    if (!MC.initialized) {
        MC.latch_state = 0;
        MC.initialized = true;
    }
    assert(MC.initialized);
    MC.latch_state = 0;
    AFMotorController_LatchTx();
    HAL_GPIO_WritePin(MOTORENABLE_GPIO_Port, MOTORENABLE_Pin, GPIO_PIN_RESET);
}

AFMotorShield * AFMotorShield_InitDCMotor(MOTOR_t num, const uint8_t freq, const AFMotorShieldPeripheral peripheral) {
    AFMotorController_Enable();
    switch (num) {
        case MOTOR_1: {
            assert(!AFMotorShield_DCMotor_1.initialized);
            MC.latch_state &= ~BV(MOTOR1_A) & ~BV(MOTOR1_B); // set both motor pins to 0
            AFMotorController_LatchTx();
            AFMotorShield_DCMotor_1.bitPosA = MOTOR1_A;
            AFMotorShield_DCMotor_1.bitPosB = MOTOR1_B;
            AFMotorShield_DCMotor_1.num = num;
            AFMotorShield_DCMotor_1.freq = freq;
            AFMotorShield_DCMotor_1.initialized = true;
            AFMotorShield_DCMotor_1.peripheral = peripheral;
            return &AFMotorShield_DCMotor_1;
        }
        case MOTOR_2: {
            assert(!AFMotorShield_DCMotor_2.initialized);
            MC.latch_state &= ~BV(MOTOR2_A) & ~BV(MOTOR2_B); // set both motor pins to 0
            AFMotorController_LatchTx();
            AFMotorShield_DCMotor_2.bitPosA = MOTOR2_A;
            AFMotorShield_DCMotor_2.bitPosB = MOTOR2_B;
            AFMotorShield_DCMotor_2.num = num;
            AFMotorShield_DCMotor_2.freq = freq;
            AFMotorShield_DCMotor_2.initialized = true;
            AFMotorShield_DCMotor_2.peripheral = peripheral;
            return &AFMotorShield_DCMotor_2;
        }
        case MOTOR_3: {
            assert(!AFMotorShield_DCMotor_3.initialized);
            MC.latch_state &= ~BV(MOTOR3_A) & ~BV(MOTOR3_B); // set both motor pins to 0
            AFMotorController_LatchTx();
            AFMotorShield_DCMotor_3.bitPosA = MOTOR3_A;
            AFMotorShield_DCMotor_3.bitPosB = MOTOR3_B;
            AFMotorShield_DCMotor_3.num = num;
            AFMotorShield_DCMotor_3.freq = freq;
            AFMotorShield_DCMotor_3.initialized = true;
            AFMotorShield_DCMotor_3.peripheral = peripheral;
            return &AFMotorShield_DCMotor_3;
        }
        case MOTOR_4: {
            assert(!AFMotorShield_DCMotor_4.initialized);
            MC.latch_state &= ~BV(MOTOR4_A) & ~BV(MOTOR4_B); // set both motor pins to 0
            AFMotorController_LatchTx();
            AFMotorShield_DCMotor_4.bitPosA = MOTOR4_A;
            AFMotorShield_DCMotor_4.bitPosB = MOTOR4_B;
            AFMotorShield_DCMotor_4.num = num;
            AFMotorShield_DCMotor_4.freq = freq;
            AFMotorShield_DCMotor_4.initialized = true;
            AFMotorShield_DCMotor_4.peripheral = peripheral;
            return &AFMotorShield_DCMotor_4;
        }
    }
}

void AFMotorShield_RunDCMotor(AFMotorShield * self, DCMotorCommand command) {
    switch (command) {
        case FORWARD: {
            MC.latch_state |= BV(self->bitPosA);
            MC.latch_state &= ~BV(self->bitPosB);
            AFMotorController_LatchTx();
            break;
        }
        case BACKWARD: {
            MC.latch_state &= ~BV(self->bitPosA);
            MC.latch_state |= BV(self->bitPosB);
            AFMotorController_LatchTx();
            break;
        }
        case RELEASE: {
            MC.latch_state &= ~BV(self->bitPosA);
            MC.latch_state &= ~BV(self->bitPosB);
            AFMotorController_LatchTx();
            break;
        }
        default:
            // Not Implemented
            assert(false);
    }
}
