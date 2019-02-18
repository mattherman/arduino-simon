typedef enum { GENERATE_SEQUENCE, WAIT_FOR_INPUT, GAME_OVER } state;
typedef enum { RED, BLUE, YELLOW, GREEN, NONE } color;
static const char *COLOR_STRING[] = {
    "Red", "Blue", "Yellow", "Green", "None"
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
    digitalWrite(colorToPin(next), HIGH);
    delay(250);
    digitalWrite(colorToPin(next), LOW);
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
    digitalWrite(colorToPin(selectedColor), !state);
  } else {
    digitalWrite(colorToPin(selectedColor), !state);
  }  
}

void gameOver(bool win) {
  currentState = GAME_OVER;
  
  Serial.print("Win? ");
  Serial.println(win);
  
  digitalWrite(RED_OUT, HIGH);
  delay(250);
  digitalWrite(RED_OUT, LOW);
  digitalWrite(BLUE_OUT, HIGH);
  delay(250);
  digitalWrite(BLUE_OUT, LOW);
  digitalWrite(YELLOW_OUT, HIGH);
  delay(250);
  digitalWrite(YELLOW_OUT, LOW);
  digitalWrite(GREEN_OUT, HIGH);
  delay(250);
  digitalWrite(GREEN_OUT, LOW);
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
