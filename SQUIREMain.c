/*
 * File:   Main.c
 * Author: applet
 *
 * Created on January 26, 2015, 3:23 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include <p18F25K22.h>
#include <usart.h>
#include <spi.h>
#include <pwm.h>
#include <timers.h>

#include "squireCommander.h"

int squireIndex = 0; //global SRAM index variable
//if I had more time I'd rewrite squire code to be as nice as the knight code
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
   //all are initially set to be inputs
   TRISB = 0xFF;
   //all are initially low (note: with TRISB=0xFF we don't rly need this now)
   PORTBbits.RB0 = 0;
   PORTBbits.RB1 = 0;
   PORTBbits.RB2 = 0;
   PORTBbits.RB3 = 0;
   PORTBbits.RB4 = 0;
   PORTBbits.RB5 = 0;
   PORTBbits.RB6 = 0;
   PORTBbits.RB7 = 0;

   //PWM SETUP STUFF
   //CCP1CON = 0; //clear CCPxCON to relinquish control of pin
   CCPTMRS0 = 0x00;
   PR2 = 0xF9;
   CCP1CON = 0b00001100;

   CCPR1L = 0x02; //initial duty cycle
   T2CON = 0b00000100; //<-timer (first 2 bits) (5,4)
   //T1CON = 0b00000111;

   TRISCbits.TRISC2 = 0;

   /* ADC setup*/
   ANSELAbits.ANSA5 = 1;
   TRISAbits.RA5 = 1;
   ADCON0 = 0b00010001; //change MSBs back to 00 if ntohing works
   ADCON1 = 0b00000000;
   ADCON2 = 0b00000101; //LEFT JUSTIFIED OUTPUT
   ADRESH = 0;
   ADRESL = 0;



}
int SPI_SQUIRE_COMMANDER(void){

    unsigned char tomStar, oldLSB, loBytes, hiBytes;
    unsigned int tempL, tempH, voltage;

    char returnToSender[4]; //return package
    oldLSB = 0b00001100;
    returnToSender[0] = 0xAA;
    returnToSender[1] = 0xAA;
    returnToSender[2] = 0xAA;
    returnToSender[3] = 0xAA;

    //int phailDetectorUnit;
    //phailDetectorUnit = 0;wwwwwwwwwwwwww
    /* TOM INDUSTRIES PRESENTS: HOLY GOD DAMN 4 LINES?! */
    while(1){

        SSP1BUF = returnToSender[0]; //SEND: status flag
        while (!SSP1STATbits.BF);
        loBytes = SSP1BUF; //RECIEVE: lower bits of desired PWM voltage

        SSP1BUF = returnToSender[1]; //SEND: lower bits of ADC
        while (!SSP1STATbits.BF);
        hiBytes = SSP1BUF; //RECIEVE: upper bits of desired PWM voltage

        SSP1BUF = returnToSender[2]; //SEND: upper bits of ADC
        while (!SSP1STATbits.BF);
        tomStar = SSP1BUF; //RECIEVE: not used (DO NOT DELETE THIS LINE)

        SSP1BUF = returnToSender[3]; //SEND: difference between PWM, ADC bits
        while (!SSP1STATbits.BF);
        //RECIEVE: not used (no need for dummy read here because...
        //...we wrap around to next SSP1BUF read (like moebius strip)

        writeSRAM(loBytes,squireIndex); //save LSBs
        squireIndex++;
        writeSRAM(hiBytes,squireIndex); //save MSBs
        squireIndex++;

        if (squireIndex > 255){ //currently using 8 bits of addresses
            squireIndex = 0;
        }

        //not dumping voltage values at this time; possible new feature?
        //hiBytes = readSRAM(squireIndex); //retrieve MSBs
        //loBytes = readSRAM(squireIndex-1); //retrieve LSBs
        //tempL = (unsigned int) loBytes;
        //tempH = (unsigned int) hiBytes;
        //voltage = (tempL >> 4) + (tempH << 2); //little tired, should work

        CCP1CON = oldLSB | loBytes;
        CCPR1L  = hiBytes;

        //read motor voltage
        fancyADCThings(returnToSender);
        //next: check error
        checkError(returnToSender,loBytes,hiBytes);
    }
return 0;//phailDetectorUnit;
}

void checkError(unsigned char* returnPackage, unsigned char loBytes, unsigned char hiBytes){
    //need to compare duty cycle bits to ADC bits
    unsigned int tempH, tempL, totalDC, totalADC;
    int difference;

    tempH = (unsigned int) hiBytes;
    tempL = (unsigned int) loBytes;

    totalDC = (tempH << 2) + (tempL >> 4); //OKAY

    tempH = (unsigned int) returnPackage[2];
    tempL = (unsigned int) returnPackage[1];
    totalADC = (tempH << 2) + (tempL >> 6);

    difference = (int)(totalADC - totalDC);
    /* TOM EXPLAINS THE ERROR DETECTOR
     * if each bit is +-.10% duty cycle
     * PWM and ADC bits use same resolution, both setting voltage for same thing
     * (you can see how easy this actually is)
     * PIC sucks at floating point math but we don't even care because:
     * we know that 1% difference is 10 bits difference
     * 2% is 20 bits difference, 5% is 50 bits difference
     * use debug mode to confirm! flag is 0xAA
     */
    if (((difference < 10) && (difference > 0))||((difference < 0) && (difference > -10))){
        //0xEA is the "it's all cool man" flag
        returnPackage[0] = 0xEA;
    } else if ((difference >= 10) && (difference < 20)){
        //level 2, of concern, too high
        returnPackage[0] = 0xBA;
    } else if ((difference >= 20) && (difference < 50)){
        //level 1, moderate, too high
        returnPackage[0] = 0xCA;
    } else if (difference >= 50){
        //level 0, severe, too high
        returnPackage[0] = 0xDA;
    } else if ((difference < 0) && (difference >= -20)){
        //level 2, of concern, too low
        returnPackage[0] = 0xBB;
    } else if ((difference < -20) && (difference > -50)){
        //level 1, moderate, too low
        returnPackage[0] = 0xCB;
    } else if (difference <= -50){
        //level 1, severe, too low
        returnPackage[0] = 0xDB;
    }
    else {
        returnPackage[0] = 0xAA;
    }
    if (difference < 0) difference = difference * (-1);
    //send the difference in voltage back to KNIGHT COMMANDER
    returnPackage[3] = (unsigned char) difference;

    //KNIGHT COMMANDER will know what to do

    return;
}
void fancyADCThings(unsigned char *returnPackage){
    ADCON0 ^= 2; //enable ADC capture by setting go/done bit
    //^^ the go/done bit will be reset by hardware
    while (!PIR1bits.ADIF); //wait for ADC process to finish
    PIR1bits.ADIF = 0; //reset ADIF bit (must be reset by software)
    returnPackage[0] = 0xEA; //assume things are fine at first
    returnPackage[1] = ADRESL; //low bits
    returnPackage[2] = ADRESH; //high bits (only want lower 2 in ADRESH)
    return;
}

void writeSRAM(unsigned char data, int address){
/*TRISAbits.RA0 = 0 //!CS
   TRISAbits.RA1 = 0; //!OE
   TRISAbits.RA2 = 0; //!WE
   TRISAbits.RA3 = 0; //latch
 */
    int delay;

    PORTAbits.RA0 = 0; //SRAM enabled
    PORTAbits.RA1 = 1; //not outputing
    PORTAbits.RA2 = 1; //writing TO the SRAM

    //IO pins set to be outputs
    TRISB = 0x00;
   // PORTB = 0x00;

    LATB = address;

    LATAbits.LATA3 = 1; //latch address
    LATAbits.LATA3 = 0; //de-latch address

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
    int delay;
    unsigned char toSender;

    //IO pins set to inputs
    TRISB = 0x00;

    PORTAbits.RA0 = 0; //SRAM enabled
    PORTAbits.RA1 = 1; //not outputing
    PORTAbits.RA2 = 1; //writing TO the SRAM

    //LATA = address;
    LATB = address;

    LATAbits.LATA3 = 1; //latch address
    LATAbits.LATA3 = 0; //de-latch address


    TRISB = 0xFF;

    PORTAbits.RA1 = 0; //output is a GO!

    //give some time to read
    toSender = PORTB;
    //disable everything

    PORTAbits.RA0 = 1; //SRAM disabled
    PORTAbits.RA1 = 1; //not outputing

    return toSender;
}

