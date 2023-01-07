#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define MUNDAUF_PIN 6
#define MUNDZU_PIN 7
#define FLOSSE_PIN 8
#define KOPF_PIN 9

byte playerBootingTime = 100;
SoftwareSerial mySoftwareSerial(10,11);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  // put your setup code here, to run once:
  pinMode(MUNDAUF_PIN, OUTPUT);
  digitalWrite(MUNDAUF_PIN, LOW);
  pinMode(MUNDZU_PIN, OUTPUT);
  digitalWrite(MUNDZU_PIN, LOW);
  pinMode(FLOSSE_PIN, OUTPUT);
  digitalWrite(FLOSSE_PIN, LOW);
  pinMode(KOPF_PIN, OUTPUT);
  digitalWrite(KOPF_PIN, LOW);

  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  delay(playerBootingTime);
  if (!myDFPlayer.begin(mySoftwareSerial)) 
  {  
    Serial.println(F("Could not boot DFPlayer Mini. Stopped setup"));
    while(true);
  }
  
  setupPlayer();
  Serial.println(F("DFPlayer Mini setup done."));
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(1000);
  digitalWrite(KOPF_PIN, HIGH);
  delay(500);
  digitalWrite(MUNDAUF_PIN, HIGH);
  myDFPlayer.play(1);
  delay(790);
  digitalWrite(MUNDAUF_PIN, LOW);
  delay(877);
  digitalWrite(MUNDAUF_PIN, HIGH);
  delay(911);
  digitalWrite(MUNDAUF_PIN, LOW);
  delay(716);
  
  digitalWrite(KOPF_PIN, LOW);
  
  delay(5000);
  digitalWrite(FLOSSE_PIN, HIGH);
  delay(1000);
  digitalWrite(FLOSSE_PIN, LOW);

  while(true);
}

void setupPlayer()
{
  myDFPlayer.setTimeOut(500);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.volume(10);
}
