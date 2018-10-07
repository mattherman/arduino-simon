const int RED_OUT = 10;
const int BLUE_OUT = 11;
const int YELLOW_OUT = 12;
const int GREEN_OUT = 13;

const int RED_IN = 40;
const int BLUE_IN = 42;
const int YELLOW_IN = 44;
const int GREEN_IN = 46;

const int MAX_SEQ_LEN = 5;

int sequence[MAX_SEQ_LEN];
int sequenceLength;

void setup() {
  Serial.begin(9600);
  
  pinMode(RED_OUT, OUTPUT);
  pinMode(BLUE_OUT, OUTPUT);
  pinMode(YELLOW_OUT, OUTPUT);
  pinMode(GREEN_OUT, OUTPUT);

  randomSeed(analogRead(0));

  sequenceLength = 0;
}

void loop() {
  if (sequenceLength == MAX_SEQ_LEN)
  {
    endGame();
    return;
  }
  
  int next = random(10, 14);
  sequence[sequenceLength] = next;
  outputSequence();
  sequenceLength++;
  delay(1000);
}

void outputSequence() {
  Serial.print("\nSequence length: ");
  Serial.print(sequenceLength);

  Serial.print("\nSequence: ");
  for(int i=0; i<=sequenceLength; i++) {
    Serial.print(sequence[i]);
    Serial.print(", ");
    
    digitalWrite(sequence[i], HIGH);
    delay(250);
    digitalWrite(sequence[i], LOW);
    delay(500);
  }
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

