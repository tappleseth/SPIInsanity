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
    ANSELC = 0;
    ANSELA = 0;
    ANSELB = 0;
   OpenSPI1(SLV_SSOFF, MODE_00, SMPMID);

   TRISCbits.TRISC2 = 1;

   /* SRAM setup stuff */

   //Control signals
   TRISAbits.TRISA0 = 0; //!CS
   TRISAbits.TRISA1 = 0; //!OE
   TRISAbits.TRISA2 = 0; //!WE

   PORTAbits.RA0 = 1; //!CS disabled
   PORTAbits.RA1 = 1; //!OE disabled
   PORTAbits.RA2 = 1; //!WE disabled

   //address bits
   TRISAbits.TRISA3 = 0; //addr0
   TRISAbits.TRISA4 = 0; //addr1
   //TRISAbits.TRISA7 = 0; //addr1
   //TRISAbits.TRISA6 = 0; //addr1

   PORTAbits.RA3 = 0;
   PORTAbits.RA4 = 0;
  // PORTAbits.RA7 = 0;
   //PORTAbits.RA6 = 0;

   //IO bits
   //all are initially set to be outputs
   TRISB = 0xFF;
   //all are initially low
   PORTBbits.RB0 = 0;
   PORTBbits.RB0 = 0;
   PORTBbits.RB0 = 0;
   PORTBbits.RB0 = 0;
   PORTBbits.RB0 = 0;
   PORTBbits.RB0 = 0;
   PORTBbits.RB0 = 0;

   //PWM SETUP STUFF
   //CCP1CON = 0; //clear CCPxCON to relinquish control of pin
   CCPTMRS0 = 0x00;
   PR2 = 0xC7;
   CCP1CON = 0b00001100;

   CCPR1L = 0x40; //initial duty cycle
   T2CON = 0b00000100; //<-timer (first 2 bits) (5,4)
   //T1CON = 0b00000111;

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



    //ANSELB = 0;
   //SPI_COMMANDER=1; //chip select OFF
}
int SPI_SQUIRE_COMMANDER(void){
     unsigned char holder;
    unsigned char package[2];
    unsigned char loBytes;
    unsigned char hiBytes;
    int i;
    unsigned char oldLSB;
    oldLSB = 0b00001100;


    //int phailDetectorUnit;
    //phailDetectorUnit = 0;wwwwwwwwwwwwww
    /* TOM INDUSTRIES PRESENTS: HOLY GOD DAMN 4 LINES?! */
    while(1){

        getsSPI1(&package,2);
        writeSRAM(package[0],3); //save MSBs
        writeSRAM(package[1],1); //save LSBs



        //writeSRAM(package[1],2); //save old LSBs

        hiBytes = readSRAM(3); //retrieve MSBs
        loBytes = readSRAM(1); //retrieve LSBs
        
        
        CCP1CON = oldLSB | loBytes;
        CCPR1L = hiBytes;
        //CCPR1L = package[0];
        //CCP1CON = CCP1CON | package[1];
       // oldLSB = CCP1CON;
        WriteSPI1(0x00);
    }


return 0;//phailDetectorUnit;
}
void writeSRAM(unsigned char data, int address){
/*TRISAbits.RA0 = 0 //!CS
   TRISAbits.RA1 = 0; //!OE
   TRISAbits.RA2 = 0; //!WE
   TRISAbits.RA3 = 0; //addr0
   TRISAbits.RA4 = 0; //addr1
 */
    unsigned char addr0;
    unsigned char addr1;
    unsigned char addr2;
    unsigned char addr3;
    int delay;

    addr0 = (address & 0x01);
    addr1 = (address >> 1) & 0x01;
    addr2 = (address >> 2) & 0x01;
    addr3 = (address >> 3) & 0x01;

    PORTAbits.RA0 = 0; //SRAM enabled
    PORTAbits.RA1 = 1; //not outputing
    PORTAbits.RA2 = 1; //writing TO the SRAM

    //PORTAbits.RA3 = addr0;
    //PORTAbits.RA4 = addr1;

    //IO pins set to be outputs
    TRISB = 0x00;

    PORTAbits.RA3 = addr0;
    PORTAbits.RA4 = addr1;
   // PORTAbits.RA7 = addr0;
    //PORTAbits.RA6 = addr1;
    LATB = data;

    PORTAbits.RA2 = 0; //writing TO the SRAM

    PORTAbits.RA2 = 1; //not writing TO the SRAM
    PORTAbits.RA0 = 1; //SRAM disabled

    //IO pins reset to be inputs
    TRISB = 0xff;
}

unsigned char readSRAM(int address){
    /*TRISAbits.RA0 = 0 //!CS
   TRISAbits.RA1 = 0; //!OE
   TRISAbits.RA2 = 0; //!WE
   TRISAbits.RA3 = 0; //addr0
   TRISAbits.RA4 = 0; //addr1
 */
    unsigned char addr0;
    unsigned char addr1;
    unsigned char addr2;
    unsigned char addr3;
    unsigned char toSender;
    int delay;
    addr0 = (address & 0x01);
    addr1 = (address >> 1) & 0x01;
    addr2 = (address >> 2) & 0x01;
    addr3 = (address >> 3) & 0x01;
    //IO pins set to inputs
    TRISB = 0xFF;

    PORTAbits.RA0 = 0; //SRAMw enabled
    PORTAbits.RA1 = 1; //not outputing
    PORTAbits.RA2 = 1; //writing TO the SRAM

    //LATA = address;
    PORTAbits.RA3 = addr0;
    PORTAbits.RA4 = addr1;
    //PORTAbits.RA7 = addr0;
    //PORTAbits.RA6 = addr1;

    PORTAbits.RA1 = 0; //output is a GO!

    //give some time to read
    toSender = PORTB;
    //disable everything

    PORTAbits.RA0 = 1; //SRAM disabled
    PORTAbits.RA1 = 1; //not outputing

    return toSender;
}
