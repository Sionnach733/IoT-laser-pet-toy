#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h >

Servo tiltServo, panServo, treatServo; 
int tiltReading = 90;
int panReading = 90;
int treatReading = 47; 
int autoMode = 0;

// min and max ranges for the servos, this is to prevent laser going onto the walls, adjust as needed
int minPan = 32;
int maxPan = 120;
int minTilt = 101;
int maxTilt = 123;

// Blynk auth token and wifi credentials, update these
char auth[] = "****";
char ssid[] = "****";
char pass[] = "****";

BlynkTimer timer; 
int manualDelay = 0;
int autoDelay = 0;
int treatDelay = 0;
int autoTimerInt = 0;
int manualTimerInt = 0;
int treatTimerInt = 0;

void setServoAuto(){
  tiltServo.write(random(minTilt, maxTilt));
  autoDelay = timer.setTimeout(100L, setAutoPan);
  timer.enable(autoDelay);
}

void setAutoPan(){
  panServo.write(random(minPan, maxPan));
}

void setAutoTreat(){
  treatServo.write(180);
}

void setServoManual(){
  tiltServo.write(tiltReading);
  manualDelay = timer.setTimeout(100L, setManualPan);
  timer.enable(manualDelay);
}

void setManualPan(){
  panServo.write(panReading);
  treatDelay = timer.setTimeout(100L, setManualTreat);
  timer.enable(treatDelay);
}

void setManualTreat(){
  treatServo.write(treatReading);
}

void setup(){
  tiltServo.attach(2);//d4 pin on nodeMCU
  panServo.attach(4);//d2 pin on nodeMCU
  treatServo.attach(12);//d6 pin on nodeMCU
  Blynk.begin(auth, ssid, pass);
  //start manual mode
  manualTimerInt = timer.setInterval(1000L, setServoManual);
  timer.enable(manualTimerInt);
}

BLYNK_WRITE (V0){
  tiltReading = param.asInt();
}

BLYNK_WRITE (V1){
  panReading = param.asInt();
}

BLYNK_WRITE (V2){
  autoMode = param.asInt();
  timer.disableAll();
  if(autoMode == 1){
    //pan/tilt servos will move randomly every 2 seconds, 10 times. adjust these values as you see fit
    autoTimerInt = timer.setTimer(2000L, setServoAuto, 10);
    timer.enable(autoTimerInt);
    //treat is dropped after laser is finished, adjust as you see fit
    treatTimerInt = timer.setTimeout(22000L, setAutoTreat);
    timer.enable(treatTimerInt);
  }else{
    manualTimerInt = timer.setInterval(1000L, setServoManual);
    timer.enable(manualTimerInt);
  }
}

BLYNK_WRITE (V3){
  treatReading = param.asInt();
}

void loop(){
  Blynk.run();
  timer.run();
}
