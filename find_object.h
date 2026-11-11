/*
 * find_object.h
 *
 *  Created on: Oct 14, 2026
 *      Author: bartle
 */

#ifndef FIND_OBJECT_H_
#define FIND_OBJECT_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"
#include "object_struct.h"
#include "timer.h"

/*
* Param :
*/
int find_objects(const float ir_sensor_data[], int ping_sensor_data[],int reading_count, struct Object objects[], int max_objects);


/*
* Param :
*/
int find_smallest_radial_object(const struct Object objects[], int object_count);


/*
* Param :
*/
int find_smallest_linear_object(const struct Object objects[], int object_count);

#endif /* FIND_OBJECT_H_ */
