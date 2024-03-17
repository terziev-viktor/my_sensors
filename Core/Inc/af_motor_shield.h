#include <stdint-gcc.h>
#include "stm32f3xx_hal.h"

struct AFMotorShield;
typedef struct AFMotorShield AFMotorShield;

#define MOTORLATCH 12
#define MOTORCLK 4
#define MOTORENABLE 7
#define MOTORDATA 8

// Bit positions in the 74HCT595 shift register output
#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR4_A 0
#define MOTOR4_B 6
#define MOTOR3_A 5
#define MOTOR3_B 7

typedef enum DCMotorCommand {
    FORWARD = 1,
    BACKWARD = 2,
    BRAKE = 3,
    RELEASE = 4
} DCMotorCommand;

typedef enum MOTOR_t {
    MOTOR_1 = 1,
    MOTOR_2 = 2,
    MOTOR_3 = 3,
    MOTOR_4 = 4
} MOTOR_t;

typedef struct AFMotorShieldPeripheral {
    TIM_HandleTypeDef *htim;
    uint16_t channel;
} AFMotorShieldPeripheral;

AFMotorShield * AFMotorShield_InitDCMotor(MOTOR_t num, uint8_t freq, AFMotorShieldPeripheral peripheral);
void AFMotorShield_SetSpeed(AFMotorShield * self, uint8_t speed);
void AFMotorShield_RunDCMotor(AFMotorShield * self, DCMotorCommand command);
