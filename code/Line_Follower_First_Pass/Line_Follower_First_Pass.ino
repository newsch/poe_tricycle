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
int leftDerivative;
int rightDerivative;

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
  Serial.print(rightSensor);
  Serial.print("\t");
  Serial.print(leftError);
  Serial.print("\t");
Serial.println(rightError);

  //LEFT PRECALCULATION
  if(leftSensor>floorBrightness){
    leftError=0;
  }
  else{
    leftError=floorBrightness-leftSensor;
  }


  //RIGHT PRECALCULATION
  if(rightSensor>floorBrightness){
    rightError=0;
  }
  else{
    rightError=floorBrightness-rightSensor;
  }

  //CALCULATING THE SHARED INTEGRAL TERM
  cumulativeError=(-1)*leftError+rightError; //calculates the reimann sum term. To be used in both left and right control

  //LEFT CONTROL
  leftDerivative=leftError-oldLeftError;
  left->setSpeed(fullSpeed-(proportionalWeight*leftError+integralWeight*cumulativeError+derivativeWeight*leftDerivative));

  //RIGHT CONTROL
  rightDerivative=rightError-oldRightError;
  right->setSpeed(fullSpeed-(proportionalWeight*rightError+integralWeight*cumulativeError+derivativeWeight*rightDerivative));


  oldLeftError=leftError;
  oldRightError=rightError;
}
