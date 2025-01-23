/*

  Testing the 6-Channel Volume Controller IC PT2258
  Uses the PT2258 library v 1.0.0 by Marco Lurati
  Tested with Arduino Uno in IDE v2.3.4

  PT2258 address
  (1: connected to VCC, 0: connected to GND)
  | CODE1 | CODE2 | ADDRESS 8bit | ADDRESS 7bit |
  |   0   |   0   |    0x80      |    0x40      |
  |   1   |   0   |    0x88      |    0x44      |
  |   0   |   1   |    0x84      |    0x42      |
  |   1   |   1   |    0x8C      |    0x46      |

  The Wire library uses addresses with 7bit, so if you perform an I2C scan,
  the address will appear as right-shifted form the one specified
  above and in the data sheet.
  Example: I2C scan finds 0x44 -> this means that the PT2258 has the address 0x88

  Mute
  ----
  The PT2258 powers up with all channels muted and all channels at maximum volume.  
  Unmute to pass signals

  Sketch modified from example created by Marco Lurati, April 21, 2023
  
  MIT License

  Copyright (c) 2023 marclura

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#include <PT2258.h>
#include <Wire.h>

PT2258 pt2258(0x88);  // device address for code1 pin connected to VCC, code2 pin connected to gnd

int fadeDelay = 2000;  // volume ramp up/down (tremolo) rate delay in uS

void setup() {

  Serial.begin(115200);

  Wire.begin();  // start the I2C bus
  //Wire.setClock(100000);  // the PT2258 is specified to work with a bus clock of 100kHz

  if (!pt2258.begin()) {  // initialise the PT2258
    Serial.println("PT2258 init error");
    while (1) {};
  }

  // set first 5 channels to fixed levels
  pt2258.attenuation(1, 79);  // set ch 1 to 79 dB attenuation
  pt2258.attenuation(2, 14);  // set ch 2 to 14 dB attenuation
  pt2258.attenuation(3, 6);   // set ch 3 to 6 dB attenuation
  pt2258.attenuation(4, 2);   // set ch 4 to 2 dB attenuation
  pt2258.attenuation(5, 0);   // set ch 5 to 0 dB attenuation

  // set level of 6th channel to full attenuation and change it in the loop
  pt2258.attenuation(6, 79);  // set ch 6 to 79 dB attenuation
  pt2258.mute(false);         // unmute channels since mute is active at power up.
}

void loop() {
  // ramp ch 6 volume up and down to create a tremolo effect
  for (byte dB = 0; dB < 40; dB++) {
    pt2258.attenuation(6, dB);  // set current attenuation
    if (dB <= 14)
      delayMicroseconds(fadeDelay);  // adjust ramp speed to account for log attenuation curve ie ramp faster when each step is harder to discern
    else
      delayMicroseconds(fadeDelay / 2);
  }

  for (byte dB = 40; dB > 0; dB--) {
    pt2258.attenuation(6, dB);  // set current attenuation
    if (dB <= 14)
      delayMicroseconds(fadeDelay);  // adjust ramp speed to account for log attenuation curve ie ramp faster when each step is harder to discern
    else
      delayMicroseconds(fadeDelay / 2);
  }
}
