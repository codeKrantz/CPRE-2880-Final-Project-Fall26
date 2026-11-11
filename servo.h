/*
 * servo.h
 *
 *  Created on: Nov 4, 2026
 *      Author: bartle
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

void servo_init(void);

int servo_move(float degrees);

int get_high_tick(float degrees);


#endif /* SERVO_H_ */
