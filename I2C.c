/*
 * I2C.c
 *
 *  Created on: 2014-8-13
 *      Author: Kevio
 */

#include "I2C.h"

uchar I2CSendBuffer[2],I2CRecvBuffer;
int I2CSendPtr=0;
void I2CWriteInit();
void I2CReadInit();
void I2C_Init (uchar SlaveAddr)
{
	P3SEL |= BIT1+BIT2;
	UCB0CTL1 |= UCSWRST;                      // Enable SW reset
	UCB0CTL0 = UCMST+UCMODE_3 + UCSYNC;             // I2C Mater, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
	UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
	UCB0BR1 = 0;
	UCB0I2COA = 0x01A5;							//master addr
	UCB0I2CSA = SlaveAddr;                         // Slave Address is 048h
	UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
}
void I2CWriteInit()
{
	UCB0CTL1 |= UCTR ;               // UCTR=1 => Transmit Mode (R/W bit = 0)
	UCB0IFG &=~(UCTXIFG+UCRXIFG);
	UCB0IE &= ~UCRXIE;
    UCB0IE |= UCTXIE ;                // enable Transmit ready interrupt
}
void I2CReadInit()
{
	UCB0CTL1 &= ~UCTR;              // UCTR=0 => Receive Mode (R/W bit = 1)
    UCB0IFG &=~(UCTXIFG+UCRXIFG);
    UCB0IE &= ~UCTXIE;               // disable Transmit ready interrupt
    UCB0IE |= UCRXIE ;                // enable Receive ready interrupt
}
void I2C_Write(uchar address,uchar data)
{
	I2CSendPtr = 1;
	I2CSendBuffer[1] = address;
	I2CSendBuffer[0] = data;

	while (UCB0STAT & UCBUSY);          // wait until I2C module has finished all operations.
    I2CWriteInit();
    _disable_interrupt();
    UCB0CTL1 |= UCTXSTT;
	__bis_SR_register(CPUOFF + GIE);    // Waiting for Sending.
    UCB0CTL1 |= UCTXSTP;                // I2C stop condition
    while (UCB0CTL1 & UCTXSTP);         // Ensure stop condition got sent
}
uchar I2C_Read(uchar address)
{
	while (UCB0STAT & UCBUSY);          // wait until I2C module has finished all operations
	//Change to Write Mode.
	I2CSendBuffer[0] = address;
	I2CSendPtr = 0;
    I2CWriteInit();
    _disable_interrupt();
    UCB0CTL1 |= UCTXSTT;                 // start condition generation
                                         // => I2C communication is started
    __bis_SR_register(CPUOFF + GIE);     // Enter LPM0 w/ interrupts

	//Change to Read Mode.
    I2CReadInit();
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    while (UCB0CTL1 & UCTXSTT);             // Start condition sent?
    UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
    __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0 w/ interrupts
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    return I2CRecvBuffer;
}

/*----------------------------------------------------------------------------*/
// Description:
//   Acknowledge Polling. The EEPROM will not acknowledge if a write cycle is
//   in progress. It can be used to determine when a write cycle is completed.
/*----------------------------------------------------------------------------*/
void I2C_AckPolling(void)
{
    while (UCB0STAT & UCBUSY) {             // wait until I2C module has
    }                                       // finished all operations

    do {
        UCB0STAT = 0x00;                    // clear I2C interrupt flags
        UCB0CTL1 |= UCTR;                   // I2CTRX=1 => Transmit Mode (R/W bit = 0)
        UCB0CTL1 &= ~UCTXSTT;
        UCB0CTL1 |= UCTXSTT;                // start condition is generated
        while (UCB0CTL1 & UCTXSTT) {        // wait till I2CSTT bit was cleared
            if (!(UCNACKIFG & UCB0STAT)) {  // Break out if ACK received
                break;
            }
        }
        UCB0CTL1 |= UCTXSTP;                // stop condition is generated after
                                            // slave address was sent => I2C communication is started
        while (UCB0CTL1 & UCTXSTP) {        // wait till stop bit is reset
        }
        __delay_cycles(500);                // Software delay
    } while (UCNACKIFG & UCB0STAT);
}


// USCI_B0 Data ISR
// Notice : UCSIAB0RX_ISR should be handle with UCSIAB0TX_ISR
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
	if (UCB0IFG&UCTXIFG)                           // TX
	{
		UCB0TXBUF = I2CSendBuffer[I2CSendPtr];      // Load TX buffer
		I2CSendPtr--;                               // Decrement TX byte counter
		if (I2CSendPtr < 0) {
			while (!(UCB0IFG&UCTXIFG));            // wait for tx complete
			UCB0IE &= ~UCTXIE;;                       // disable interrupts.
			UCB0IFG &= ~UCTXIFG;                     // Clear USCI_B0 TX int flag
			__bic_SR_register_on_exit(LPM0_bits);   // Exit LPM0
		}
	}
	else if (UCB0IFG&UCRXIFG)                      // RX
	{
		I2CRecvBuffer = UCB0RXBUF;                  // store received data in buffer
		__bic_SR_register_on_exit(LPM0_bits);       // Exit LPM0
	}
}







