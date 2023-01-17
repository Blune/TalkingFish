#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#define ALARM_PIN A1
#define LICHT_PIN A2
#define KLOPF_PIN A3
#define BUTTON_PIN 5
#define MUNDAUF_PIN 6
#define MUNDZU_PIN 7
#define FLOSSE_PIN 8
#define KOPF_PIN 9
#define ALARM_SOUND 1
#define TOUCH_SOUND 4
#define LIGHT_SOUND 5
#define BUTTON_SOUND 6
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
  //Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
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
  //Debug Info:
  Serial.print("Piezo sagt: "); // analogRead(KLOPF_PIN
  Serial.println(analogRead(KLOPF_PIN));
  Serial.print("Photowiderstand sagt: "); // + analogRead(LICHT_PIN
  Serial.println(analogRead(LICHT_PIN));
  Serial.println("");
  if(!digitalRead(BUTTON_PIN)){
    sayHilfe();
    delay(2000);
  }  
  else if(digitalRead(ALARM_PIN)){
    sayAlarm();
    delay(2000);
  }
  else if(analogRead(KLOPF_PIN) < 506 || analogRead(KLOPF_PIN) > 522){
    sayTouch();
    delay(2000);
  }
  
  else if(analogRead(LICHT_PIN)< 400){
    sayProblem();
    delay(2000);
  }  
}
void wackelFlosse(){
  digitalWrite(FLOSSE_PIN, HIGH);
  delay(500);
  digitalWrite(FLOSSE_PIN, LOW);
}
void sayWord(int duration) {
  digitalWrite(MUNDAUF_PIN, HIGH);
  delay(duration);
  digitalWrite(MUNDAUF_PIN, LOW);
}
void raiseHead(){
  digitalWrite(KOPF_PIN, HIGH);
  delay(500);
}
void lowerHead(){
  digitalWrite(KOPF_PIN, LOW);
}
void sayAlarm(){
  raiseHead();
  myDFPlayer.play(ALARM_SOUND);
  sayWord(790);
  delay(877);
  sayWord(911);
  delay(716);
  lowerHead();
}
void sayProblem(){
  raiseHead();
  myDFPlayer.play(LIGHT_SOUND);
  delay(60);
  sayWord(88); //HAST
  delay(70);
  sayWord(58); //DUN
  delay(32);
  sayWord(97); //PRO
  delay(70);
  sayWord(180); //BLEM
  delay(250);
  sayWord(66); //GEH
  delay(47);
  sayWord(120); //WEIT
  delay(40);
  sayWord(120); //ER
  lowerHead();
  wackelFlosse();
}
void sayTouch() {
  raiseHead();
  myDFPlayer.play(TOUCH_SOUND);
  sayWord(160); //CANT
  delay(116);
  sayWord(140); //TOUCH
  delay(120);
  sayWord(130); //THIS
  lowerHead();
  wackelFlosse();
}
void sayHilfe(){
  raiseHead();
  myDFPlayer.play(BUTTON_SOUND);
  delay(25);
  sayWord(56); //BIT
  delay(47);
  sayWord(84); //TE
  delay(70);
  sayWord(84); //HELFEN
  delay(197);
  sayWord(72); //SIE
  delay(75);
  sayWord(184); //MIRICH
  delay(122);
  sayWord(52); //BININ
  delay(70);
  sayWord(58); //GEFAHR
  delay(141);
  sayWord(52); 
  delay(100);
  sayWord(172); 
  delay(194);
  sayWord(46); 
  delay(47);
  sayWord(240);
  delay(207);
  sayWord(61); 
  delay(67);
  sayWord(120); 
  delay(50);
  lowerHead();
}
void setupPlayer()
{
  myDFPlayer.setTimeOut(500);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.volume(10);
}
