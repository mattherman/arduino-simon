#define NOTE_A2  110
#define NOTE_E4  330
#define NOTE_CS5 554
#define NOTE_E5  659
#define NOTE_A5  880

typedef enum { GENERATE_SEQUENCE, WAIT_FOR_INPUT, GAME_OVER } state;
typedef enum { RED, BLUE, YELLOW, GREEN, NONE } color;
static const char *COLOR_STRING[] = {
  "Red", "Blue", "Yellow", "Green", "None"
};
static const int COLOR_TONE[] = {
  NOTE_A5, NOTE_E5, NOTE_CS5, NOTE_E4
};
const int LOSS_TONE = NOTE_A2;

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
const unsigned long WAIT_TIME_MS = 3000;

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
  delay(1000);
  if (sequenceLength == MAX_SEQ_LEN)
  {
    Serial.println("Win!");
    gameOver(true);
    return;
  }

  Serial.println("Updating sequence...");
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
  if (state == LOW) {
    lastInput = selectedColor;
    activateOutput(selectedColor);
  } else {
    deactivateOutput(selectedColor);
  }  
}

void gameOver(bool win) {
  currentState = GAME_OVER;
  
  if (win) {
    outputWinSequence();
  } else {
    outputLoseSequence();
  }
}

void outputWinSequence() {
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

  delay(250);
  activateOutput(GREEN);
  delay(250);
  deactivateOutput(GREEN);
  activateOutput(BLUE);
  delay(150);
  deactivateOutput(BLUE);
  activateOutput(YELLOW);
  delay(150);
  deactivateOutput(YELLOW);
  activateOutput(RED);
  delay(150);
  deactivateOutput(RED);
}

void outputLoseSequence() {
  delay(1000);
  tone(BUZZER, LOSS_TONE, 1000);
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
