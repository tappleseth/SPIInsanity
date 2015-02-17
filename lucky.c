/* 
 * File:   lucky.c
 * Author: applet
 *
 * Created on February 11, 2015, 8:01 PM
 */

#include "Smartie.h"
#define PIP_COMMANDER PORTAbits.RA5 

int pollSwitches(unsigned char*, unsigned char*, unsigned char*);

int main(int argc, char** argv) {
    int pickles;
    pickles = 0;
    Setup();
    pickles = Test();
    return pickles;
}

void Setup(){
    //OpenI2C1(MASTER,SLEW_ON);
    
    OpenSPI1( SPI_FOSC_64, MODE_00, SMPMID );
   
    ANSELA = 0;
    ANSELC = 0;
    ANSELB = 0;
    TRISBbits.TRISB7 = 1;
    TRISBbits.TRISB6 = 1;
    TRISBbits.TRISB5 = 1;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB3 = 0;
    TRISAbits.TRISA5 = 0;

    PORTBbits.RB5 = 0;
    PORTBbits.RB4 = 0;
    PORTBbits.RB3 = 0;
    SSP1BUF = 0;
    //RCONbits.IPEN = 0; //no interrupt priorities
   // INTCONbits.RBIE = 1; //port b interrupts on
    // INTCONbits.GIE=1; //global interrupts on
    //INTCON2bits.RBIP = 1; //port b 'interrupt on change' high priority
   // IOCBbits.IOCB7 = 1; //enable B7 ioc
   // IOCBbits.IOCB6 = 1; //enable B6 ioc
   // INTCONbits.RBIF = 0; //sets portb interrupt flag to 0 initially
    //INTCON2bits.INTEDG0 = 1; //interrupt on rising edge

    return;
}

int Test(){

    volatile unsigned char tomLord;
    unsigned char status;
    unsigned char timeLSB;
    unsigned char timeMSB;

    status = 0x01;
    timeLSB = 0x00;
    timeMSB = 0x00;
    tomLord = 0;
    SSP1BUF = 0;
    PIP_COMMANDER = 0;
    while(1){
        
        PIP_COMMANDER = 0;
        
        pollSwitches(&status, &timeLSB, &timeMSB);

        SSP1BUF = status; //sending MSBs of PWM duty cycle
        while (!SSP1STATbits.BF);
        tomLord = SSP1BUF; //LSBs of ADC results from slave

        SSP1BUF = timeLSB; //sending LSBs of PWM duty cycle
        while (!SSP1STATbits.BF);
        tomLord = SSP1BUF; //status report from slave

        SSP1BUF = timeMSB; //sending MSBs of PWM duty cycle
        while (!SSP1STATbits.BF);
        tomLord = SSP1BUF; //LSBs of ADC results from slave
        
        PIP_COMMANDER = 1;
        
    }

    return 1;

}

int pollSwitches(unsigned char* stat, unsigned char* LSB, unsigned char* MSB){
    static unsigned int timerRB6 = 0;
    static unsigned char sampleRB6 = 0;

    static unsigned int timerRB7 = 0;
    static unsigned char sampleRB7 = 0;
    
    //was top switch pressed?
    if ((PORTBbits.RB6 == 1)&&(!sampleRB6)){
        sampleRB6 = 1; //keep an eye on this mfer
    }
    
    //switch not pressed OR depressed
    if (PORTBbits.RB6 == 0){
        sampleRB6 = 0;
    }

    //while button is pressed, count how long
    if (sampleRB6) timerRB6++;

    //if held down long enough, enable MODE SELECT
    if ((timerRB6 >= 400)&&(sampleRB6)){
        sampleRB6 = 0;
        //how long was it pressed?
      if (*stat == 0x01) *stat = 0xFA;
      else if (*stat == 0xFA) *stat = 0x01;
      timerRB6 = 0;
    }

    //if it was pressed down for a little while, change MODES
    else if ((timerRB6 > 10)&&(timerRB6 < 400)&&(!sampleRB6)){
        if (*stat == 0xFA) *stat = 0xFB;
        else if (*stat == 0xFB) *stat = 0xFA;
        else *stat = 0x01;
        timerRB6 = 0;
    }

    if ((PORTBbits.RB7 == 1)&&(!sampleRB7)){
        sampleRB7 = 1;
        timerRB7 = 1;
    }

    if (sampleRB7) timerRB7++;

    if (timerRB7 > 40){
         if (*stat == 0xFA){
             if (*LSB == 0b1111) *LSB = 0;
             else *LSB = *LSB + 1;
         }
         else if (*stat == 0xFB){
             if (*MSB == 0b11) *MSB = 0;
             else *MSB = *MSB + 1;
         }
         timerRB7 = 0;
    }

    if (PORTBbits.RB7 == 0){
        sampleRB7 = 0;
    }



    return 1;
}


/*
#pragma code interruptCommander=0x08
#pragma interrupt interruptCommander
void  interruptCommander(){
    int i;
    //what the hell caused the interrupt?
    PORTBbits.RB3 = 0; //canary in coalmine

    if (INTCONbits.RBIF = 1){ //it was port B! Something was done on port b!!

        if (PORTBbits.RB7 == 1){ //BUTTON: selector button pressed
            if (PORTBbits.RB4 == 0) LATBbits.LATB4 = 1;
            else LATBbits.LATB4 = 0;

            if (status == 0xFA){ //adjusting seconds
                if (timeLSB == 0b1111) timeLSB = 0;
                else  timeLSB++;
            } else if (status == 0xFB){//adjusting minutes
                if (timeMSB == 0b11) timeMSB = 0;
                else timeMSB++;
            }

        }

        if (PORTBbits.RB6 == 1){ //BUTTON: command confirm button pressed
            for (i = 0; i < 10000 ; i++);
            //switch 2 held down!
            if (PORTBbits.RB6 == 1){ 
          
             if (PORTBbits.RB5 == 0){
                    status = 0xFA; //HALT, WE ARE SETTING CLOCK (seconds)
             }
             else{
                 status = 0x01; //GO, WE ARE DONE SETTING CLOCK
             }
            } 
            //switch 2 was only pressed briefly!
            else {
                if (status == 0xFA){
                     status = 0xFB; //switch to adjusting minutes
                }
                else if (status == 0xFB){
                    status = 0xFA;
                }
            }
        }
            
        //reset port b interrupt flag
        INTCONbits.RBIF = 0;
    }

    return;

}
*/
