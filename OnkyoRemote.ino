// Include IR Remote Library by Ken Shirriff
 
#include <IRremote.h>

// Create IR Send Object
IRsend irsend;

int proximity;
int previousProximity;
int debounceTime = 200;
int turnOffTime = 10; // in seconds

// Define switch pin
const int piezo = 12;
const int reed = 8;   //playPause
const int volumeUp = 7;
const int volumeDown = 6;
const int nextTrack = 5;
const int previousTrack = 4;

unsigned long currentMillis;
unsigned long pauseMillis = 0;
boolean isOff;
int previousNextTrack = LOW;
int previousPreviousTrack = LOW;

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
      isOff = false;
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
        delay(5000);
      }
      // Play
      irsend.sendNEC(0x4B5822DD, 32);
      isOff = false; 
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
  
  if (digitalRead(nextTrack) && digitalRead(nextTrack) != previousNextTrack) {
    irsend.sendNEC(0x4BC0B847, 32);
    Serial.println("Next track");
    simpleSound();
    delay(debounceTime);
  }

  if (digitalRead(previousTrack) && digitalRead(previousTrack) != previousPreviousTrack) {
    irsend.sendNEC(0x4BC07887, 32);
    Serial.println("Previous track");
    simpleSound();
    delay(debounceTime);
  }

  if ((millis()-pauseMillis) > (1000 * turnOffTime) && (millis()-pauseMillis) < (1000 * turnOffTime + 250) && pauseMillis != 0 && !proximity && !previousProximity) {
    irsend.sendNEC(0x4B20D32C, 32);
    Serial.println("Turn off");
    powerSound();
    isOff = true;
    delay(debounceTime);
  }
  
  previousProximity = digitalRead(reed);
  previousNextTrack = digitalRead(nextTrack);
  previousPreviousTrack = digitalRead(previousTrack);
  delay(200);
}
