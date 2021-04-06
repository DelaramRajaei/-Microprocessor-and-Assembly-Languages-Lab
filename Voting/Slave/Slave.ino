/*Delaram Rajaei 9731084*/
#include <Keypad.h>
#include<SPI.h>

/*LED*/
#define LED_GREEN   11
#define LED_RED   12
#define LED_YELLOW   13

/*Buzzer*/
#define BUZZER 7

/*Voting*/
int count = 0;

/*SPI*/
volatile boolean received = false;
volatile byte Slavereceived, Slavesend;
int machineNumber;

/* Keypad setup */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 3;
byte rowPins[KEYPAD_ROWS] = {25, 26, 27, 28};
byte colPins[KEYPAD_COLS] = {24, 23, 22};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

void setup() {

  Serial.begin(9600);

  /*SPI*/
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8); //Divide clk by 8
  pinMode(SS, INPUT_PULLUP);
  pinMode(MOSI, INPUT);
  pinMode(MISO, OUTPUT);
  pinMode(SCK, INPUT);

  /*LED*/
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  /*Buzzer*/
  pinMode(BUZZER, OUTPUT);

  /*Start*/
  digitalWrite(SS, HIGH);
  SPCR |= _BV(SPE);    // turn on SPI in slave mode
  SPI.attachInterrupt();  //Allows SPI interrupt
}

void loop() {
  if (received) {
    TrunOffAllLights();
    Serial.print("Voting Machine number ");
    Serial.println(machineNumber);
    Serial.println("Press any key to continue ...");
    char key = keypad.getKey();
    while (key ==  NO_KEY) {
      key = keypad.getKey();
    }
    getInput();
  }
}

void getInput() {
  if (count < 2) {
    digitalWrite(LED_YELLOW, HIGH);
    showCandidates();
    Serial.print("Choose a number : ");
    char key = keypad.getKey();
    while (key ==  NO_KEY) {
      key = keypad.getKey();
    }
    Serial.println(key);
    TrunOffAllLights();
    if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5') {
      voting(key);
      count++;
    }
    else {
      digitalWrite(LED_RED, HIGH);
      Serial.println(" Wrong number please try again later... ");
      tone(BUZZER, 100, 100);
    }

  } else {
    Serial.println("Sorry but you already voted for two candidates.");
    digitalWrite(LED_RED, HIGH);
  }
  Serial.println("===========================================");
  delay(500);
  clearScreen();
}

void clearScreen() {
  for (int i = 0; i < 10; i++)
    Serial.println("");
}

void voting(char key) {
  digitalWrite(LED_GREEN, HIGH);
  if (key == '1') {
    Serial.println(" You voted for Delaram Rajaei! ");
  }
  else if (key == '2') {
    Serial.println(" You voted for Nicole Charles! ");
  }
  else if (key == '3') {
    Serial.println(" You voted for  Aimie Lancaster! ");
  }
  else if (key == '4') {
    Serial.println(" You voted for Jacob Macdonald! ");
  }
  else if (key == '5') {
    Serial.println(" You voted for Colin Roman! ");
  }
  Serial.println(" Thank you for voting! ");
  Slavesend = key;
  SPDR = Slavesend;  //Sends the key value to master via SPDR
  Serial.println(" Send Vote!");
  delay(1000);
}

void showCandidates() {
  Serial.println("1. Delaram Rajaei ");
  Serial.println("2. Nicole Charles ");
  Serial.println("3. Aimie Lancaster ");
  Serial.println("4. Jacob Macdonald ");
  Serial.println("5. Colin Roman ");
}

void TrunOffAllLights() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
}

ISR (SPI_STC_vect)
{
  Slavereceived = SPDR;
  if (Slavereceived == 1 || Slavereceived == 2 || Slavereceived == 3 || Slavereceived == 4) {
    machineNumber = Slavereceived;
    received = true;
  }
  Slavereceived = 0;
}
