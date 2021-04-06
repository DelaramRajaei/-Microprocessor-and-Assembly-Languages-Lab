#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

/*Safe States*/
bool lock = true;

/*Password*/
String Password = "1234";

/*LED*/
#define LED_LOCK   A0
#define LED_UNLOCK A1
#define BUZZER 7 


/* Locking mechanism definitions */
#define SERVO_PIN 2
#define SERVO_LOCK_POS   20
#define SERVO_UNLOCK_POS 90
Servo lockServo;

/*Delay*/
unsigned long delayStart = 0;

/* Display */
const int rs = 13, en = 12, d4 = 8, d5 = 9, d6 = 10, d7 = 11 ;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/* Keypad setup */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 3;
byte rowPins[KEYPAD_ROWS] = {25, 26, 27, 28};
byte colPins[KEYPAD_COLS] = {22, 23, 24};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

void setup() {
  /*Buzzer*/
  pinMode(BUZZER, OUTPUT);

  /*LED*/
  pinMode(LED_LOCK, OUTPUT);
  pinMode(LED_UNLOCK, OUTPUT);

  lcd.begin(16, 2);
  lockServo.attach(SERVO_PIN, 1000, 2000);

  /* Make sure the physical lock is sync with the EEPROM state */
  Serial.begin(115200);
  if (lock) {
    closeSafe();
  } else {
    openSafe();
  }

  showStartupMessage();
}

void loop() {
  if (lock) {
    safeLockedLogic();
  } else {
    safeUnlockedLogic();
  }
}

void closeSafe() {
  tone(BUZZER, 100, 500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Close");
  lockServo.write(SERVO_LOCK_POS);
  digitalWrite(LED_LOCK, HIGH);
  digitalWrite(LED_UNLOCK, LOW);
  lock = true;
}

void openSafe() {
  tone(BUZZER, 2000, 500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Open");
  lockServo.write(SERVO_UNLOCK_POS);
  digitalWrite(LED_UNLOCK, HIGH);
  digitalWrite(LED_LOCK, LOW);
  lock = false;
}

void showStartupMessage() {
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");

  lcd.setCursor(0, 0);
  lcd.print("Enter pass:");
}

void safeUnlockedLogic() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1 -> lock");
  lcd.setCursor(0, 1);
  lcd.print("2 -> new code");

  char key = keypad.getKey();
  bool flag = false;
  delayStart = millis();
  while ((millis() - delayStart) <= 10000 && key != '1' && key != '2') {
    key = keypad.getKey();
    lcd.setCursor(14, 1);
    lcd.print((millis() - delayStart) / 1000);
  }

  if (key == '2') {
    setNewCode();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Locked!");
  closeSafe();
  showWaitScreen(100);

}

void safeLockedLogic() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Safe Locked! ");

  String userCode = getPass();
  bool unlockedSuccessfully = checkPass(userCode);
  showWaitScreen(200);

  if (unlockedSuccessfully) {
    showUnlockMessage();
    openSafe();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied!");
    showWaitScreen(500);
  }
}

String getPass() {
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
