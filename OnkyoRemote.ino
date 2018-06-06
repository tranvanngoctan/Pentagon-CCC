// Include IR Remote Library by Ken Shirriff
 
#include <IRremote.h>

// Create IR Send Object
IRsend irsend;

int proximity;
int previousProximity;
int khz = 38;
int debounceTime = 100;

// Define switch pin
const int piezo = 12;
const int reed = 8;   //playPause
const int volumeUp = 7;
const int volumeDown = 6;
const int nextTrack = 5;
const int previousTrack = 4;

unsigned int playPauseSignals[] = {8900,4500, 550,550, 550,1700, 550,550, 550,600, 550,1700, 550,550, 550,1700, 550,1650, 550,600, 550,1700, 550,550, 550,1700, 550,1700, 500,600, 550,550, 550,600, 550,550, 550,550, 550,1700, 550,550, 600,550, 550,550, 550,1700, 550,550, 550,1700, 550,1700, 550,550, 550,1700, 550,1700, 550,1700, 550,550, 550,1700, 550};
unsigned int volumeUpSignals[] = {8900,4450, 550,600, 550,1650, 550,600, 550,550, 550,1700, 550,550, 550,1700, 550,1700, 550,1650, 550,1700, 550,600, 550,550, 550,550, 550,600, 550,550, 550,600, 550,550, 550,1700, 550,550, 550,550, 550,600, 550,550, 550,600, 550,550, 550,1700, 550,550, 550,1700, 550,1700, 550,1650, 550,1700, 550,1700, 550,1700, 500};
unsigned int volumeDownSignals[] = {8900,4500, 550,550, 500,1750, 550,550, 550,600, 500,1700, 550,600, 500,1700, 550,1700, 550,1700, 500,1750, 500,600, 500,600, 550,600, 500,600, 550,600, 500,600, 500,1750, 500,1700, 550,600, 500,600, 550,600, 500,600, 500,600, 550,600, 550,550, 500,600, 550,1700, 550,1700, 500,1750, 500,1700, 550,1700, 550,1700, 500};
unsigned int nextTrackSignals[] = {8850,4500, 550,600, 500,1700, 550,600, 500,600, 550,1700, 500,600, 550,1700, 500,1750, 500,1750, 500,1700, 550,600, 550,550, 500,600, 550,600, 500,600, 550,600, 500,1700, 550,600, 550,1650, 550,1700, 550,1700, 500,600, 550,600, 500,600, 550,550, 550,1700, 550,600, 500,600, 500,600, 550,1700, 550,1700, 550,1700, 500};
unsigned int previousTrackSignals[] = {8900,4500, 550,550, 550,1700, 550,550, 550,600, 550,1650, 550,600, 550,1650, 550,1700, 550,1700, 550,1700, 550,550, 550,550, 550,600, 550,550, 550,600, 550,550, 550,550, 550,1700, 550,1700, 550,1700, 550,1650, 550,600, 550,550, 550,550, 600,1650, 550,600, 550,550, 550,550, 550,600, 550,1700, 550,1650, 550,1700, 550};
unsigned int powerSignals[] = {8900,4500, 550,550, 550,1700, 550,550, 550,600, 500,1700, 600,550, 550,1650, 550,1700, 550,600, 500,600, 550,1700, 550,550, 550,550, 550,600, 550,550, 550,600, 500,1700, 550,1700, 550,550, 550,1700, 550,550, 550,600, 550,1700, 550,1650, 550,600, 550,550, 550,1700, 550,550, 550,1700, 550,1700, 550,550, 550,550, 550};

unsigned long currentMillis;
unsigned long pauseMillis = 0;
boolean isOff;

void setup() {
  Serial.begin(9600);
  pinMode(reed, INPUT_PULLUP);
  pinMode(volumeUp, INPUT);
  pinMode(volumeDown, INPUT);
  pinMode(nextTrack, INPUT);
  pinMode(previousTrack, INPUT);
}

void simpleSound() {
  tone(piezo, 500);
  delay(50);
  noTone(piezo);
  delay(50);
}

int tones[] = {261, 277, 294, 311, 330};//, 349, 370, 392, 415, 440};

void powerSound() {
  for (int i = 0; i < 5; i++)
  {
    tone(piezo, tones[i]);
    delay(150);
  }
  noTone(piezo);
}

void loop() {
  // Cover is open:   proximity = true
  // Cover is closed: proximity = false
  proximity = digitalRead(reed);
  
  if (proximity != previousProximity) {
    // Close the cover
    if (!proximity) {
      // Pause
      irsend.sendNEC(0x4B5822DD, 32);
      Serial.println("Pause");
      simpleSound();
      pauseMillis = millis(); // The moment user closes the cover
    }

    // Open the cover
    if (proximity) {
      // Turn on the CD-player first if it is off
      if (isOff) {
        irsend.sendNEC(0x4B20D32C, 32);
        isOff = false;
        Serial.println("Turn on");
        powerSound();
        delay(500);
      }
      // Play
      irsend.sendNEC(0x4B5822DD, 32);
      Serial.println("Play");
      simpleSound();
    }
    
    delay(debounceTime);
  }
  
  if (digitalRead(volumeUp)) {
    irsend.sendNEC(0x4BC040BF, 32);
    Serial.println("Volume up");
    simpleSound();
    delay(debounceTime);
  }

  if (digitalRead(volumeDown)) {
    irsend.sendNEC(0x4BC0C03F, 32);
    Serial.println("Volume down");
    simpleSound();
    delay(debounceTime);
  }
  
  if (digitalRead(nextTrack)) {
    irsend.sendNEC(0x4BC0B847, 32);
    Serial.println("Next track");
    simpleSound();
    delay(debounceTime);
  }

  if (digitalRead(previousTrack)) {
    irsend.sendNEC(0x4BC07887, 32);
    Serial.println("Previous track");
    simpleSound();
    delay(debounceTime);
  }

  if ((millis()-pauseMillis) > 10000 && (millis()-pauseMillis) < 10250 && pauseMillis != 0) {
    irsend.sendNEC(0x4B20D32C, 32);
    Serial.println("Turn off");
    powerSound();
    isOff = true;
    delay(debounceTime);
  }
  
  previousProximity = digitalRead(reed);
  delay(200);
}
