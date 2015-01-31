/*
 * File:   Main.c
 * Author: applet
 *
 * Created on January 26, 2015, 3:23 PM
 */

#include <stdio.h>
#include <stdlib.h>

/*
 *
 */


// PIC18F25K22 Configuration Bit Settings

// 'C' source line config statements


// PIC18F25K22 Configuration Bit Settings

// 'C' source line config statements

#include <p18F25K22.h>
#include <usart.h>
#include <spi.h>
#include <pwm.h>
#include <timers.h>

#include "squireCommander.h"

//LOCAL NODE SPI SETUP
#define SPI_SCK LATCbits.LATC3//clock output, C7
#define SPI_SDO  LATCbits.LATC5 //Serial Data Output, C6
#define SPI_SDI PORTCbits.RC4 //Serial Data Input, C5
#define SPI_COMMANDER LATAbits.LATA5 //chip select, RA5

//GLOBAL THINGS
#define VOLT 0xAA //hey remote node, you OK bro?
#define AFFR 0x0C //remote node: "yeah man"
#define QUER 0xC0

unsigned char tomStar;
void main(void) {
    int turboPhail;
    turboPhail = 0;
    //initialize stuff we did
    Setup();
    //test();
    //test(); //IO test code

    //the primary functionality of local node: SPI COMMAND HQ
    turboPhail = SPI_SQUIRE_COMMANDER();
    //^^returns int if we exit for some reason (reason to be given by int value)
}


void Setup(){
   // OpenSPI1(SLV_SSOFF, MODE_01, SMPEND);
    char period;
    period = 0x80;
   //OpenTimer2(0b10110111);
//   OpenEPWM1(period);
   OpenSPI1(SLV_SSOFF, MODE_00, SMPMID);
   TRISBbits.TRISB0 = 0;
   TRISBbits.TRISB1 = 0;

   TRISCbits.TRISC2 = 1;

   //PWM SETUP STUFF
   //CCP1CON = 0; //clear CCPxCON to relinquish control of pin
   CCPTMRS0 = 0x00;
   PR2 = 0xFF;
   CCP1CON = 0b00001100;
   CCPR1L = 0x40;
   T2CON = 0b00000111;
   TRISCbits.TRISC2 = 0;
   
   /*CCPTMRS1 = 0;
   CCP1CON = CCP1CON | 0x0C;
   PR2 = 0x3F;
   //configure ccp module thingy
   CCP2CON = CCP2CON | 0x0C;
   //load ccprx register with pwm duty cycle
   CCPR2L = 0x80;
   //cleared TMR2 interrupt flag from register PIR1
   PIR1 = PIR1 & 0b11111101;
   //clear
   T2CON = T2CON & 0x00; //prescale value
   //enable timer by setting
   T2CON = T2CON | 0x03; //TIMR2 enabled (bit 3 = 1)
   //enable PWM interrupt bit
   TRISCbits.TRISC2 = 0;8*/
   

   // STEP 2: select slave node
    ANSELC = 0;
  
    
    //ANSELB = 0;
   //SPI_COMMANDER=1; //chip select OFF
}
int SPI_SQUIRE_COMMANDER(void){

    unsigned char inByteH;
    unsigned char inByteL;
    unsigned char outByte;
    unsigned char outByte2;
    unsigned char package[3];
    int counter;
    int counterPrevious;
    int phailDetectorUnit;
    phailDetectorUnit = 0;
    PORTBbits.RB1 = 0;
    counterPrevious = 0;
    /* PHASE 2 : do normal squire SPI things */
    while(1){

        getsSPI1(&package,3);
        CCPR1L = package[0];
        CCP1CON |= (package[1]);
        WriteSPI1(0x00);
    }


return phailDetectorUnit;
}
