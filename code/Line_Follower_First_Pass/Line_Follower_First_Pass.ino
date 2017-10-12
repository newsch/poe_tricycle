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

int fullSpeed=32;

int leftError;
int rightError;

int leftSpeed;
int rightSpeed;

int cumulativeError=0; //negative is left, positive is right
int leftDerivative;
int rightDerivative;

float proportionalWeight=1/8.0;
float integralWeight=0.001;
float derivativeWeight=0.001;

int oldLeftError;
int oldRightError;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  AFMS.begin();

  left->run(FORWARD);
  right->run(FORWARD);
}

void loop() {
  // TODO: offset sensor by 300 so black tape reads as 0
  leftSensor = constrain(1024 - analogRead(A0), minTapeBrightness, maxFloorBrightness);
  rightSensor = constrain(1024 - analogRead(A1), minTapeBrightness, maxFloorBrightness);

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

  //LEFT PRECALCULATION
  if(leftSensor>maxFloorBrightness){
    leftError=0;
  }
  else{
    leftError = constrain(maxFloorBrightness-leftSensor, 0, maxFloorBrightness);
  }


  //RIGHT PRECALCULATION
  if(rightSensor>maxFloorBrightness){
    rightError=0;
  }
  else{
    rightError = constrain(maxFloorBrightness-rightSensor, 0,  maxFloorBrightness);
  }

  //CALCULATING THE SHARED INTEGRAL TERM
  cumulativeError=(-1)*leftError+rightError; //calculates the reimann sum term. To be used in both left and right control

  //LEFT/RIGHT CONTROL
  leftDerivative=leftError-oldLeftError;
  rightDerivative=rightError-oldRightError;
  // left->setSpeed(fullSpeed-(proportionalWeight*leftError+integralWeight*cumulativeError+derivativeWeight*leftDerivative));
  // right->setSpeed(fullSpeed-(proportionalWeight*rightError+integralWeight*cumulativeError+derivativeWeight*rightDerivative));
  leftSpeed = fullSpeed - map(leftError, 0, maxFloorBrightness - minTapeBrightness, 0, fullSpeed);
  rightSpeed = fullSpeed - map(rightError, 0, maxFloorBrightness - minTapeBrightness, 0, fullSpeed);

  if (leftSpeed == 0 && rightSpeed == 0) {
    leftSpeed == 5;
    rightSpeed == 5;
  }

  left->setSpeed(leftSpeed);
  right->setSpeed(rightSpeed);

  // Serial.print(leftError);
  // Serial.print("\t");
  // Serial.print(rightError);
  // Serial.print("\t");
  // Serial.print(leftSpeed);
  // Serial.print("\t");
  // Serial.println(rightSpeed);

  // Serial.print(proportionalWeight);
  // Serial.print("\t");
  // Serial.print(leftError);
  // Serial.print("\t");
  // Serial.println(proportionalWeight * leftError);

  oldLeftError=leftError;
  oldRightError=rightError;
}
