/*
The MIT License (MIT)

Basic remote control test app -  tx code

This s/w is design to work with a second micro:bit 
running the "R1-rx-train-remote-motor-control" application

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

MicroBit    uBit;

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    uBit.radio.enable();

    // setup direction control input pin
    MicroBitPin P2(MICROBIT_ID_IO_P2, MICROBIT_PIN_P2, PIN_CAPABILITY_ALL); // switch directions  - input
    P2.getDigitalValue(PullUp);



    //setup images
    MicroBitImage up("0,0,255,0,0\n0,255,255,255,0\n255,0,255,0,255\n0,0,255,0,0\n0,0,255,0,0\n");              // up arrow
    MicroBitImage dn("0,0,255,0,0\n0,0,255,0,0\n255,0,255,0,255\n0,255,255,255,0\n0,0,255,0,0\n");          //down arrow
    MicroBitImage st("0,255,255,255,0\n255,0,0,0,255\n255,255,255,255,255\n255,0,0,0,255\n0,255,255,255,0\n");    // stop sign
    MicroBitImage rv("0,0,0,255,0\n255,255,255,255,255\n0,0,0,0,0\n255,255,255,255,255\n0,255,0,0,0\n");    // reverse sign



    while(1) {

        if (uBit.buttonA.isPressed() and uBit.buttonB.isPressed()) {
            uBit.radio.datagram.send("4");
            uBit.display.print(st);
            while (uBit.buttonA.isPressed() and uBit.buttonB.isPressed()) {
                uBit.sleep(100);
            }
            if (uBit.buttonA.isPressed())  {
                uBit.radio.datagram.send("F");
                uBit.display.print("F");
                while (uBit.buttonA.isPressed()) {
                    uBit.sleep(100);
                }
            } 
            else if  (uBit.buttonB.isPressed())  {
                uBit.radio.datagram.send("R");
                uBit.display.print("R");
                while (uBit.buttonB.isPressed()) {
                    uBit.sleep(100);
                }
            }
        } 
        else if (uBit.buttonA.isPressed()) {
            uBit.radio.datagram.send("1");
            uBit.display.print(up);
        }


        else if (uBit.buttonB.isPressed()) {
            uBit.radio.datagram.send("2");
            uBit.display.print(dn);
        }

        else if (P2.isTouched()) {
            uBit.radio.datagram.send("3");
            uBit.display.print(rv);
            while (P2.isTouched()) {
                uBit.sleep(100);
            }
            uBit.radio.datagram.send("0");
        }

        uBit.sleep(100);

        uBit.display.clear();
    }

}

