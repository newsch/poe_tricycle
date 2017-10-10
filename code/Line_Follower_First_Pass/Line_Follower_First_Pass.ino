#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *left=AFMS.getMotor(1);
Adafruit_DCMotor *right=AFMS.getMotor(2);

int floorBrightness = 900;

int leftSensor;
int rightSensor;

int fullSpeed=128;

int leftError;
int rightError;

int cumulativeError=0; //negative is left, positive is right
int derivative;

float proportionalWeight=1/8;
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
  // put your main code here, to run repeatedly:
  leftSensor=1024-analogRead(A0);
  rightSensor=1024-analogRead(A1);

  Serial.print(leftSensor);
  Serial.print("\t");
  Serial.println(rightSensor);

  //RIGHT PRECALCULATION
  if(rightSensor>floorBrightness){
    rightError=0;
  }
  else{
    rightError=floorBrightness-rightSensor;
  }

  //LEFT CONTROL
  if(leftSensor>floorBrightness){
    left->setSpeed(fullSpeed);
    leftError=0;
  }
  else{
    leftError=floorBrightness-leftSensor;
    cumulativeError=(-1)*leftError+rightError; //calculates the reimann sum term. To be used in both left and right control
    derivative=leftError-oldLeftError;
    left->setSpeed(fullSpeed-(proportionalWeight*leftError+integralWeight*cumulativeError+derivativeWeight*derivative));
  }
  oldLeftError=leftError;

  //RIGHT CONTORL
  if(rightSensor>floorBrightness){
    right->setSpeed(fullSpeed);
    rightError=0;
  }
  else{
    rightError=floorBrightness=rightSensor;
    derivative=leftError-oldLeftError;
    right->setSpeed(fullSpeed-(proportionalWeight*leftError+integralWeight*cumulativeError+derivativeWeight*derivative));
  }
  rightError=oldRightError;



}
