/*
 * lcd.h
 *
 *  Created on: 2014-8-14
 *      Author: Kevio
 */
#include <msp430.h>
#include <intrinsics.h>
#include <math.h>
#include <stdlib.h>
#ifndef LCD_H_
#define LCD_H_

#define LCD_PWR_0 P10OUT&=~BIT3
#define LCD_RST_1  P10OUT|=BIT2
#define LCD_RST_0  P10OUT&=~BIT2
#define LCD_CE_1  P10OUT|=BIT1
#define LCD_CE_0  P10OUT&=~BIT1
#define LCD_DC_1  P10OUT|=BIT0
#define LCD_DC_0  P10OUT&=~BIT0
#define LCD_CIN_1  P3OUT|=BIT7
#define LCD_CIN_0  P3OUT&=~BIT7
#define LCD_CLK_1 P5OUT|=BIT5
#define LCD_CLK_0 P5OUT&=~BIT5


void LCD_Set(unsigned char x,unsigned char y);
void LCD_Char(unsigned char c);
void LCD_Write_Byte(unsigned char data,unsigned char dc);
void LCD_Clear(void);
void delaylcd(unsigned int n);
void delaylcd_us(unsigned int n);
void Init_LCD();
//void Init_CLK(void);
void Init_Port(void);
void Init_SPI(void);
void LCD_Printf(unsigned char x,unsigned char y,char *s);



#endif /* LCD_H_ */
