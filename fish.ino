#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

//PINS
//analog
#define SOUND_PIN A1
#define LICHT_PIN A2
#define KLOPF_PIN A3
//digital
#define LOW_PIN 2
#define SWITCH_PIN 3
#define ALARM_PIN 4
#define BUTTON_PIN 5
#define MUNDAUF_PIN 6
#define MUNDZU_PIN 7
#define FLOSSE_PIN 8
#define KOPF_PIN 9

//SOUNDS
#define ALARM_SOUND 1
#define TOUCH_SOUND 4
#define LIGHT_SOUND 5
#define BUTTON_SOUND 6
#define ZIEGE_SOUND 7
#define KINSKI_SCHNAUZE_SOUND 8
#define KINSKI_ARBEITE_SOUND 9
#define KINSKI_KANNSONICH_SOUND 10
#define KINSKI_WASSOLLDAS_SOUND 11
#define KINSKI_IMWEG_SOUND 12
#define BOOT_SOUND 13

//VARIABLES
unsigned long currentMillis;
unsigned long lastSoundAlarm = 0;
unsigned long lastTailAlarm = 0;
unsigned long alarmSoundPause = 1000UL * 60UL * 15UL; // 15 min interval
unsigned long alarmTailPause = 1000UL * 30UL; // 30 seconds interval
unsigned long lastAction = 1000; // Sytemzeit (Millis) der letzten Aktion überhaupt
unsigned long last_klatsch = 0;
unsigned long silent_time = 0;
bool loud = false;

bool toggleSound = false;

byte touchSensitivity = 50; // 10 -> sehr empfindlich , 255 -> unempfindlich
unsigned int touchWert = 0;
int klopf_10; // gemessener Klopfwert (10 Bit)
int real_klopf; // Differenz zum Durchschnitt (Absolutwert 10 Bit)
int average_klopf_10 = 0; // durchschnittlticher Klopfwert (10 Bit)
int average_klopf_13 = 0; // durchschnittlticher Klopfwert (13 Bit)

int light_10; // gemessener Lichtwert (10 Bit)
int average_light_10 = 0; // durchschnittlticher Lichtwert (10 Bit)
long average_light_20 = 0; // durchschnittlticher Lichtwert (13 Bit)

byte soundSensitivity = 100; // 1 -> sehr empfindlich , 255 -> unempfindlich
unsigned int soundWert = 0;
int sound_10; // gemessener Soundwert (10 Bit)
int average_sound_10 = 0; // durchschnittlticher Soundwert (10 Bit)
int average_sound_15 = 0; // durchschnittlticher Soundwert (15 Bit)
int volume_10; // Lautstärke (Differenz zum Durchschnittswert, Absolutwert 10 Bit)
int average_volume_10; // Lautstärke (Durchschnitt über 2 Minuten, 10 Bit)
long average_volume_27; // Lautstärke (Durchschnitt über 2 Minuten, 27 Bit)

byte playerBootingTime = 100;
SoftwareSerial mySoftwareSerial(10, 11);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  //Sound: Analog digital Messung beschleunigen
  // ADCSRA = (ADCSRA & 248) | 6; // doch nicht nötig (Geschwindigkeit reicht aus)

  //Motors
  pinMode(MUNDAUF_PIN, OUTPUT);
  digitalWrite(MUNDAUF_PIN, LOW);
  pinMode(MUNDZU_PIN, OUTPUT);
  digitalWrite(MUNDZU_PIN, LOW);
  pinMode(FLOSSE_PIN, OUTPUT);
  digitalWrite(FLOSSE_PIN, LOW);
  pinMode(KOPF_PIN, OUTPUT);
  digitalWrite(KOPF_PIN, LOW);

  //Alarm
  pinMode(ALARM_PIN, INPUT);

  //Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  //Switch
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LOW_PIN, OUTPUT);
  digitalWrite(LOW_PIN, LOW);

  //Player
  mySoftwareSerial.begin(9600);
  Serial.begin(38400);
  delay(playerBootingTime);
  if (!myDFPlayer.begin(mySoftwareSerial))
  {
    Serial.println(F("Could not boot DFPlayer Mini. Stopped setup"));
    while (true);
  }

  setupPlayer();
  Serial.println(F("Setup done!"));

  average_sound_15 = analogRead(SOUND_PIN) << 5; // Durchschnittswert (grob) setzen

  delay(100);
  sayBoot();
}

void loop() {
  //Debug Info:
  //
  //Serial.print("Piezo sagt: ");
  //Serial.println(analogRead(KLOPF_PIN));

  //Serial.print("Sound sagt: ");
  //Serial.println(analogRead(SOUND_PIN));

  //Serial.print("Licht: ");
  //Serial.println(analogRead(LICHT_PIN));

  //Serial.print("AlarmPin sagt: ");
  //Serial.println(digitalRead(ALARM_PIN));

  //Serial.println("");

  //Serial.print("K: ");
  //Serial.println(klopf_10);


  //Serial.print("piezo 10 sagt: ");
  //Serial.println(piezo_10);

  //Serial.print("touchWert sagt: ");
  //Serial.println(touchWert);

  if (!digitalRead(BUTTON_PIN)) {
    sayHilfe();
    delay(2000);
    lastAction = millis();
  }

  klopf_10 = analogRead(KLOPF_PIN);
  average_klopf_10 = (average_klopf_13 + 4) >> 3;
  average_klopf_13 += klopf_10 - average_klopf_10;
  real_klopf = abs(klopf_10 - average_klopf_10);
  touchWert += real_klopf;
  if (touchWert) touchWert --;

  if (millis() < lastAction + 1000)
  {
    touchWert = 0;
  }
  else if ((touchWert >> 2) > touchSensitivity) {
    if (getSwitchMode())
    {
      sayWasSollDas();
    }
    else
    {
      sayTouch();
    }
    Serial.print("klopf 10 sagt: ");
    Serial.println(klopf_10);
    Serial.print("real_klopf sagt: ");
    Serial.println(real_klopf);
    Serial.print("average_klopf_10 sagt: ");
    Serial.println(average_klopf_10);
    Serial.print("touchWert sagt: ");
    Serial.println(touchWert);
    Serial.println("");
    touchWert = 0;
    delay(1000);
    lastAction = millis();
  }

  light_10 = analogRead(LICHT_PIN);
  average_light_10 = (average_light_20 + 512) >> 10;
  average_light_20 += light_10 - average_light_10;
  if (light_10 + 5 < average_light_10 - (average_light_10 >> 3)) // empfindlich: >> 3
    //if(light_10 + 5 < average_light_10 - (average_light_10 >> 2)) // unempfindlich: >> 2
  {
    if (getSwitchMode())
    {
      sayImWeg();
    }
    else
    {
      sayProblem();
    }
    Serial.print("light 10 sagt: ");
    Serial.println(light_10);
    Serial.print("average_light_10 sagt: ");
    Serial.println(average_light_10);
    Serial.println("");
    delay(1000);
    lastAction = millis();
    average_light_20 = 0; // Normalwert zurücksetzen (neue Licht-Anpassung)
  }
  else if (digitalRead(ALARM_PIN)) {
    currentMillis = millis();
    if (lastSoundAlarm == 0 || currentMillis - lastSoundAlarm > alarmSoundPause) {
      lastSoundAlarm = currentMillis;
      sayAlarm();
      delay(1000);
      lastAction = millis();
    }
    else if (currentMillis - lastTailAlarm > alarmTailPause) {
      lastTailAlarm = currentMillis;
      wackelFlosse();
      delay(1000);
      lastAction = millis();
    }
  }

  sound_10 = analogRead(SOUND_PIN);
  average_sound_10 = (average_sound_15 + 16) >> 5;
  average_sound_15 += sound_10 - average_sound_10;
  volume_10 = abs(sound_10 - average_sound_10);

  //Klatschen
  if (volume_10 > 4)
  {
    if (!loud)
    {
      if (millis() - last_klatsch < 900)
      {
        if (millis() - silent_time > 100)
        {
          sayZiege();
          lastAction = millis();
          last_klatsch = 0;
        }
      }
      else
      {
        last_klatsch = millis();
      }
      loud = true;
    }
  }
  else
  {
    if (volume_10 < 3)
    {
      if (loud)
      {
        silent_time = millis();
        loud = false;
      }
    }
    else
    {
      silent_time = millis();
    }
  }


  if (volume_10)
  {
    if (volume_10 == 1)
    {
      volume_10 = 0;
    }
    else
    {
      //volume_10 = sqrt((volume_10 << 2) - 4.1);
      volume_10 = pow (((volume_10 << 3) - 6), 0.3);
    }
  }

  average_volume_10 = (average_volume_27 + 65536) >> 17; // ca. 1000 Messungen pro Sekunde >> 17 entspricht ca. 2 Minuten (Tau-Wert)
  average_volume_27 += volume_10 - average_volume_10;
  if ((average_volume_27 >> 7) > soundSensitivity)
  {
    if (getSwitchMode())
    {
      //sayWasSollDas();
      sayKannDochSoNich();
    }
    else
    {
      sayArbeite();
    }
    Serial.print("average_sound_10: ");
    Serial.println(average_sound_10);
    Serial.print("volume_10: ");
    Serial.println(volume_10);
    Serial.print("average_volume_20: ");
    Serial.println(average_volume_27 >> 7);
    Serial.println("");
    delay(1000);
    lastAction = millis();
    average_volume_27 = 0; // langfristig gemessene Lautstärke zurücksetzen
  }
}

int getSwitchMode() {
  toggleSound = !toggleSound;
  return toggleSound;
}

int getAdvancedMode() {  
  return digitalRead(SWITCH_PIN);
}

void wackelFlosse() {
  digitalWrite(FLOSSE_PIN, HIGH);
  delay(500);
  digitalWrite(FLOSSE_PIN, LOW);
}

void sayWord(int duration) {
  digitalWrite(MUNDAUF_PIN, HIGH);
  delay(duration);
  digitalWrite(MUNDAUF_PIN, LOW);
}

void raiseHead() {
  digitalWrite(KOPF_PIN, HIGH);
  delay(500);
}

void lowerHead() {
  digitalWrite(KOPF_PIN, LOW);
}

void sayAlarm() {
  raiseHead();
  myDFPlayer.play(ALARM_SOUND);
  sayWord(790);
  delay(877);
  sayWord(911);
  delay(716);
  lowerHead();
}

void sayProblem() {
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

void sayBoot() {
  raiseHead();
  myDFPlayer.play(BOOT_SOUND);
  delay(3000);
  lowerHead();
  wackelFlosse();
}

void sayZiege() {
  raiseHead();
  myDFPlayer.play(ZIEGE_SOUND);
  delay(75);
  sayWord(1660);
  delay(50);
  lowerHead();
  wackelFlosse();
}

void sayArbeite() {
  raiseHead();
  myDFPlayer.play(KINSKI_ARBEITE_SOUND);
  delay(75);
  sayWord(50);
  delay(69);
  sayWord(200);
  delay(43);
  sayWord(123);
  delay(74);
  sayWord(49);
  delay(25);
  sayWord(170);
  delay(25);
  sayWord(106);
  delay(98);
  sayWord(184);
  delay(95);
  sayWord(190);
  delay(50);
  lowerHead();
  wackelFlosse();
}

void sayKannDochSoNich() {
  raiseHead();
  myDFPlayer.play(KINSKI_KANNSONICH_SOUND);
  delay(168);
  sayWord(232);
  delay(130);
  sayWord(124);
  delay(254);
  sayWord(165);
  delay(230);
  sayWord(126);
  delay(188);
  sayWord(174);
  delay(84);
  sayWord(224);
  delay(50);
  lowerHead();
  wackelFlosse();
}

void sayWasSollDas() {
  raiseHead();
  myDFPlayer.play(KINSKI_WASSOLLDAS_SOUND);
  delay(134);
  sayWord(48);
  delay(240);
  sayWord(182);
  delay(80);
  sayWord(83);
  delay(50);
  lowerHead();
  wackelFlosse();
}
void sayImWeg() {
  raiseHead();
  myDFPlayer.play(KINSKI_IMWEG_SOUND);
  delay(105);
  sayWord(99);
  delay(71);
  sayWord(97);
  delay(99);
  sayWord(63);
  delay(140);
  sayWord(269);
  delay(220);
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



void sayHilfe() {
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
  myDFPlayer.volume(25);
}
