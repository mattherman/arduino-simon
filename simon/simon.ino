/*************************************************
 * Public Constants
 *************************************************/
 
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

typedef enum { GENERATE_SEQUENCE, WAIT_FOR_INPUT, GAME_OVER } state;
typedef enum { RED, BLUE, YELLOW, GREEN, NONE } color;
static const char *COLOR_STRING[] = {
  "Red", "Blue", "Yellow", "Green", "None"
};
static const int COLOR_TONE[] = {
  NOTE_A5, NOTE_E5, NOTE_CS5, NOTE_E4
};

state currentState = GENERATE_SEQUENCE;

const byte RED_OUT = 2;
const byte BLUE_OUT = 3;
const byte YELLOW_OUT = 4;
const byte GREEN_OUT = 5;

const byte RED_IN = 21;
const byte BLUE_IN = 20;
const byte YELLOW_IN = 19;
const byte GREEN_IN = 18;

const byte BUZZER = 7;
const bool MUTE = false;

const int MAX_SEQ_LEN = 5;

color sequence[MAX_SEQ_LEN];
int sequenceLength;

int currentSequenceIndex = 0;
volatile color lastInput = NONE;
unsigned long start = 0;
const unsigned long WAIT_TIME_MS = 5000;

void setup() {
  Serial.begin(9600);
  
  pinMode(RED_OUT, OUTPUT);
  pinMode(BLUE_OUT, OUTPUT);
  pinMode(YELLOW_OUT, OUTPUT);
  pinMode(GREEN_OUT, OUTPUT);

  pinMode(RED_IN, INPUT_PULLUP);
  pinMode(BLUE_IN, INPUT_PULLUP);
  pinMode(YELLOW_IN, INPUT_PULLUP);
  pinMode(GREEN_IN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(RED_IN), redInput, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BLUE_IN), blueInput, CHANGE);
  attachInterrupt(digitalPinToInterrupt(YELLOW_IN), yellowInput, CHANGE);
  attachInterrupt(digitalPinToInterrupt(GREEN_IN), greenInput, CHANGE);

  randomSeed(analogRead(0));

  sequenceLength = 0;
}

void loop() {
  switch(currentState) {
    case GENERATE_SEQUENCE: 
      generateSequence();
      break;
    case WAIT_FOR_INPUT:
      waitForInput();
      break;
    default: 
      break;
  }
}

void generateSequence() {
  Serial.println("Updating sequence...");
  delay(1000);
  if (sequenceLength == MAX_SEQ_LEN)
  {
    gameOver(true);
    return;
  }
  
  int next = random(2, 6);
  sequence[sequenceLength] = pinToColor(next);
  outputSequence();
  sequenceLength++;

  resetInput();
  currentState = WAIT_FOR_INPUT;
}

void outputSequence() {
  for(int i=0; i<=sequenceLength; i++) {
    color next = sequence[i];
    Serial.print(COLOR_STRING[next]);
    Serial.print(" ");
    activateOutput(next);
    delay(250);
    deactivateOutput(next);
    delay(500);
  }
  Serial.print("\n");
}

void waitForInput() {
  // Check if we have exceeded the wait time, if so, game over
  unsigned long current = millis();
  if (current > (start + WAIT_TIME_MS)) {
    Serial.println("Ran out of time!");
    gameOver(false);
  }
  
  // No buttons pressed yet
  if (lastInput == NONE) return;
  
  color expectedInput = sequence[currentSequenceIndex];

  Serial.print("Expected=");
  Serial.print(COLOR_STRING[expectedInput]);
  Serial.print("|Actual=");
  Serial.println(COLOR_STRING[lastInput]);
  
  if (lastInput == expectedInput) {
    currentSequenceIndex++;
    if (currentSequenceIndex == sequenceLength) {
      currentSequenceIndex = 0;
      currentState = GENERATE_SEQUENCE;
    } else {
      resetInput();
    }
  } else {
    Serial.println("Wrong!");
    gameOver(false);
  }
}

void activateOutput(color target) {
  digitalWrite(colorToPin(target), HIGH);
  if (!MUTE) tone(BUZZER, COLOR_TONE[target]);
}

void deactivateOutput(color target) {
  digitalWrite(colorToPin(target), LOW);
  noTone(BUZZER);
}

void resetInput() {
  lastInput = NONE;
  start = millis();
}

void redInput() {
  handleInput(RED, digitalRead(RED_IN));
}

void blueInput() {
  handleInput(BLUE, digitalRead(BLUE_IN));
}

void yellowInput() {
  handleInput(YELLOW, digitalRead(YELLOW_IN));
}

void greenInput() {
  handleInput(GREEN, digitalRead(GREEN_IN));
}

void handleInput(color selectedColor, int state) {
  Serial.print("Setting ");
  Serial.print(COLOR_STRING[selectedColor]);
  Serial.print(" to ");
  Serial.println(state);
  if (state == LOW) {
    Serial.print(COLOR_STRING[selectedColor]);
    Serial.println(" pressed..");
    lastInput = selectedColor;
    activateOutput(selectedColor);
  } else {
    deactivateOutput(selectedColor);
  }  
}

void gameOver(bool win) {
  currentState = GAME_OVER;
  
  Serial.print("Win? ");
  Serial.println(win);
  
  activateOutput(RED);
  delay(250);
  deactivateOutput(RED);
  activateOutput(BLUE);
  delay(250);
  deactivateOutput(BLUE);
  activateOutput(YELLOW);
  delay(250);
  deactivateOutput(YELLOW);
  activateOutput(GREEN);
  delay(250);
  deactivateOutput(GREEN);
}

color pinToColor(int pin) {
  if (pin == RED_OUT) return RED;
  if (pin == BLUE_OUT) return BLUE;
  if (pin == YELLOW_OUT) return YELLOW;
  if (pin == GREEN_OUT) return GREEN;
  return NONE;
}

int colorToPin(color ledColor) {
  if (ledColor == RED) return RED_OUT;
  if (ledColor == BLUE) return BLUE_OUT;
  if (ledColor == YELLOW) return YELLOW_OUT;
  if (ledColor == GREEN) return GREEN_OUT;
  return 0;
}
