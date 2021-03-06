#include <LedControl.h>

//Pin Mappings (final)
// 0 A9 (9) - Note1 (C)
// 1 A8 (8) - Note2 (Cs)
// 2 A7 (7) - Note3 (D)
// 3 A6 (6) - Note4 (Ds)
// 4 A5 (5) - Note5 (E)
// 5 A4 (4) - Note6 (F)
// 6 A3 (3) - Note7 (Fs)
// 7 A2 (2) - Note8 (G)
// 8 A1 (1) - Note9 (Gs)
// 9 A0 (0) - Note10 (A)
// 10 A22 (67) - Note11 (As)
// 11 A21 (66) - Note12 (B)
// 12 A20 (39) - Note13 (C)
// 13 A19 (38) - Octave Down
// 14 A18 (37) - Middle C
// 15 A17 (36) - Octave Up
// 16 A16 (35) - preset 1
// 17 A15 (34) - preset 2
// 18 A14 (33) - preset 3
// 19 A13 (32) - Toggle Vibrato
// 20 A12 (31) - Toggle Octave

// D0 - display
// D1 - display
// D2 - display
// D3 - Note1 (C)
// D4 - Note2 (Cs)
// D5 - Note3 (D)
// D6 - Note4 (Ds)
// D7 - Note5 (E)
// D8 - Note6 (F)
// D9 - Note7 (Fs)
// D10 - Note8 (G)
// D11 - Note9 (Gs)
// D12 - Note10 (A)
// D24 - Note11 (As)
// D25 - Note12 (B)
// D26 - Note13 (C)

// A10 (64)
// A11 (65)
// A12 (31)
// A13 (32)
// A14 (33)
// A15 (34)
// A16 (35)
// A17 (36)
// A18 (37)
// A19 (38)

int thresh = 200;
int pinNum[21] = {9,8,7,6,5,4,3,2,1,0,67,66,39,38,37,36,35,34,33,32,31};
//int pinNum[18] = {9,8,7,6,5,4,3,2,1,0,67,66,39,38,37,36,35,34,32,31};
int dPinNum[13] = {3,4,5,6,7,8,9,10,11,12,24,25,26};
unsigned char CKeyNote[13] = {60,61,62,63,64,65,66,67,68,69,70,71,72};
bool NoteOn[86];
int NoteVar[3][3] = 
{
  {-13,-12,-11},
  {-1,0,1},
  {11,12,13}
};

//     | flat | natu | shar
// dow |  -13 |  -12 |  -11 
// 0   |  -1  |   0  |  1
// up  |  11  |  12  |  13
int octArr = 0;
int accArr = 0;

const int sharpThresh = 700;
const int flatThresh = 350;
bool flat = false;
bool sharp = false;
bool octDown = false;
bool octUp = false;
bool pre1 = true;
bool pre2 = false;
bool pre3 = false;
int octaveShift = 0;
unsigned long lastOctaveDown;
unsigned long lastOctaveUp;

LedControl left = LedControl(0, 2, 1, 1);
LedControl right = LedControl(3, 5, 4, 1);

void setup() {
  // put your setup code her=e, to run once:
  Serial.begin(9600);
  left.shutdown(0, false);  // turns on display
  left.setIntensity(0, 15); // 15 = brightest
  left.clearDisplay(0);
  right.shutdown(0, false);  // turns on display
  right.setIntensity(0, 15); // 15 = brightest
  right.clearDisplay(0);
  for (int i = 0; i < 13; i++) {
    pinMode(dPinNum[i], OUTPUT);
    digitalWrite(dPinNum[i], LOW);
  }
}

void loop() {
  int volt;
  int note = 0;

  double vol = 0;
  
  for (int i = 0; i < 21; i++) {
    volt = analogRead(pinNum[i]);
    if (i == 19) { //toggle vibrato
        if (volt <= flatThresh) {
          Serial.println("flat");
          sharp = false;
          flat = true;
          accArr = 0;
        }
        else if (volt >= sharpThresh) {
          Serial.println("sharp");
          sharp = true;
          flat = false;
          accArr = 2;
        }
        else {
          sharp = false;
          flat = false;
          accArr = 1;
        }
    }
    else if (i == 20) { //toggle octave
        if (volt <= flatThresh) {
          Serial.println("OctDown");
          octUp = false;
          octDown = true;
          octArr = 0;
        }
        else if (volt >= sharpThresh) {
          Serial.println("OctUp");
          octUp = true;
          octDown = false;
          octArr = 2;
        }
        else {
          octUp = false;
          octDown = false;
          octArr = 1;
        }
    }
    else if (i == 13) {
      if (octaveShift > -3 && volt == 0) {
        if (lastOctaveDown == 0 || ((millis() - lastOctaveDown) > 200)) {
          octaveShift -= 1;
          Serial.println("octave down button");
        }
       lastOctaveDown = millis();
      }
    }
    else if (i == 14) {
      if (volt >= thresh) {
        Serial.println("middle C button");
        octaveShift = 0;
      }
    }
    else if (i == 15) {
      if (octaveShift <= 3 && volt >= thresh) {
        if (lastOctaveUp == 0 || ((millis() - lastOctaveUp) > 200)) {
          octaveShift += 1;
          Serial.println("octave up button");
        }
       lastOctaveUp = millis();
      }
    }
    else if (i == 16) {
      if (volt >= thresh) {
        Serial.println("preset 1");
        if (!pre1) {
          pre1 = true;
          pre2 = false;
          pre3 = false;
          usbMIDI.sendProgramChange(0,1);
        }
      }
    }
    else if (i == 17) {
      if (volt >= thresh) {
        Serial.println("preset 2");
        if (!pre2) {
          pre2 = true;
          pre1 = false;
          pre3 = false;
          usbMIDI.sendProgramChange(1,1);
        }
      }
    }
    else if (i == 18) {
      if (volt >= thresh) {
        Serial.println("preset 3");
        if (!pre3) {
          pre3 = true;
          pre2 = false;
          pre1 = false;
          usbMIDI.sendProgramChange(2,1);
        }
      }
    }
    else {
      vol = (volt+100.00)/(700.00) * 100.00;
      note = CKeyNote[i] + 12*octaveShift + NoteVar[octArr][accArr];
      showNotes(octaveShift + octArr);
      showPre(pre1, pre2, pre3);
      if (volt >= thresh) {
        digitalWrite(dPinNum[i], HIGH);
        for (int o = 0; o < 3; o++) {
          for (int a = 0; a <3; a++) {
            int testNote = CKeyNote[i] + 12*octaveShift + NoteVar[o][a];
            if (o == octArr && a == accArr) {
              if (NoteOn[testNote-21]) { //already playing
                usbMIDI.sendControlChange(7,vol,i);
              }
              else {
                usbMIDI.sendNoteOn(testNote,vol,i);
                NoteOn[testNote-21] = true;
              }
            }
            else {
              NoteOn[testNote-21] = false;
              usbMIDI.sendNoteOff(testNote,vol,i);
            }
          }
        }
      }
      else {
        digitalWrite(dPinNum[i], LOW);
        if (NoteOn[note-21]) {          
          usbMIDI.sendNoteOff(note,vol,i);
          NoteOn[note-21] = false;
        }
      }
    }
  }
  delay(50);
}

void showNotes(int octPos){
  left.setRow(0, 7, 0x4E);
  left.setDigit(0, 6, 3+octPos, false);
}

void showPre(bool pre1, bool pre2, bool pre3) {
  int pre = 1;
  if (pre1) {
    pre = 1;
  }
  else if (pre2) {
    pre = 2;
  }
  else if (pre3) {
    pre = 3;
  }
  left.setDigit(0, 3, 5, false);
  left.setRow(0, 2, B1101111);
  left.setRow(0, 1, B0001111);
  left.setDigit(0, 0, pre, false);
}

