/*
 * ping.c
 *
 *  Created on: Oct 28, 2026
 *      Author: bartle
 */

#include "ping.h"
#include "timer.h"
#include "lcd.h"

void ping_init_ours(void)
{
    SYSCTL_RCGCGPIO_R |= 0x02;      // Port B Enable
    SYSCTL_RCGCTIMER_R |= 0x08;     // Module 3 in Run Mode Enable

    GPIO_PORTB_DEN_R |= 0x08;
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0xF000) | 0x7000;
    GPIO_PORTB_AFSEL_R &= ~0x08;
    GPIO_PORTB_DIR_R |= 0x08;
    GPIO_PORTB_DATA_R &= ~0x08;

    TIMER3_CFG_R = 0x04;
    TIMER3_TBMR_R = 0x07;
    TIMER3_CTL_R &= ~0x100;
    TIMER3_TBMR_R = 0x07;
    TIMER3_TBILR_R = 0XFFFF;
    TIMER3_TBPR_R = 0xFF;
    TIMER3_CTL_R |= 0x0C00;
    TIMER3_ICR_R = 0x400;
    TIMER3_IMR_R |= 0x400;

    IntRegister(INT_TIMER3B, TIMER3B_Handler_Ours);
    IntEnable(INT_TIMER3B);
    IntMasterEnable();

    TIMER3_CTL_R |= 0x100;

    rising_time = 0;
    falling_time = 0;
    edges = 0;
}

int ping_read(void)
{
    TIMER3_IMR_R &= ~0x400;     // Disable Timer

    GPIO_PORTB_AFSEL_R &= ~0x08;  // Allows PB3 to operate as a normal GPIO

    GPIO_PORTB_DIR_R |= 0x08;   // Configure PB# as an output to send the trigger

    edges = 0; //RESET FOR THE SECOND READ!!!

    GPIO_PORTB_DATA_R &= ~0x08; // Set PB3 to LOW
    GPIO_PORTB_DATA_R  |= 0x08; // Set PB3 to HIGH to trigger PING sensor

    timer_waitMicros(5);        // Datasheet and Lecture says to wait 5 micro seconds

    GPIO_PORTB_DATA_R &= ~0x08; // Set PB3 to LOW to finish the trigger pulse

    GPIO_PORTB_DIR_R &= ~0x08;  // Configure PB# as an input to send the trigger

    GPIO_PORTB_AFSEL_R |= 0x08;   // Allows PB3 to connect to TIMER3B's Capture input

    TIMER3_ICR_R = 0x400;       // Clear pending interrupts
    TIMER3_IMR_R |= 0x400;      // Re-enable TIMER3B capture interrupts

    while (edges < 2);          // Ensuring out edges are captured


    // Calculates the number of clock cycles.
    // Timer counts DOWN
    // Mask to 24 bits to account for timer wrap-around
    uint32_t cycles = (rising_time - falling_time) & 0xFFFFFF;

    // Convert clock cycles to distance (cm)
    // Assumes 16 MHz lock and 340 m/s speed of sound
    // Divide by 2 for sound traveling to object and back
    int value = (cycles * 17u / 16000u);

    return value;
}

void TIMER3B_Handler_Ours(void)
{
    // Read he captured 24-bit timer value
    // Timer counts when edge is detected
    uint32_t time = TIMER3_TBR_R & 0xFFFFFF;

    // Clear any interrupt flags
    TIMER3_ICR_R = 0x400;

    if (edges == 0)
    {
        rising_time = time;     // Marks the start of echo pulse

        edges = 1;              // First edge caught
    }
    else if (edges == 1)
    {
        falling_time = time;    // Marks the end of echo pulse

        edges = 2;              // Both edges have been captured

        TIMER3_IMR_R &= ~0x400; // Disable capture interrupts until next reading
    }
}
