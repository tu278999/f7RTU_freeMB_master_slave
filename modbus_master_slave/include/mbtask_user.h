/*
 * mbtask_user.h
 *
 *  Created on: Oct 15, 2021
 *      Author: tu.lb174310
 */

#ifndef MODBUS_MBTASK_USER_H_
#define MODBUS_MBTASK_USER_H_



void vInitMBTask(void);

void mastermonitor_task(void*p);

void masterpoll_task(void*p);

void slavepoll_task(void*p);

void led_task(void*p);







#endif /* MODBUS_MBTASK_USER_H_ */
