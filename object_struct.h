/*
 * object_struct.h
 *
 *  Created on: Oct 14, 2026
 *      Author: bartle
 */

#ifndef OBJECT_STRUCT_H_
#define OBJECT_STRUCT_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

#define MAX 5


struct Object
{
    int distance;        // closest reading to the object (used for display/driving)
    float linear_width;    // third side of the isosceles triangle (cm)
    int start;             // angle of the first reading
    int end;               // angle of the last reading
    int angle;             // center angle where the servo should point
    int radial_width;      // end - start, the width in degrees (as in the lab manual)
};

struct Object objectList[MAX];

#endif /* OBJECT_STRUCT_H_ */
