/*
 * servo.c
 *
 *  Created on: Nov 4, 2026
 *      Author: bartle
 */

#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include "Timer.h"
#include "servo.h"

#define SERVO_PERIOD 320000 // Timer Ticks based on 20 ms x 16MHz

//int start_degree = 12666.0; // 0 degree cybot 01
//int end_degree  = 27378.0;  // 180 degree cybot 01

//int start_degree = 8000.0; // 0 degree cybot 22
//int end_degree  = 28000.0; // 180 degree cybot 22

int start_degree = 8000;   // Cybot 25
int end_degree   = 27533;  // CyBot 25

void servo_init(void)
{
    // Enable Port B and Timer 1 clocks
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x02;

    while ((SYSCTL_PRGPIO_R & 0x02) == 0);    // may not need
    while ((SYSCTL_PRTIMER_R & 0x02) == 0);   // may not need

    // Configure PB5 for T1CCP1 (function 7)
    GPIO_PORTB_DIR_R |= 0x20;
    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_AMSEL_R &= ~0x20;

    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0x00F00000) | 0x00700000;

    GPIO_PORTB_AFSEL_R |= 0x20;

    // Disable Timer1B before configuring
    TIMER1_CTL_R &= ~0x0100;

    // Split timer mode: 16-bit + 8-bit prescaler
    TIMER1_CFG_R = 0x04;

    // Periodic PWM, count down
    // Bit 10: update match at next timeout
    TIMER1_TBMR_R = 0x040A ;     // CHANGED FROM: TIMER1_TBMR_R = 0x0A; 0x040A

    // Non-inverted PWM output
    TIMER1_CTL_R &= ~0x4000;

    // Disable Timer1B interrupts
    TIMER1_IMR_R &= ~0x0F00 ;    // CHANGED FROM: TIMER1_IMR_R &= ~0x0400; ~0x0F00

    // 20 ms period = 320,000 ticks = 4 E200
    // 15 ms period = 240,000 ticks = 3 A980
    TIMER1_TBPR_R = 0x04;
    TIMER1_TBILR_R = 0xE200;

    // 1 ms HIGH pulse = 16,000 ticks // Trying 8,000
    // 2 ms HIGH pulse = 32,000 ticks
    // Match  = 320,000 - 16,000 = 304,000 = 4 A380
    // Match  = 320,000 -  8,000 = 312,000 = 4 C2C0 // This worked to find PERFECT 0 degrees
    TIMER1_TBPMR_R = 0x04;
    TIMER1_TBMATCHR_R = 0xC2C0;

    // Enable Timer1B
    TIMER1_CTL_R |= 0x0100;
}

int servo_move(float degrees)
{
    uint32_t high_ticks;
    uint32_t match;         // Position Counter Value


    // Keep degrees within 0 - 180
    /*if (degrees < 0)
    {
        degrees = 0;
    }

    if (degrees > 180)
    {
        degrees = 180;
    }*/


    /*
     * Convert degrees into HIGH pulse ticks
     * High Pulse Ticks = Degrees * (Pulses per Revolution / 180)
     *
     * 0 degrees = 1 ms (16,000 ticks) // Trying a lower HIGH pulse tick 8,000. Worked to find perfect 0 but ruined 90 and 180.
     * 180 degrees = 2 ms (32,000 ticks)
     *
     * 8,000  = 0 degrees
     * 28,000 = 180 degrees
     * These two are variables that change with every CyBot
    */
    high_ticks = start_degree + (uint32_t)(degrees * (end_degree / 180.0));


    // Calculate the match value
    match = SERVO_PERIOD - high_ticks;

    // Update 24-bit match value
    TIMER1_TBPMR_R = (match >> 16) & 0xFF;
    TIMER1_TBMATCHR_R = match & 0xFFFF;

    // Allow servo time to move
    timer_waitMillis(50);

    return match;
}

int get_high_tick(float degrees)
{
    uint32_t high_ticks = start_degree + (uint32_t)(degrees * (end_degree / 180.0));;

    return high_ticks;
}


