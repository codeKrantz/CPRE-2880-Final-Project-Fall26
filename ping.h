/*
 * ping.h
 *
 *  Created on: Oct 28, 2026
 *      Author: bartle
 */

#ifndef PING_H_
#define PING_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

extern volatile uint32_t rising_time;   // Rising edge
extern volatile uint32_t falling_time;  // Falling edge

// 0 = No edges
// 1 = rising edge detected
// 2 = both edges detected
extern volatile int edges;

void ping_init_ours(void);

int ping_read(void);

void TIMER3B_Handler_Ours(void);


#endif /* PING_H_ */
