
//      ******************************************************************
//      *                                                                *
//      *         Simple example for controlling a stepper motor         *
//      *                                                                *
//      *            S. Reifel & Co.                6/24/2018            *
//      *                                                                *
//      ******************************************************************


// This is the simplest example of how to run a stepper motor.
//
// Documentation for this library can be found at:
//    https://github.com/Stan-Reifel/SpeedyStepper
//
//
// This library requires that your stepper motor be connected to the Arduino
// using drive electronics that has a "Step and Direction" interface.
// Examples of these are:
//
//    Pololu's DRV8825 Stepper Motor Driver Carrier:
//        https://www.pololu.com/product/2133
//
//    Pololu's A4988 Stepper Motor Driver Carrier:
//        https://www.pololu.com/product/2980
//
//    Sparkfun's Big Easy Driver:
//        https://www.sparkfun.com/products/12859
//
//    GeckoDrive G203V industrial controller:
//        https://www.geckodrive.com/g203v.html
//
// For all driver boards, it is VERY important that you set the motor
// current before running the example.  This is typically done by adjusting
// a potentiometer on the board.  Read the driver board's documentation to
// learn how.

// ***********************************************************************


#include <SpeedyStepper.h>

const int MOTOR_STEP_PIN = 7;
const int MOTOR_DIRECTION_PIN = 4;

SpeedyStepper stepper;

void setup()
{
  stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);
  stepper.setSpeedInStepsPerSecond(40500);
  stepper.setAccelerationInStepsPerSecondPerSecond(40000);
}

void loop()
{
  stepper.moveRelativeInSteps(10000);
  delay(100);
  stepper.moveRelativeInSteps(-10000);
  delay(100);
}
