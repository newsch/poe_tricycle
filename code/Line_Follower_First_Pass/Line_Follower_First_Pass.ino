#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *left=AFMS.getMotor(1);
Adafruit_DCMotor *right=AFMS.getMotor(2);

int maxFloorBrightness = 900;
int minTapeBrightness = 750;

int leftSensor;
int rightSensor;

int maxSpeed=32;

int leftSpeed;
int rightSpeed;

int leftError;
int rightError;

int oldLeftError;
int oldRightError;

void setup() {
  Serial.begin(9600);  // initialize serial port
  AFMS.begin();  // initialize motor shield

  // set motor direction
  left->run(FORWARD);
  right->run(FORWARD);
}

void loop() {
  leftSensor = 1024 - analogRead(A0);
  rightSensor = 1024 - analogRead(A1);

  // PRECALCULATIONS
  leftError = constrain(maxFloorBrightness-leftSensor, 0, maxFloorBrightness);
  rightError = constrain(maxFloorBrightness-rightSensor, 0,  maxFloorBrightness);

  // LEFT/RIGHT CONTROL
  leftSpeed = maxSpeed - map(leftError, 0, maxFloorBrightness - minTapeBrightness, 0, maxSpeed);
  rightSpeed = maxSpeed - map(rightError, 0, maxFloorBrightness - minTapeBrightness, 0, maxSpeed);

  // prevent stopping when both sensors see tape on sharp turns
  if (leftSpeed == 0 && rightSpeed == 0) {
    leftSpeed = 5;
    rightSpeed = 5;
  }

  // set motor speeds
  left->setSpeed(leftSpeed);
  right->setSpeed(rightSpeed);

  oldLeftError=leftError;
  oldRightError=rightError;

  Serial.print(analogRead(A0));
  Serial.print("\t");
  Serial.print(analogRead(A1));
  Serial.print("\t");
  Serial.print(leftSensor);
  Serial.print("\t");
  Serial.print(rightSensor);
  Serial.print("\t");
  Serial.print(leftError);
  Serial.print("\t");
  Serial.println(rightError);
}
