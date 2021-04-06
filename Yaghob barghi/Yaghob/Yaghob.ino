#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

/*Yaghob States*/
bool lock = true;

/*Password*/
String Password = "1234";

/*LEDS*/
#define ALL_LED   A12

#define COL1 A0
#define COL2 A1
#define COL3 A2
#define COL4 A3

#define ROW1 A8
#define ROW2 A9
#define ROW3 A10
#define ROW4 A11

/* Locking mechanism definitions */
#define SERVO_PIN 7
#define SERVO_LOCK_POS   20
#define SERVO_UNLOCK_POS 90
Servo lockServo;

/*Row's and column's servos*/
#define SERVO_PIN1 6
#define SERVO_UNLOCK_POS1 0
Servo servo1;

#define SERVO_PIN2 5
#define SERVO_UNLOCK_POS2 0
Servo servo2;

#define SERVO_PIN3 4
#define SERVO_UNLOCK_POS3 0
Servo servo3;


/*Number Of Foods*/
const byte LED_ROWS_ARRAY = 4;
const byte LED_COLS_ARRAY = 4;
int count[LED_COLS_ARRAY][LED_ROWS_ARRAY] = {
  {9, 9, 9, 9},
  {9, 9, 9, 9},
  {9, 9, 9, 9},
  {9, 9, 9, 9}
};
int servoLock = 100;

/*Delay*/
unsigned long delayStart = 0;

/* Display */
const int rs = 13, en = 12, d4 = 8, d5 = 9, d6 = 10, d7 = 11 ;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/* Keypad setup */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 3;
byte rowPins[KEYPAD_ROWS] = {26, 27, 28, 29};
byte colPins[KEYPAD_COLS] = {25, 24, 23};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

void setup() {
  /*LED*/
  pinMode(ALL_LED, OUTPUT);

  pinMode(COL1, OUTPUT);
  pinMode(COL2, OUTPUT);
  pinMode(COL3, OUTPUT);
  pinMode(COL4, OUTPUT);

  pinMode(ROW1, OUTPUT);
  pinMode(ROW2, OUTPUT);
  pinMode(ROW3, OUTPUT);
  pinMode(ROW4, OUTPUT);

  lcd.begin(16, 2);
  lockServo.attach(SERVO_PIN, 1000, 2000);

  servo1.attach(SERVO_PIN1, 1000, 2000);
  servo2.attach(SERVO_PIN2, 1000, 2000);
  servo3.attach(SERVO_PIN3, 1000, 2000);

  /* Make sure the physical lock is sync with the EEPROM state */
  Serial.begin(115200);
  if (lock) {
    lockYaghob();
  } else {
    openYaghob();
  }

  showStartupMessage();
}

void loop() {
  if (lock) {
    lockedLogic();
  } else {
    unlockedLogic();
  }
}

void openYaghob() {
  TrunOffAllLights();
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Opend!");
  lockServo.write(SERVO_UNLOCK_POS);
  digitalWrite(ALL_LED, LOW);
  lock = false;
}

void lockYaghob() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Locked!");
  lockServo.write(SERVO_LOCK_POS);
  digitalWrite(ALL_LED, HIGH);
  TrunOffAllLights();
  lock = true;
}

void showStartupMessage() {
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");
  delay(2000);
}

void unlockedLogic() {
  TrunOffAllLights();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1.Lock");
  lcd.setCursor(0, 1);
  lcd.print("2.New Code");

  servo1.write(SERVO_UNLOCK_POS1);
  servo2.write(SERVO_UNLOCK_POS2);
  servo3.write(SERVO_UNLOCK_POS3);

  char key = keypad.getKey();
  bool flag = false;
  while (key != '1' && key != '2') {
    key = keypad.getKey();
  }

  if (key == '2') {
    setNewCode();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Locked!");
  lockYaghob();
  showWaitScreen(100);

}

void lockedLogic() {
  TrunOffAllLights();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1.Unlock");
  lcd.setCursor(0, 1);
  lcd.print("2.Get Food");

  servo1.write(servoLock);
  servo2.write(servoLock);
  servo3.write(servoLock);

  char key = keypad.getKey();
  bool flag = false;
  while (key != '1' && key != '2') {
    key = keypad.getKey();
  }

  if (key == '1') {
    String userCode = getPass();
    bool unlockedSuccessfully = checkPass(userCode);
    showWaitScreen(200);

    if (unlockedSuccessfully) {
      showUnlockMessage();
      openYaghob();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Access Denied!");
      showWaitScreen(500);
    }
  }
  if (key == '2') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("#.Delete");
    lcd.setCursor(0, 1);
    lcd.print("*.Confirm");
    char col ;
    char row ;
    int i = 0;
    while (i <= 3) {
      char key = keypad.getKey();
      if (key == '#' || i == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("#.Delete");
        lcd.setCursor(0, 1);
        lcd.print("*.Confirm");
        i = 1;
      }
      else if (key == '*' && i == 3) {
        i = getFood(col, row);
      }
      else if (key != NO_KEY && key != '#' && key != '*') {
        if (i == 1) {
          lcd.setCursor(14, 1);
          lcd.print(key);
          row = key;
        }
        else if (i == 2) {
          lcd.setCursor(15, 1);
          lcd.print(key);
          col = key;
        }
        i++;
      }
      else if (key == '*') {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wrong Input!");
        i = 0;
      }
    }
  }


}

String getPass() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter pass:");
  lcd.setCursor(5, 1);
  lcd.print("[____]");
  lcd.setCursor(6, 1);
  String result = "";
  while (result.length() < 4) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9') {
      lcd.print('*');
      result += key;
    }
  }
  return result;
}

bool checkPass(String userCode) {
  if (userCode.equals(Password))
    return true;
  else return false;
}

void showWaitScreen(int delayMillis) {
  lcd.setCursor(2, 1);
  lcd.print("[..........]");
  lcd.setCursor(3, 1);
  for (byte i = 0; i < 10; i++) {
    delay(delayMillis);
    lcd.print("=");
  }
}

void showUnlockMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Unlocked!");
}

void setNewCode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter new code:");
  String newCode = getPass();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Confirm new code");
  String confirmCode = getPass();

  if (newCode.equals(confirmCode)) {
    Password = newCode;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Code match");
    lcd.setCursor(0, 1);
    lcd.print("new pass");
  } else {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Code mismatch");
    lcd.setCursor(0, 1);
    lcd.print("no new pass");
  }
  delay(2000);
}

int getFood(char c, char r) {
  int i;
  TrunOffAllLights();
  int row ;
  int col ;
  row = change(r);
  col = change(c);
  lcd.clear();
  lcd.setCursor(0, 0);
  if (col <= 4 && row <= 4) {
    if (count[row - 1][col - 1] != 0) {
      count[row - 1][col - 1] = count[row - 1][col - 1] - 1;
      servoLock = servoLock - 20;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Number Of food=");
      lcd.setCursor(8, 1);
      lcd.print(count[row - 1][col - 1]);
      TurnOnLights(r, c);
      i = 4;
      delay(2000);
    }
  }
  else {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("No Food!");
    lcd.setCursor(2, 1);
    lcd.print("Pick Another!");
    i = 0;
    delay(2000);
  }
  return i;
}

void TrunOffAllLights() {
  digitalWrite(COL1, LOW);
  digitalWrite(COL2, LOW);
  digitalWrite(COL3, LOW);
  digitalWrite(COL4, LOW);

  digitalWrite(ROW1, LOW);
  digitalWrite(ROW2, LOW);
  digitalWrite(ROW3, LOW);
  digitalWrite(ROW4, LOW);
}

void TurnOnLights(char row, char col) {
  if (col == '1') {
    digitalWrite(COL1, HIGH);
  }
  else if (col == '2') {
    digitalWrite(COL2, HIGH);
  }
  else if (col == '3') {
    digitalWrite(COL3, HIGH);
  }
  else if (col == '4') {
    digitalWrite(COL4, HIGH);
  }

  if (row == '1') {
    digitalWrite(ROW1, HIGH);
  }
  else if (row == '2') {
    digitalWrite(ROW2, HIGH);
  }
  else if (row == '3') {
    digitalWrite(ROW3, HIGH);
  }
  else if (row == '4') {
    digitalWrite(ROW4, HIGH);
  }
}

int change(char in) {

  if (in == '1') {
    return 1;
  }
  else if (in == '2') {
    return 2;
  }
  else if (in == '3') {
    return 3;
  }
  else if (in == '4') {
    return 4;
  }
  else return 5;
}
