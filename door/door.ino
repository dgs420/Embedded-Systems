#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>

#define Password_Length 5

int signalPin = 12;

char Data[Password_Length];
char Master[Password_Length] = "1234";
byte data_count = 0, master_count = 0;
bool Pass_is_good;

bool isOpen = false;
char customKey;

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

short a = 0, i = 0, s = 0, j = 0;

byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

void setup() {
  lcd.init();
  lcd.backlight();
  myServo.attach(signalPin);
  lcd.setCursor(0, 0);
  lcd.print("Waiting...");
  // Initialize Master password from EEPROM
  for (int i = 0; i < Password_Length - 1; i++) {
    EEPROM.get(i, Master[i]);
  }
}

void loop() {

  customKey = customKeypad.getKey();
  if (customKey == '*') {
    DisplayText(0, 0, "Enter Password:");

    GetCode();

    if (data_count == Password_Length - 1) {
      lcd.clear();

      if (!strcmp(Data, Master)) {
        DisplayText(0, 0, "Correct");
        if (isOpen) {
          DisplayText(0, 0, "Door is opened!");
        } else {
          Unlock();
        }
      } else {
        DisplayText(0, 0, "Incorrect");
      }

      WaitKey();
    }
  }

  if (customKey == '#') {  // To change the code it calls the changecode function

    DisplayText(0, 0, "Old Password:");
    GetCode();
    if (data_count == Password_Length - 1) {
      lcd.clear();

      if (!strcmp(Data, Master)) {
        DisplayText(0, 0, "New Password: ");
        ChangePassword();
      } else {
        DisplayText(0, 0, "Incorrect");
      }

      WaitKey();
    }
  }

  if (customKey == 'D') {
    if (isOpen) {
      Lock();
      DisplayText(0, 0, "Door locked");
    } else {

      DisplayText(0, 0, "Door locked!");
    }

    WaitKey();
  }
}

void GetCode() {  // Getting   code sequence
  while (data_count < Password_Length - 1) {
    customKey = customKeypad.getKey();
    if (customKey != NULL) {
      Data[data_count] = customKey;
      lcd.setCursor(data_count, 1);
      lcd.print(Data[data_count]);
      data_count++;
    }
    delay(100);
  }
}

void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
  return;
}

void Unlock() {
  myServo.write(0);  // Rotate servo to unlock (or your "unlock" position)
  delay(120);        // Wait for 5 seconds
  myServo.write(91);
  delay(2000);
  isOpen = true;
}

void Lock() {
  myServo.write(180);  // Rotate servo to "lock" position
  delay(120);          // Wait
  myServo.write(91);
  isOpen = false;
}

void DisplayText(int x, int y, String text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
  delay(1000);
}

void WaitKey() {
  lcd.clear();
  clearData();
  lcd.print("Waiting...");
}

void ChangePassword() {
  clearData();  // Clear any existing data

  while (data_count < Password_Length - 1) {
    char key = customKeypad.getKey();
    if (key) {
      Data[data_count] = key;  // Store the new key press in Data array
      lcd.setCursor(data_count, 1);
      lcd.print(Data[data_count]);
      // lcd.print("*"); // Optionally, print * for each digit entered for privacy
      data_count++;
    }
    delay(100);  // Small delay to debounce keypad input
  }

  // Once the new password is fully entered, update the Master password
  for (int i = 0; i < Password_Length; i++) {
    Master[i] = Data[i];
    EEPROM.put(i, Master[i]);
  }

  DisplayText(0, 0, "Pwd Changed");
}