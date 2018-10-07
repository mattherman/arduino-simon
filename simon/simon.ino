const byte RED_OUT = 2;
const byte BLUE_OUT = 3;
const byte YELLOW_OUT = 4;
const byte GREEN_OUT = 5;

const byte RED_IN = 21;
const byte BLUE_IN = 20;
const byte YELLOW_IN = 19;
const byte GREEN_IN = 18;

const int MAX_SEQ_LEN = 5;

int sequence[MAX_SEQ_LEN];
int sequenceLength;

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
  if (sequenceLength == MAX_SEQ_LEN)
  {
    endGame();
    return;
  }
  
  int next = random(2, 6);
  sequence[sequenceLength] = next;
  outputSequence();
  sequenceLength++;
  delay(1000);
}

void outputSequence() {
  for(int i=0; i<=sequenceLength; i++) {  
    digitalWrite(sequence[i], HIGH);
    delay(250);
    digitalWrite(sequence[i], LOW);
    delay(500);
  }
}

void redInput() {
  Serial.println("RED_IN");
}

void blueInput() {
  Serial.println("BLUE_IN");
}

void yellowInput() {
  Serial.println("YELLOW_IN");
}

void greenInput() {
  Serial.println("GREEN_IN");
}

void endGame() {
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

