/*
The MIT License (MIT)

Basic remote control test app -  tx code
speed control by using accell input
inc start / stop and reverse using buttons

Copyright (c) David Saul

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
#include <string>

MicroBit    uBit;

MicroBitImage i(5,5); // setup blank 5x5 image

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    uBit.radio.enable();

    // Create a packet containing just a 2 bytes.
    PacketBuffer mess(3);
    mess[0] = 0;            // speed high byte
    mess[1] = 0;            // speed low byte
    mess[2] = 0;            // direction start / stop [1/0] and forward / reverse [4/0]

    // serial channel - for debug only
    MicroBitSerial serial(USBTX, USBRX);


    // setup direction control input pin
    MicroBitPin P2(MICROBIT_ID_IO_P2, MICROBIT_PIN_P2, PIN_CAPABILITY_ALL); // switch directions  - input
    P2.getDigitalValue(PullUp);



    //setup images
    //set speed images
    MicroBitImage S0("0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,255,0,0,0\n");
    MicroBitImage S1("0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,255,0,0,0\n");
    MicroBitImage S2("0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,255,0,0,0\n0,255,0,0,0\n");
    MicroBitImage S3("0,0,0,0,0\n0,0,0,0,0\n0,255,0,0,0\n0,255,0,0,0\n0,255,0,0,0\n");
    MicroBitImage S4("0,0,0,0,0\n0,255,0,0,0\n0,255,0,0,0\n0,255,0,0,0\n0,255,0,0,0\n");
    MicroBitImage S5("0,255,0,0,0\n0,255,0,0,0\n0,255,0,0,0\n0,255,0,0,0\n0,255,0,0,0\n");

    char buf[10];

    // wellcome message
    uBit.display.scroll("Tx-Hi",80);
    int ytmp = uBit.accelerometer.getY();

    while(1) {          // limit operation to positive accel results - 9oclock to 3 oclock

        int y = uBit.accelerometer.getY();

        if (y<0)
            y=0;
        if (y>1023)
            y=1023;

        if (y > ytmp + 18 or y < ytmp -18 ) {       // basic noise filter ignores lower 5 bits
                                                    // only send message on change   

            // parse integer number in to 2 bytes so it can be sent as a simple data gram message
            int yhb = y / 4;
            int ylb = y-(yhb*4);

            mess[0] = yhb;
            mess[1] = ylb;
            uBit.radio.datagram.send(mess);

            ytmp = y;           // update temp accel number

            /*
                    // output accel data to serial port -debug only

                    sprintf(buf, "%i", y);
                    uBit.serial.printf("%s\r\n",buf);
                    sprintf(buf, "%i", yhb);
                    uBit.serial.printf("%s\r\n",buf);
                    sprintf(buf, "%i", ylb);
                    uBit.serial.printf("%s\r\n",buf);
                    sprintf(buf, "%i", mess[2]);
                    uBit.serial.printf("%s\r\n",buf);
              */

            // update speed display
            if (y > 1000)
                i.paste(S5);
            else if (y > 700)
                i.paste(S4);
            else if (y> 500)
                i.paste(S3);
            else if (y > 300)
                i.paste(S2);
            else if (y > 100)
                i.paste(S1);

            if (mess[2] == 1 or mess[2] == 5)  {
                i.setPixelValue(4,0,1); //     indicate power is active with 2 leds
                i.setPixelValue(3,0,1);
            } else {
                i.setPixelValue(4,0,0); //     indicate power is off with 2 leds
                i.setPixelValue(3,0,0);
            }

            // update led display
            uBit.display.print(i);

        }

        // check for buttons
        if (uBit.buttonA.isPressed()) {                 // button A - Start / Stop
                      
            if (mess[2] == 0 or mess[2] == 4)
                mess[2] = mess[2] + 1;
            else
                mess[2] = mess[2] - 1;

            uBit.radio.datagram.send(mess);             // update message
            
            while (uBit.buttonA.isPressed())            // wait for button to be released
                uBit.sleep(50);
                
            }

        if (uBit.buttonB.isPressed()) {                 // button B - Forward / Backwards
        
         uBit.sleep(250);
            if (uBit.buttonB.isPressed()) { 
        
                if (mess[2] == 0 or mess[2] == 1) {
                    mess[2] = mess[2] + 4;
                    uBit.display.print("F");
                } 
                else {
                    mess[2] = mess[2] - 4;
                    uBit.display.print("R");
                }

                mess[0] = 0;     // stop train
                mess[1] = 0;     // this avoids you immediatly reversing into something !
                i.paste(S1);     // force display image to 'slow'


                while (uBit.buttonB.isPressed())            // wait for button to released
                    uBit.sleep(50);

                uBit.radio.datagram.send(mess);             // update message
                // sprintf(buf, "%i", mess[2]);
                // uBit.serial.printf("%s\r\n",buf);

                while (uBit.accelerometer.getY()>0) {       // force user to start train from zero speed
                    uBit.display.print("w");                // user has to rotate micro bit passed   
                    uBit.sleep(25);                         //  9/3 oclock to reset train    
                    uBit.display.print(" ");
                    uBit.sleep(25);
                    }
                }
            else  {
                while (uBit.buttonB.isPressed()==0) {    // stay in this loop until button B is pressed again
                    i.setPixelValue(3,2,1);         // Display small 'L' to indicate
                    i.setPixelValue(3,3,1);         // speed is locked     
                    i.setPixelValue(3,4,1);    
                    i.setPixelValue(4,4,1);
                    
                    // update led display
                    uBit.display.print(i); 
                    }

                              
                while (uBit.buttonB.isPressed())            // wait for button to released
                    uBit.sleep(50);
                    
                // Force user to match speed before restoring control
                // First rotate controller to 9/3 oclock then
                // the controller will cut in when the speed is matched
                
                while (uBit.accelerometer.getY() > 0) {       
                    
                    i.setPixelValue(3,2,1);         // Flash small 'L' to indicate
                    i.setPixelValue(3,3,1);         // waiting for zero rotate     
                    i.setPixelValue(3,4,1);    
                    i.setPixelValue(4,4,1);
                    uBit.display.print(i);                    
                    uBit.sleep(25);                             
                    i.setPixelValue(3,2,0);         
                    i.setPixelValue(3,3,0);             
                    i.setPixelValue(3,4,0);    
                    i.setPixelValue(4,4,0);  
                    uBit.display.print(i);                     
                    uBit.sleep(25);
                    }                   
                
                while (uBit.accelerometer.getY()< y) {       
                    i.setPixelValue(3,2,1);         // flash small 'P' to indicate
                    i.setPixelValue(3,3,1);         // waiting match speeds     
                    i.setPixelValue(3,4,1);    
                    i.setPixelValue(4,2,1); 
                    i.setPixelValue(4,3,1);
                    uBit.display.print(i);                                        
                    uBit.sleep(25);                           
                    i.setPixelValue(3,2,0);        
                    i.setPixelValue(3,3,0);           
                    i.setPixelValue(3,4,0);    
                    i.setPixelValue(4,2,0); 
                    i.setPixelValue(4,3,0);
                    uBit.display.print(i);                        
                    uBit.sleep(25);
                    }
                
                }             
        }

        // make sure things don't happen to quickly
        uBit.sleep(100);

    }

}

