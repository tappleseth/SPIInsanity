/* 
 * File:   CAscreen.c
 * Author: applet
 *
 * Created on February 12, 2015, 3:46 PM
 */

#include "CA.h"

int main(int argc, char** argv) {
    int test;
    Setup();
    test = Test();
    return 0;
}

void Setup(){
    //OpenI2C1(MASTER,SLEW_ON);
     OpenSPI1(SLV_SSON, MODE_00, SMPMID);

    ANSELA = 0;
    ANSELC = 0;
    ANSELB = 0;
    TRISB = 0;
    PORTB = 0;

    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC1 = 0;
    TRISBbits.TRISB1 = 1;

    PORTBbits.RB1 = 0;

    PORTCbits.RC1 = 0;
    PORTCbits.RC2 = 0;

    PR2 = 0x01;

    T1GCONbits.T1GSS0 = 1;
    T1GCONbits.T1GSS1 = 0;
    
    T1GCONbits.TMR1GE = 1;
    T1CONbits.TMR1ON = 1;

    T1CONbits.TMR1CS1 = 0;
    T1CONbits.TMR1CS0 = 0;

    T1CONbits.TMR1ON = 1;
    T1CONbits.T1CKPS0 = 0;
    T1CONbits.T1CKPS1 = 0;

    /* RCONbits.IPEN = 0; //no interrupt priorities
    INTCONbits.RBIE = 1; //port b interrupts on
    INTCONbits.GIE=1; //global interrupts on
   // INTCON2bits.RBIP = 1; //port b 'interrupt on change' high priority
    IOCBbits.IOCB7 = 1; //enable B7 ioc
    IOCBbits.IOCB6 = 1; //enable B6 ioc
    INTCONbits.RBIF = 0; //sets portb interrupt flag to 0 initially
    //INTCON2bits.INTEDG0 = 1; //interrupt on rising edge
   */
     }

int Test(){
    unsigned char status;
    unsigned char timeLSB;
    unsigned char timeMSB;

    unsigned char tempLSB;
    unsigned char tempMSB;

    unsigned long int clock;
    clock = 0;

    timeLSB = 0;
    timeMSB = 0;
    tempLSB = timeLSB;
    tempMSB = timeMSB;

    status = 0x01;
    SSP1BUF = 0;
    while (PORTCbits.RC3);
    while(1){

    SSP1BUF = 0xFA; //sending dummy data
    while (!SSP1STATbits.BF);
    status = SSP1BUF;
    //bullshit = SSP1BUF;

    SSP1BUF = 0xFF; //sending dummy data
    while (!SSP1STATbits.BF);
    tempLSB = SSP1BUF;

    SSP1BUF = 0xF4; //sending dummy data
    while (!SSP1STATbits.BF);
    tempMSB = SSP1BUF;

    if (status == 0x01){
        //if (clock == 100){
        if (TMR1L == 0xFF){
            //clock = 0;
            //TMR1L = 0;
           // TMR1H = 0;
        PORTCbits.RC1 = 1;
        PORTCbits.RC2 = 0;
        if (timeLSB == 0b1111){
            timeLSB = 0;
            if (timeMSB == 0b11) timeMSB = 0;
            else timeMSB++;
            
        }
        else timeLSB++;
        } else clock++;
    } else if ((status == 0xFA)||(status == 0xFB)){
        timeLSB = tempLSB;
        timeMSB = tempMSB;
        PORTCbits.RC1 = 0;
        PORTCbits.RC2 = 1;
    }
  
        LATBbits.LATB4 = (timeLSB>>3)&0x1;
        LATBbits.LATB5 = (timeLSB>>2)&0x1;
        LATBbits.LATB6 = (timeLSB>>1)&0x1;
        LATBbits.LATB7 = (timeLSB)&0x1;

        LATBbits.LATB3 = (timeMSB&0x1);
        LATBbits.LATB2 = (timeMSB>>1)&0x1;
    

    }
 return 1;
    }

   





