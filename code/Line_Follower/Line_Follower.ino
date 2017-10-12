#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *left=AFMS.getMotor(1);
Adafruit_DCMotor *right=AFMS.getMotor(2);

// #define DEBUG  // print sensor and motor data to serial
#define SERIAL_INPUT  // rudimentary serial control

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

  #ifdef SERIAL_INPUT
    Serial.println("Available commands:\n\ts<INT>\tset maxSpeed\n\tf<INT>\tset maxFloorBrightness\n\tt<INT>\tset minTapeBrightness\n\tg\tget all values");
    Serial.println("\nReady for command:");
  #endif

  // set motor direction
  left->run(FORWARD);
  right->run(FORWARD);
}

void loop() {
  // Serial Input
  #ifdef SERIAL_INPUT
    if (Serial.available() > 0) {
      switch (Serial.peek()) {
        case 's':
          Serial.read();  // clear first char
          maxSpeed = Serial.parseInt();
          Serial.print("Set maxSpeed to ");
          Serial.println(maxSpeed);
          break;
        case 'f':
          Serial.read();
          maxFloorBrightness = Serial.parseInt();
          Serial.print("Set maxFloorBrightness to ");
          Serial.println(maxFloorBrightness);
          break;
        case 't':
          Serial.read();
          minTapeBrightness = Serial.parseInt();
          Serial.print("Set minTapeBrightness to ");
          Serial.println(minTapeBrightness);
          break;
        case 'g':
          Serial.read();
          Serial.println("Current values:");
          Serial.print("maxSpeed\t");
          Serial.println(maxSpeed);
          Serial.print("maxFloorBrightness\t");
          Serial.println(maxFloorBrightness);
          Serial.print("minTapeBrightness\t");
          Serial.println(minTapeBrightness);
          break;
        default:
          Serial.println("Unknown command");
          while(Serial.available()) {  // clear input buffer
            Serial.read();
            delay(10);
          }
          break;
      }
      Serial.println("\nReady for command:");
    }
  #endif

  leftSensor = 1024 - analogRead(A0);
  rightSensor = 1024 - analogRead(A1);

  // PRECALCULATIONS
  leftError = constrain(maxFloorBrightness-leftSensor, 0, maxFloorBrightness);
  rightError = constrain(maxFloorBrightness-rightSensor, 0,  maxFloorBrightness);

  // LEFT/RIGHT CONTROL
  leftSpeed = maxSpeed - map(leftError, 0, maxFloorBrightness, 0, maxSpeed);
  rightSpeed = maxSpeed - map(rightError, 0, maxFloorBrightness, 0, maxSpeed);

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

  #ifdef DEBUG
    Serial.print(leftSensor);
    Serial.print("\t");
    Serial.print(rightSensor);
    Serial.print("\t");
    Serial.print(leftSpeed);
    Serial.print("\t");
    Serial.println(rightSpeed);
  #endif
}
