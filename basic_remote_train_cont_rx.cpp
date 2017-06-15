/*
The MIT License (MIT)

Train remote control app - assumes matching tx code running on second microbit

This s/w is design to work with a second micro:bit 
running the "R1-tx-train-motor-control" application

Copyright (c) David Saul 2017


Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBit.h"

MicroBit    uBit;

//define global variables
int lev = 0;
int dir = 0;
MicroBitImage i(5,5); // 5x5 image


void onData(MicroBitEvent)
{
    ManagedString s = uBit.radio.datagram.recv();
    
    MicroBitImage st("0,255,255,255,0\n255,0,0,0,255\n255,255,255,255,255\n255,0,0,0,255\n0,255,255,255,0\n");    // stop sign
    
    if (s == "1")  {            // speed up
        lev=lev+10;
        i.setPixelValue(0,0,255);   // flash top right led
        //uBit.display.print(i);
        if (lev > 1023) {
            lev = 1023;
            i.setPixelValue(2,0,0); // flash middle top led to indicate max power
            uBit.display.print(i);
            uBit.sleep(50);
            i.setPixelValue(2,0,1);
            uBit.display.print(i);
        }
    }

    if (s == "2")   {        // slow down
        lev=lev-10;
        i.setPixelValue(4,0,255);       //flash top left led 
        if (lev <0) {
            lev =0;
            i.setPixelValue(2,4,0);     // flash middle botton led to indcate zero power
            uBit.display.print(i);
            uBit.sleep(50);
            i.setPixelValue(2,4,1);
            uBit.display.print(i);
            
        }
    }

    if (s=="F")  {              // set forward direction
        lev = 0;
        dir = 1;
        uBit.display.print("F");
        }
        
    if (s=="R")  {              // set reverse 
        uBit.display.print("R");
        lev = 0;
        dir = 0;
        }
    
    if (s=="4") {           // check for stop / start
        lev = 0 ; 
        uBit.display.print(st);     // flash stop graphic
        uBit.sleep(50);
        st.clear();
        uBit.display.print(st);
        }

}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    
    // serial channel - for debug only
    //MicroBitSerial serial(USBTX, USBRX);

    //setup ports
    MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_ALL); // pmw forwards
    MicroBitPin P8(MICROBIT_ID_IO_P8, MICROBIT_PIN_P8, PIN_CAPABILITY_ALL); // switch directions  - input

    //remote stuff
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();


    //set speed images
    MicroBitImage S0("0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,255,0,0\n");
    MicroBitImage S1("0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,255,255,255,0\n");
    MicroBitImage S2("0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,255,255,255,0\n0,255,255,255,0\n");
    MicroBitImage S3("0,0,0,0,0\n0,0,0,0,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n");
    MicroBitImage S4("0,0,0,0,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n");
    MicroBitImage S5("0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n");
    


    // set up P0 as PWM output
    P0.setAnalogValue(lev);

    // setup direction control input pin
    P8.setDigitalValue(0);

    // wellcome message
    uBit.display.scroll("Hi", 100);
    
    
// main loop
    
    while(1) {

        // update speed display

        if (lev > 700)
            i.paste(S5);
        else if (lev > 400)
            i.paste(S4);
        else if (lev > 300)
            i.paste(S3);
        else if (lev > 200)
            i.paste(S2);
        else if (lev > 60)
            i.paste(S1);
        else
            i.paste(S0);

        // check for local stop
        if (uBit.buttonA.isPressed() or uBit.buttonB.isPressed()) {
            lev = 0;
            uBit.display.print("S");     // flash S
            uBit.sleep(100);
        }
            


        // update relevant PWM output
        if (dir == 1) {
            P8.setDigitalValue(1);
            P0.setAnalogValue(lev);
        } else {
            P8.setDigitalValue(0);
            P0.setAnalogValue(lev);
        }

        // avoid things running to fast
        uBit.sleep(100);

        // update led display
        uBit.display.print(i);
        
        // output level to serial port -debug only
      //  uBit.serial.printf("%d\r\n",lev);
    }


}

