/*
 * movement.h
 *
 *  Created on: Sep 2, 2026
 *      Author: bartle
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "open_interface.h"
#include "uart_extra_help.h"

#define MOVE_SPEED 100 //Slowed down from lab 3
#define TURN_SPEED 35 //extra slow to be safe


//Public api:

/*
 * @param sensor_data - to collect and use sensor data
 * @param distance - the distance the robot should travel as a double in mm
 * @return totalDistance - the total distance traveled
*/
double move_forward(oi_t *sensor_data, double distance, char mode);

/*
 * @param sensor_data - to collect and use sensor data
 * @param distance - the distance the robot should travel as a double in mm
 * @return totalDistance - the total distance traveled
*/
double move_backward(oi_t *sensor_data, double distance);

/*
 * @param sensor_data - to collect and use sensor data
 * @param degrees - the number of degrees the robot should turn to the LEFT in degrees
 * @return totalAngle - the angle the robot actually turned
*/

/*
* @param sensor - pointer to the initialized oi_t sensor struct
* @param distance - distance to travel, in millimeters
* @return the net forward distance actually covered by the robot
*/
double move_with_bump_and_cliff(oi_t *sensor_data, double distance_cm, char mode);

double turn_counterclockwise(oi_t *sensor_data, double degrees, char mode);

/*
* @param sensor_data - to collect and use sensor data
* @param degrees - the number of degrees the robot should turn to the RIGHT in degrees
* @return totalAngle - the angle the robot actually turned
*/
double turn_clockwise(oi_t *sensor_data, double degrees, char mode) ;

#endif /* MOVEMENT_H_ */
