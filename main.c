

/**
 *
 * Final Project CPRE288: Group PG-E3
 * Authors: Nathan Bartle, Conrad Krantz, Anay Shah, Connor Weiss
 * Project Title: CyDog - A Guide Cybot for Individuals with Impaired Vision
 *
 * Goal: Successfully guide an individual around obstacles on a sidewalk.
 * Needs: We plan to use Sensors (PING, ADC, Bumpers, Cliff/Holes), Movement (Wheels, Servo), and Display (LCD, PuTTY, Python GUI).
 *
 */


/*
 * main.c
 */

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"
#include "timer.h"
#include "lcd.h"
#include "ping.h"
#include "uart_extra_help.h"
#include "adc.h"
#include "movement.h"
#include "open_interface.h"
#include "servo.h"
#include "find_object.h"
#include "button.h"
#include "object_struct.h"

// Timer Variables - Servo
volatile uint32_t rising_time;      // Rising edge
volatile uint32_t falling_time;     // Falling edge
volatile int edges;

// Constant Variables
#define SD_OBJECT_THRESHOLD 70      //made really small because of lab space set up, LOTS OF NOISE!!!! SMALLER FOR IR
#define IR_OBJECT_THRESHOLD 0.70f
#define DEGREES_PER_READING 2
#define NUM_READINGS        91      // 0 to 180 degrees in 2-degree steps, inclusive
#define STEP_DISTANCE_CM    40      // 40 mm = 4 cm
#define STEP_ANGLE_DEG      10

// Adding global volatile variables for communicating between your Interrupt Service Routine, and your non-interupt code.
volatile char uart_data;            // Your UART interrupt code can place read data here
volatile char flag;                 // Your UART interrupt can update this flag





int ping_distances[NUM_READINGS]; // cleaned/thresholded readings
float IR_distances[NUM_READINGS];   // IR sensor data

//mode is used to narrow the search after we know we are close to the object(0 = full field and 1 = 45-135)
void do_scan(char mode)
{
    int left_blinder;
    int right_blinder;

    if (mode == 0)
    {
        left_blinder = 0;
        right_blinder = 180;
    }
    else
    {
        left_blinder = 45;
        right_blinder = 135;
    }

    //float sensor_data_array_e1[91]
    int i;
    for (i = 0; i <= 180; i += DEGREES_PER_READING)
    {
        int idx = i / DEGREES_PER_READING;
        servo_move(i);

        // Best-Fit Function from MatLab
        float coeff1 = 0.000000180080284;
        float coeff2 = -0.000781109266614;
        float coeff3 = 0.957040175015444;
        int base = adc_read();

        ping_distances[idx] = ping_read();

        IR_distances[idx] = (coeff1 * powf(base, 2)) + (coeff2 * base) + coeff3;

        // Using IR sensor to scan, to use for detecting objects.

        // Deleted * 100 from both conditions. Now in meters
        if (IR_distances[idx] > IR_OBJECT_THRESHOLD || i < left_blinder
                || i > right_blinder) //added Blinder to ignore edges of a larger object not infront of the cybot
        {
            IR_distances[idx] = IR_OBJECT_THRESHOLD;
        }
        //no need for else because it should already be stored correctly

        // Anything past the threshold, or at the extreme edges, is "no object"

        //remember its in cm
        if (ping_distances[idx] > (SD_OBJECT_THRESHOLD)) // walls create noise (|| i <= 8 || i > 167 - blinders)
        {
            ping_distances[idx] = SD_OBJECT_THRESHOLD;
        }
        //lcd_printf(ping_distances[idx]);

    }
}

void print_scan_data(void)
{
    int i;
    char line[50];

    for (i = 0; i < NUM_READINGS; i++)
    {
        sprintf(line, "%-9d%-10d%.2f\n", i * DEGREES_PER_READING,
                ping_distances[i], (IR_distances[i] * 100)); // add \r for anything other than python
        send_string(line);
    }
}

void print_objects(int object_count)
{
    char line[100];
    int j;

    sprintf(line, "\r\nObjects found: %d\r\n", object_count);
    send_string(line);
    send_string("Object#  Angle  Dist(cm)  Radial(deg)   Width(cm)\r\n");
    for (j = 0; j < object_count; j++)
    {
        sprintf(line, "%-9d%-7d%-10d%-13d%.3f\r\n", j + 1, objectList[j].angle,
                objectList[j].distance, objectList[j].radial_width,
                objectList[j].linear_width);
        send_string(line);
    }
}

int main(void)
{
	// Initializations
    uart_init(115200);              // Baud Rate = 115200
    uart_interrupt_init();
    IntMasterEnable();
    adc_init();
    lcd_init();
    ping_init_ours();
    servo_init();
    timer_init();
    button_init();

    // Initializing sensor for movement
    oi_t *cy_bot = oi_alloc();
    oi_init(cy_bot);

    uint8_t button;

    while (1)
    {
       button = button_getButton();
        if (button == 1)
        {

            move_with_bump_and_cliff(cy_bot, 700, 0);
        }
    }











    return 0;
}
