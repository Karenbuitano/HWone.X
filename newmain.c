
#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro
#include"i12c_display.h"
#include"i2c_master_int.h"
#include "accel.h"

// DEVCFG0
    #pragma config DEBUG = OFF // no debugging
    #pragma config JTAGEN = OFF // no jtag
    #pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
    #pragma config PWP = OFF // no write protect
    #pragma config BWP = OFF // not boot write protect
    #pragma config CP = OFF // no code protect

// DEVCFG1
    #pragma config FNOSC = PRIPLL // use primary oscillator with pll
    #pragma config FSOSCEN = OFF // turn off secondary oscillator
    #pragma config IESO = OFF // no switching clocks
    #pragma config POSCMOD = HS // high speed crystal mode
    #pragma config OSCIOFNC = OFF // free up secondary osc pins
    #pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for bus clock
    #pragma config FCKSM = CSDCMD // do not enable clock switch
    #pragma config WDTPS = PS1048576 // slowest wdt
    #pragma config WINDIS = OFF // no wdt window
    #pragma config FWDTEN = OFF // wdt off by default
    #pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 40MHz
    #pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
    #pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV
    #pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 40MHz
    #pragma config UPLLIDIV = DIV_2 // divide 8MHz input clock
    #pragma config UPLLEN = ON // USB clock on

// DEVCFG3
    #pragma config USERID = 0 // some 16bit userid
    #pragma config PMDL1WAY = ON // not multiple reconfiguration, check this
    #pragma config IOL1WAY = ON // not multimple reconfiguration, check this
    #pragma config FUSBIDIO = ON // USB pins controlled by USB module
    #pragma config FVBUSONIO = ON // controlled by USB module

int readADC(void);
int display_message_i(char message,int counter);
int display_acc(int numpixels[2]);
int main() {
    // startup
    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that
    // kseg0 is cacheable (0x3) or uncacheable (0x2)
    // see Chapter 2 "CPU for Devices with M4K Core"
    // of the PIC32 reference manual
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    
    // no cache on this chip!

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
    DDPCONbits.JTAGEN = 0;

    acc_setup();
    __builtin_enable_interrupts();
    
    
    
   
   //This works to print just characters to the screen
   
   // char message[100];
    //sprintf(message,"Hey hey hey!");
    //int counter=0;
    //while(message[counter]) {
       // display_message_i(message[counter],counter);
       // counter++;
    //}
   // display_draw();
    
    
    short accels[3]; // accelerations for the 3 axes

    short mags[3]; // magnetometer readings for the 3 axes

    short temp;
    
    // read the accelerometer from all three axes

// the accelerometer and the pic32 are both little endian by default (the lowest address has the LSB)

// the accelerations are 16-bit twos compliment numbers, the same as a short



// need to read all 6 bytes in one transaction to get an update.
// accels[0]= the bit for x
//accels[1]= the bit for y
// while loop. accels is the output. If accels is positive in the x direction, do something if accels is negative in the x direction, do something. If it is positive in the y direction, do something. If it is negative in the y direction, do something.

//I want to read in an x value which correspond to accels[0]
//I want to read in a y value which correspond to accels[1]

//I want to constantly update so that the numbers change
//I know that accels is [x y z] with 6 bites of data x=2 bytes, y= 2 bytes, z=2 bytes
//while loop so that constantly being updated
//if in the x direction: 
//loop, go through xg/128 times 2g in 64 pixels, light up that many pixels, clear the remaining pixels, 
//if changing direction, clear all pixels before, fill in all after origin
//for loop through 64 times, 
//turn on bits until pixels= number of gs
//turn everything after that off
// display pixels
//while loop again 







    int numpixels[2];
    int ii=0;
    float g[3];
 while (1){
 acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);
 display_init();
 display_clear();
 for (ii=0;ii<3;ii++){
    g[ii]=(float)accels[ii]/(float) 16384; 
 }
 numpixels[0]=(float)accels[0]/(float)512;

 numpixels[1]=(float)accels[1]/(float)1024;
 char message[100];
    sprintf(message," %d %d ", numpixels[0],numpixels[1]);//g[2]);
    //sprintf(message,"hey hey");
    int counter=0;
    while(message[counter]) {
       display_message_i(message[counter],counter);
        counter++;
    }
    display_draw();
   //r++;
 }
    
    

/*else if(accels[0]<0){
    sprintf(message,"the negative x acceleration is", accels[0]);
    int counter=0;
    while(message[counter]) {
       display_message_i(message[counter],counter);
        counter++;
    }
    display_draw();
}
else if(accels[1]>0){
    sprintf(message,"the positive ")
}*/


acc_read_register(OUT_X_L_M, (unsigned char *) mags, 6);

// read the temperature data. Its a right justified 12 bit two's compliment number

acc_read_register(TEMP_OUT_L, (unsigned char *) &temp, 2);
    
    // set up USER pin as input
    ANSELBbits.ANSB13 = 0; // sets pin B13 to digital input

    // set up LED1 as digital output
    TRISBbits.TRISB7 = 0;
    //RPB7Rbits.RPB7R = 0b0001; // set B7 to U1TX
    
    // Set LED2 as OC1 using Timer2 at 1kHz
    RPB15Rbits.RPB15R = 0b0101; // sets pin B15 to output compare
    T2CONbits.TCKPS = 0;		// sets pre-scaler to 1
	PR2 = 39999;				// sets maximum counter value
	TMR2 = 0;					// initializes timer 2
	OC1CONbits.OCM = 0b110;		// use without fail safe
	OC1RS = 2000;				// sets duty cycle to 50%
	OC1R = 2000;				// sets initial duty cycle to 50%
	T2CONbits.ON = 1;			// turns timer on
	OC1CONbits.ON = 1;			// turns output control on

    // Read the analog input
    ANSELAbits.ANSA0 = 1; // sets pin A0 to analog input
    AD1CON3bits.ADCS = 3;
    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.ADON = 1;
    
    while (1) {
        _CP0_SET_COUNT(0); // set core timer to 0
        LATBINV = 0b10000000; // invert pin B7 for LED1

        // wait for half a second, setting LED brightness to pot angle
        while (_CP0_GET_COUNT() < 10000000) {
            int val;
            val = readADC();
            OC1RS = val * 39; // value/1023 * 400000 simplifies to about 39

            if (PORTBbits.RB13 == 1) {
                // nothing
            } else {
                LATBINV = 0b10000000; // invert LED1 as fast as possible
            }
        }
    }
    

}

int readADC(void) {
    int elapsed = 0;
    int finishtime = 0;
    int sampletime = 20;
    int a = 0;

    AD1CON1bits.SAMP = 1;
    elapsed = _CP0_GET_COUNT();
    finishtime = elapsed + sampletime;
    while (_CP0_GET_COUNT() < finishtime) {
    }
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE) {
    }
    a = ADC1BUF0;
    return a;
}

static const char ASCII[96][5] = {
 {0x00, 0x00, 0x00, 0x00, 0x00} // 20  (space)
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c �
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ?
,{0x00, 0x06, 0x09, 0x09, 0x06} // 7f ?
}; // end char ASCII[96][5]

int row, col;
int counter;
int display_message_i(char message,int counter) {
        int ASCII_variable, ASCII_arr[5];
        int ii, k, dummy[8][1] = {1, 1, 1, 1, 1, 1, 1, 1}, line, character[8], bits[8][5];

        for (ii=0; ii<5; ii++) {
            ASCII_variable = message - 0x20; // gets row number in ASCII
            ASCII_arr[ii] = ASCII[ASCII_variable][ii]; 

            line = ASCII_arr[ii];
            for (k=0; k<8; k++) { 
                bits[k][ii] = line & 1;
                line>>=1;
            }
        }

        
        for (col=0; col<5; col++) {
            for (row=0; row<8; row++) {
                display_pixel_set(row,col+(counter*5),bits[row][col]);
            }
        }
}

//int display_acc(int numpixels[2]){
    // want to create a 128 x 64 array
// send to pixel set the number of pixels out of the 128 I want to turn on in the x direction, number out of the 64 I want to turn on in the y direction
// 
    //ggdram= array of 128 x 64 already
// function of (start x, start y, number)
// number is numpixels[0] or numpixels[1] (the number of pixels in the x or y direction)
    //
    //int row, col;
        //for (col=0; col<5; col++) {
            //for (row=0; row<8; row++) {
                //display_pixel_set(row,col+(counter*5),bits[row][col]);
            //}
        //}
//}   
int display_acc_x(int startx,int starty, int numpixels[2]){
    startx=64;
    starty=32;
    int m;int j;
    if (numpixels[0]>0){ 
    for (m=1;m<numpixels[0];m++){
        for (j=1;j<2;j++){
            if (startx+m<128){
               display_pixel_set(startx+m, starty+j,1); 
            }
        }
    }
    
    
}
}





