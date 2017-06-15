/*
The MIT License (MIT)

Train remote control app - assumes matching tx code running on second microbit
this is a development of R1 that accepts a single byte to as the speed

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
int lev_lb = 0;
int lev_hb = 0;
int dir = 0;
int st_sp = 0;


MicroBitImage i(5,5); // 5x5 image


void onData(MicroBitEvent)

{
    
    PacketBuffer p = uBit.radio.datagram.recv();
    lev_hb = p[0];
    lev_lb = p[1];
    
    if (p[2] == 0 or p[2] == 1)     {    // set direction
        dir = 0;
        i.setPixelValue(0,0,1); // set top left pixel to indicate forward
        
        }
    else  {
        dir = 1;
        i.setPixelValue(4,0,1); // set top right pixel to indicate forward
       
        }
        
    if (p[2] == 0 or p[2] == 4) {          // start / stop
        i.setPixelValue(4,0,0); // clear direction pixels
        i.setPixelValue(0,0,0); // 
        st_sp = 0;
        }
    else
        st_sp = 1;
        
    
    lev = (4*lev_hb)+lev_lb;
      
      
    i.setPixelValue(2,2,0); // flash middle top led to indicate message recieved
    uBit.display.print(i);
    uBit.sleep(20);
    i.setPixelValue(2,2,1);
    uBit.display.print(i);
    
 
    
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    
    // serial channel - for debug only
    MicroBitSerial serial(USBTX, USBRX);

    //setup ports
    MicroBitPin P1(MICROBIT_ID_IO_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_ALL); // pmw forwards
    MicroBitPin P16(MICROBIT_ID_IO_P16, MICROBIT_PIN_P16, PIN_CAPABILITY_ALL); // switch directions  - input

    //MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_ALL); // pmw forwards
    //MicroBitPin P8(MICROBIT_ID_IO_P8, MICROBIT_PIN_P8, PIN_CAPABILITY_ALL); // switch directions  - input

    //remote stuff
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();
    // Create a packet containing 2 bytes.
    //PacketBuffer P(2);


    //set speed images
    MicroBitImage S0("0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,255,0,0\n");
    MicroBitImage S1("0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,255,255,255,0\n");
    MicroBitImage S2("0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,255,255,255,0\n0,255,255,255,0\n");
    MicroBitImage S3("0,0,0,0,0\n0,0,0,0,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n");
    MicroBitImage S4("0,0,0,0,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n");
    MicroBitImage S5("0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n");
    


    // set up P0 as PWM output
    P1.setAnalogValue(lev);

    // setup direction control input pin
    P16.setDigitalValue(0);

    // wellcome message
    uBit.display.scroll("Hi", 100);
    
    
// main loop
    
    while(1) {
    
        // update speed display

        if (lev > 1000)
            i.paste(S5);
        else if (lev > 700)
            i.paste(S4);
        else if (lev > 500)
            i.paste(S3);
        else if (lev > 300)
            i.paste(S2);
        else if (lev > 100)
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
        if (st_sp == 1) {                   // start - stop
            if (dir == 1) {
                P16.setDigitalValue(1);
                P1.setAnalogValue(lev);
                } 
            else {
                P16.setDigitalValue(0);
                P1.setAnalogValue(lev);
                }
            }    
        else {
            P1.setAnalogValue(0);
            }

        // avoid things running to fast
        uBit.sleep(50);

        // update led display
        uBit.display.print(i);
        
        // output level to serial port -debug only
        //uBit.serial.printf("%d\r\n",p[0]);
       // uBit.serial.printf("%d\r\n",p[1]);
      // uBit.serial.printf("%d\r\n",p[2]);
    }


}

