#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>
#include "motor.h"

#define PIN 18
#define PWM_RANGE 200

#define MIN_PULSE_WIDTH 5
#define MAX_PULSE_WIDTH 25


int moveMotor(int degree)
{
    float dutyCycle;        // the unit is %
    int pulseWidth;

    if (degree < 0 || degree > 180)
        return -1;

    pulseWidth = MIN_PULSE_WIDTH + (degree * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) / 180);
    dutyCycle = 5 + (float) degree / 36;

    wiringPiSetupGpio();
    pinMode(PIN, OUTPUT);
    softPwmCreate(PIN, 0, PWM_RANGE);
    softPwmWrite(PIN, pulseWidth);

    printf("pulse width %d\n", pulseWidth);
    printf("duty cycle: %f\n", dutyCycle);
    
    delay(1500);

    return 0;
}
