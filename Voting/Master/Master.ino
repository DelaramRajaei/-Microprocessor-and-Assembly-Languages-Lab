/*Delaram Rajaei 9731084*/
#include<SPI.h>
#include <Wire.h>

/*Slaves*/
#define SS_1  49
#define SS_2  48
#define SS_3  47
#define SS_4  46

/*Buzzer*/
#define BUZZER 7

/*LED*/
#define SLAVE1   13
#define SLAVE2   12
#define SLAVE3   11
#define SLAVE4   10

/*EEPROM addresses*/
#define ADDR 0b1010000
#define VOTES 0xAB
#define Rajaei_VOTES 0x50
#define CHARLES_VOTES 0x51
#define LANCASTER_VOTES 0x52
#define MACDONALD_VOTES 0x53
#define ROMAN_VOTES 0x54

/*Votes*/
int allVotes = 0;
int RajaeiVotes = 0;
int CharlesVotes = 0;
int LancasterVotes = 0;
int MacdonaldVotes = 0;
int RomanVotes = 0;

/*SPI*/
byte Mastersend, Mastereceive;
char myArray[50];
int i = 0;
char vote;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8); //Divide clk by 8

  /*Pins*/
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  pinMode(SS_1, OUTPUT);
  pinMode(SS_2, OUTPUT);
  pinMode(SS_3, OUTPUT);
  pinMode(SS_4, OUTPUT);

  /*Buzzer*/
  pinMode(BUZZER, OUTPUT);

  /*Slaves*/
  digitalWrite(SS_1, HIGH);   //Disable Slave 1
  digitalWrite(SS_2, HIGH);   //Disable Slave 2
  digitalWrite(SS_3, HIGH);   //Disable Slave 3
  digitalWrite(SS_4, HIGH);   //Disable Slave 4

  sendNumbers();
  restoreVotes();
  Serial.println("Welcome...");
  printVotes();
}

void loop() {
  digitalWrite(SS_1, LOW);   //Enable Slave 1
  digitalWrite(SS_2, LOW);   //Enable Slave 2
  digitalWrite(SS_3, LOW);   //Enable Slave 3
  digitalWrite(SS_4, LOW);   //Enable Slave 4

  Mastereceive = SPI.transfer("OK");
  if (Mastereceive == '1' || Mastereceive == '2' || Mastereceive == '3' || Mastereceive == '4' || Mastereceive == '5') {
    vote = Mastereceive;
    Serial.print("New vote for ");
    Serial.println(vote);
    tone(BUZZER, 100, 100);
    saveVotes(vote);
    printVotes();
  }
}

void checkVotes() {
  if (allVotes == 255)allVotes = 0;
  if (RajaeiVotes == 255)RajaeiVotes = 0;
  if (CharlesVotes == 255)CharlesVotes = 0;
  if (LancasterVotes == 255)LancasterVotes = 0;
  if (MacdonaldVotes == 255)MacdonaldVotes = 0;
  if (RomanVotes == 255)RomanVotes = 0;
}

void  printVotes() {
  Serial.println("===========================================");
  Serial.print("All votes -> ");
  Serial.println(allVotes);

  Serial.print("1.Delaram Rajaei votes -> ");
  Serial.println(RajaeiVotes);

  Serial.print("2.Nicole Charles votes -> ");
  Serial.println(CharlesVotes);

  Serial.print("3.Aimie Lancaster votes -> ");
  Serial.println(LancasterVotes);

  Serial.print("4.Jacob Macdonald votes -> ");
  Serial.println(MacdonaldVotes);

  Serial.print("5.Colin Roman votes -> ");
  Serial.println(RomanVotes);
  Serial.println("===========================================");
}

void saveVotes(char vote) {
  Serial.println("Writing on EEPROM ...");
  if (vote == '1') {
    RajaeiVotes += 1;
    writeEEPROM (Rajaei_VOTES , (uint8_t)RajaeiVotes);
  } else if (vote == '2') {
    CharlesVotes += 1;
    writeEEPROM (CHARLES_VOTES , (uint8_t)CharlesVotes);
  }  else if (vote == '3') {
    LancasterVotes += 1;
    writeEEPROM (LANCASTER_VOTES , (uint8_t)LancasterVotes);
  }  else if (vote == '4') {
    MacdonaldVotes += 1;
    writeEEPROM (MACDONALD_VOTES , (uint8_t)MacdonaldVotes);
  }  else if (vote == '5') {
    RomanVotes += 1;
    writeEEPROM (ROMAN_VOTES , (uint8_t)RomanVotes);
  }
  allVotes += 1;
  writeEEPROM (VOTES , allVotes);
}

void restoreVotes() {
  Serial.println("Reading from EEPROM ...");
  RajaeiVotes = readEEPROM (Rajaei_VOTES);
  CharlesVotes = readEEPROM (CHARLES_VOTES);
  LancasterVotes = readEEPROM (LANCASTER_VOTES);
  MacdonaldVotes = readEEPROM (MACDONALD_VOTES);
  RomanVotes = readEEPROM (ROMAN_VOTES);
  allVotes = readEEPROM (VOTES);
  checkVotes();
}

void sendNumbers() {
  Serial.println("Sending Slave numbers...");

  digitalWrite(SS_1, LOW);   //Enable Slave 1
  delay(100);
  //Sending text...
  Mastersend = 1;
  SPI.transfer(Mastersend);
  delay(1);
  digitalWrite(SS_1, HIGH);  //Disable Slave 1
  delay(200);

  digitalWrite(SS_2, LOW);   //Enable Slave 2
  delay(100);
  //Sending text...
  Mastersend = 2;
  SPI.transfer(Mastersend);
  delay(1);
  digitalWrite(SS_2, HIGH);  //Disable Slave 2
  delay(200);

  digitalWrite(SS_3, LOW);   //Enable Slave 3
  delay(100);
  //Sending text...
  Mastersend = 3;
  SPI.transfer(Mastersend);
  delay(1);
  digitalWrite(SS_3, HIGH);  //Disable Slave 3
  delay(200);

  digitalWrite(SS_4, LOW);   //Enable Slave 4
  delay(100);
  //Sending text...
  Mastersend = 4;
  SPI.transfer(Mastersend);
  delay(1);
  digitalWrite(SS_4, HIGH);  //Disable Slave 4
  delay(200);

}

void writeEEPROM (uint8_t memAddress , uint8_t data) {
  Wire.beginTransmission(ADDR);
  Wire.write(0x00);
  Wire.write(memAddress);
  Wire.write(data);
  Wire.endTransmission();
  delay(500);
}

uint8_t readEEPROM (uint8_t memAddress) {
  uint8_t data;
  Wire.beginTransmission(ADDR);
  Wire.write(0x00);
  Wire.write(memAddress);
  Wire.endTransmission();

  delay(1);
  Wire.requestFrom(ADDR, (uint8_t)1);
  while (!Wire.available());
  data = Wire.read();
  return data;
}
