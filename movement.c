/*
 * movement.c
 *
 *  Created on: Sep 2, 2026
 *      Author: bartle
 */

#include "movement.h"
#include <math.h>
#include "lcd.h"
#include "open_interface.h"
#include "uart_extra_help.h"

volatile char flag; //might need here


//due to the flag we need a way to know if we are driving manual or auto, so I will user a char mode(0 = manual and 1 = auto)

double move_forward(oi_t *sensor_data, double distance, char mode)
{
    double totalDistance = 0;

    //should be a positive value
    if (distance < 0)
    {
        return 0;
    }

    oi_setWheels(MOVE_SPEED, MOVE_SPEED);
    while (totalDistance < distance && (flag == 0 || mode == 0))
    {
        oi_update(sensor_data);
        totalDistance += sensor_data->distance;
    }

    //toggled to not move
    if (flag == 1)
    {
        //toggled
        send_string("Robot told to stop!\r\n");
    }
    oi_setWheels(0, 0); // stop
    //oi_free(sensor_data);

    //A float with 2 decimals to the lcd screen
    //lcd_printf("%.2f", totalDistance);
    return totalDistance;
}

double move_backward(oi_t *sensor_data, double distance)
{
    double totalDistance = 0;

    //should be a positive value
    if (distance < 0)
    {
        return 0;
    }
    oi_setWheels(-MOVE_SPEED, -MOVE_SPEED);
    while (totalDistance < distance)
    {
        oi_update(sensor_data);
        totalDistance += fabs(sensor_data->distance);
    }

    oi_setWheels(0, 0); // stop
    return totalDistance;
}

double move_with_bump_and_cliff(oi_t *sensor_data, double distance, char mode)
{
    double totalDistance = 0;

    oi_setWheels(MOVE_SPEED, MOVE_SPEED);
    while (totalDistance < distance && (flag == 0 || mode == 0))
    {
        oi_update(sensor_data);
        totalDistance += fabs(sensor_data->distance);

        char message[100];
        if (sensor_data->bumpLeft) //If both bumpers are pressed, the robot will turn right
        {
            sprintf(message, "LEFT BUMP DETECTED after %f cm \r\n", totalDistance);
            send_string("LEFT BUMP DETECTED after \r\n");
            oi_setWheels(0, 0);
            totalDistance -= move_backward(sensor_data, 50); // Move backward 15cm
            turn_clockwise(sensor_data, 90, mode); // Turn 35 degrees clockwise
            move_forward(sensor_data, 200, mode); // Moving laterally so totalDistance does NOT need to be updated
            turn_counterclockwise(sensor_data, 90, mode); // Turn 90 degrees counterclockwise
            //turn_counterclockwise(sensor_data, 50, mode); // Turn 45 degrees counterclockwise, should be straight again
            move_forward(sensor_data, 150, mode); //move infront of object
            send_string("Done accounting for bump\r\n");
            break;
        }
        else if (sensor_data->bumpRight)
        {
            sprintf(message, "RIGHT BUMP DETECTED after %f cm \r\n", totalDistance);
            send_string("RIGHT BUMP DETECTED after \r\n");
            oi_setWheels(0, 0);
            totalDistance -= move_backward(sensor_data, 50); // Move backward 15cm
            turn_counterclockwise(sensor_data, 90, mode); // Turn 90 degrees counterclockwise
             // Turn 35 degrees clockwise
            move_forward(sensor_data, 200, mode); // Moving laterally so totalDistance does NOT need to be updated
            turn_clockwise(sensor_data, 90, mode);
            move_forward(sensor_data, 150, mode);
            //turn_clockwise(sensor_data, 50, mode); // Turn 90 degrees clockwise, should be straight again
            send_string("Done accounting for bump\r\n");
            break;
        }

        else if(sensor_data->cliffFrontLeftSignal && sensor_data->cliffFrontRightSignal)
        {
            lcd_printf("HOLE FOUND DON'T MOVE!");
            break;
        }

        oi_setWheels(MOVE_SPEED + 5, MOVE_SPEED); // To prevent the robot from stopping after a bump, the wheels are set to move forward again

    }

    if (flag == 1)
       {
           //toggled
           send_string("Robot told to stop!\r\n");
       }

    oi_setWheels(0, 0); // stop
    //oi_free(sensor_data);

    //A float with 2 decimals to the lcd screen
    //lcd_printf("%.2f", totalDistance);

    return totalDistance;
}

double turn_clockwise(oi_t *sensor_data, double degrees, char mode)
{
    double totalAngle = 0;

    // right wheel back, left wheel forward == spin right in place
    oi_setWheels(-TURN_SPEED, TURN_SPEED);
    while (totalAngle < degrees && (flag == 0 || mode == 0))
    {
        oi_update(sensor_data);
        //totalAngle == sensor_data->angle ; may be need to account for negative
        totalAngle += fabs(sensor_data->angle);
    }

    //toggled to not move
    if (flag == 1)
    {
        //toggled
        send_string("Robot told not to turn!\r\n");
    }

    oi_setWheels(0, 0); // stop
    return totalAngle;
}

double turn_counterclockwise(oi_t *sensor_data, double degrees, char mode)
{
    double totalAngle = 0;
    //slower than moving for accuracy
    oi_setWheels(TURN_SPEED, -TURN_SPEED);
    //left for cybot 2 seems to be too much
    while (totalAngle < degrees - 2 && (flag == 0 || mode == 0))
    {
        oi_update(sensor_data);
        totalAngle += sensor_data->angle;
    }

    //toggled to not move
    if (flag == 1)
    {
        //toggled
        send_string("Robot told not to turn!\r\n");
    }

    oi_setWheels(0, 0); // stop
    //oi_free(sensor_data);

    return totalAngle;

}

/*double move_Cliff()
{

}*/

