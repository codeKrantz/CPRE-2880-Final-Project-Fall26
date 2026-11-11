/*
 * find_object.c
 *
 *  Created on: Oct 14, 2026
 *      Author: bartle
 */


#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

#include "timer.h"
#include "uart_extra_help.h"
#include "open_interface.h"
#include "object_struct.h"

#define PI 3.14159265358979323846
#define MAX 5
#define IR_OBJECT_THRESHOLD 0.70f   //made really small because of lab space set up, LOTS OF NOISE!!!! SMALLER FOR IR
#define DEGREES_PER_READING 2
#define NUM_READINGS 91         // 0 to 180 degrees in 2-degree steps, inclusive


int ping_distances[NUM_READINGS]; // cleaned/thresholded readings
float IR_distances[NUM_READINGS];   //IR sensor data

int find_objects(const float ir_sensor_data[], int ping_sensor_data[], int reading_count, struct Object objects[], int max_objects)
{
    int object_count = 0;
    int i = 0;


    //reading_count is used to prevent going over 180 because the array is only 91 elements long, and each element represents 2 degrees
    while (i < reading_count && object_count < max_objects) //ensuring we fit the global array
    {
        // A reading at or above the threshold is the wall/nothing.
        if (ir_sensor_data[i] >= IR_OBJECT_THRESHOLD)
        {
            i++;
            continue;
        }

        struct Object *current_object = &objects[object_count]; //NEEDS TO BE A POINTER SO WE CAN MODIFY THE OBJECT IN THE ARRAY, why did this take so long to fix :(
        current_object->start = i * DEGREES_PER_READING;        //changed to a variable instead of a 2, "no magic numbers"
        current_object->distance = ping_sensor_data[i];//ping data

        // Consecutive readings below the threshold belong to one object.
        //need a way to differentiate objects even if they bleed into field of view
        while (i < reading_count && ir_sensor_data[i] < IR_OBJECT_THRESHOLD)
        {
            // Store the closest reading as the object's distance.
            if( ir_sensor_data[i] >= IR_OBJECT_THRESHOLD || (fabs(ir_sensor_data[i-1] - ir_sensor_data[i])) >= 10)
            {
                break;
            }

            // Update to find the closest distance
            if (ping_sensor_data[i] < current_object->distance)
            {
                current_object->distance = ping_sensor_data[i]; //use -> instead of . because current_object is a pointer to the object in the array, not the object itself!
            }

            i++;
        }

        // Finds end of object
        current_object->end = (i-1) * DEGREES_PER_READING; // i-1 because the last reading was above the threshold, so we need to go back one reading to get the end of the object
        current_object->angle = (current_object->start + current_object->end) / 2; // average of the start and end angles to get the center angle
        current_object->radial_width = current_object->end - current_object->start; //as described in the lab maunal

        double width_radians = current_object->radial_width * PI / 180.0;
        double distance = current_object->distance;

        //2(d)tan(theta / 2)
        current_object->linear_width = (float) (2 * (distance) * tan( (width_radians) * 0.5 ));//switch to radians instead of distance

           //changed from > 10 to allow IR to be more accurate
        if(current_object->radial_width >= 7 )
        {
            //trying to get rid of some of the noise
            object_count++;
        }
        //if it doesn't increment then we should have the ability to remake the object because the array will still be looking at the index we are skippind due to noise

    }

    return object_count;
}



/*
 * Index of the smallest radial-width object, or -1 if there are none.
 */
int find_smallest_radial_object(const struct Object objects[], int object_count)
{
    if (object_count <= 0)
    {
        return -1;
    }
    int smallest = 0;
    int i;
    for (i = 1; i < object_count; i++)
    {
        if (objects[i].radial_width < objects[smallest].radial_width)
        {
            smallest = i;
        }
    }
    return smallest;
}

/*
 * Index of the smallest linear-width object, or -1 if there are none.
 */
int find_smallest_linear_object(const struct Object objects[], int object_count)
{
    if (object_count <= 0)
    {
        return -1;
    }
    int smallest = 0;
    int i;
    for (i = 1; i < object_count; i++)
    {
        if (objects[i].linear_width < objects[smallest].linear_width)
        {
            smallest = i;
        }
    }
    return smallest;
}
