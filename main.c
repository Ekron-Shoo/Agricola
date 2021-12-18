#include <Tle493d_w2b6.h>
#include <Wire.h>
#include <I2C_Anything.h>
int WHEEL_DIAMETER = 1; //units here will determine units for distance
boolean hasBeenCounted;//prevents a large number of false positive revolutions in case the wheel stays at the same position
boolean hasBeenHalfway;//minimizes the amount of false positives if the wheel moves back accidentaly
int numOfRots;//number of times the wheel completed a full revolution
int rpm;//rotations per minute
int lastAngle;//last recorded angle in radians before measurement termination 
int lastPart;//part that didn't count as a full rotation in the end
float disTrav;//distance in units of wheel's diameter
boolean isRecording = true;//user input
boolean isStopped = false;//user input
boolean isForwards;//checks to see if the wheel is going forwards and not backwards
Tle493d_w2b6 TSensor = Tle493d_w2b6();
void setup() {
 Serial.begin(9600);
 //starts sensor
 TSensor.begin();
 TSensor.setWakeUpThreshold(1,-1,1,-1,1,-1);
 TSensor.disableTemp();
 hasBeenCounted = false;
 hasBeenHalfway = true;
 Wire.begin(21);
}
void loop() {
   Wire.beginTransmission (21);
   I2C_writeAnything (disTrav);
   Wire.endTransmission ();
 TSensor.updateData();
   if((TSensor.getAzimuth() > 0)&&(!hasBeenCounted)&&(hasBeenHalfway)){
     TSensor.updateData();
     numOfRots++;
     hasBeenCounted = true;
     hasBeenHalfway = false;
     if(TSensor.getAzimuth() > 2){
       isForwards = false;
     }else{
       isForwards = true;
     }
     Serial.println(numOfRots);
     Serial.println(TSensor.getAzimuth());
   }
   if(TSensor.getAzimuth() < 0){
     hasBeenCounted = false;
   }
   if((TSensor.getAzimuth() < 2)&&(TSensor.getAzimuth() > 0)){
     hasBeenHalfway = true;
   }
     //records current position to calculate remaining distance travelled
     //in case it didn't make a full rotation
     lastAngle = TSensor.getAzimuth();
     //this if statement prevents the last rotation from being counted twice
     //in case the user stops right on the rotation boundary
     if((lastAngle == PI)||(lastAngle == -PI)){
       //the direction is important to calculate how much of the wheel has spun
       //going forwards or backwards could be the difference between having spun
       //1/4th of the way versus 3/4ths of the way, a lot on a large wheel
       if(isForwards){
         lastPart = (lastAngle+PI)/(2*PI);
       }else{
         lastPart = ((2*PI)-(lastAngle+PI)/(2*PI));
       }
     }
     //calculates distance travelled in units of wheel's diameter
     disTrav = ((numOfRots+lastPart) * (PI * WHEEL_DIAMETER));
}
