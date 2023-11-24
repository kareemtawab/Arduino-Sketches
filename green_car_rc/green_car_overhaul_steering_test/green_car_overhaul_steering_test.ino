/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <ServoTimer2.h>

ServoTimer2  myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
    myservo.write(1050);              // tell servo to go to position in variable 'pos'
    // delay(1000);
    // myservo.write(1700);              // tell servo to go to position in variable 'pos'
    // delay(1000);
    // myservo.write(1500);              // tell servo to go to position in variable 'pos'
    // delay(1000);
    // myservo.write(1300);              // tell servo to go to position in variable 'pos'
    // delay(1000);
}
