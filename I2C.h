/*
 * I2C.h
 *
 *  Created on: 2014-8-13
 *      Author: Kevio
 */

#ifndef I2C_H_
#define I2C_H_

#include <msp430.h>

typedef unsigned char uchar;
void I2C_Init(uchar);
void I2C_Write(uchar,uchar);
uchar I2C_Read(uchar);


#endif /* I2C_H_ */
