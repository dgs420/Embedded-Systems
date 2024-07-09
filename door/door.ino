#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>

#define Password_Length 5
#define SS_PIN A2
#define RST_PIN A3
#define ANGLE 200

int signalPin = 10;

char Data[Password_Length];
char Master[Password_Length] = "1234";
byte data_count = 0, master_count = 0;
String tag_UID = "2371501C";  // Replace these with the UID of your tag
String tag_UID1 = "70D7CF21";

String tagID = "";
MFRC522 mfrc522(SS_PIN, RST_PIN); 
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
  SPI.begin();             // SPI bus
  mfrc522.PCD_Init();
  myServo.attach(signalPin);
  lcd.setCursor(0, 0);
  lcd.print("Waiting...");
  // Initialize Master password from EEPROM
  for (int i = 0; i < Password_Length - 1; i++) {
    EEPROM.get(i, Master[i]);
  }

  // Initialize Lock state from EEPROM
  EEPROM.get(100, isOpen);
}

void loop() {
  readID();
  if (tagID == tag_UID || tagID == tag_UID1) {
    DisplayText(0, 0, "Valid card");
    lcd.clear();
    if (isOpen) {
      DisplayText(0, 0, "Door is opened!");
      lcd.clear();
    } else {
      // Unlock();
      SetLock(0,ANGLE,true);
    }
    tagID = "";
    WaitKey();
  } else if (tagID != tag_UID && tagID != NULL){
    DisplayText(0, 0, "Invalid card");
    tagID = "";
    lcd.clear();
    WaitKey();
  }
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
          delay(1000);
        } else {
          // Unlock();
          SetLock(0,ANGLE,true);
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
      // Lock();
      SetLock(180,ANGLE,false);
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
      // lcd.print(Data[data_count]);
      lcd.print("*");
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

void SetLock(int x, int y, boolean z) {
  myServo.write(x);  // Rotate servo 
  delay(y);          // Wait
  myServo.write(91);
  isOpen = z;
  EEPROM.put(100, isOpen);
}



boolean readID() {
  //Check if a new tag is detected or not. If not return.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }
  //Check if a new tag is readable or not. If not return.
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  tagID = "";
  // Read the 4 byte UID
  for (uint8_t i = 0; i < 4; i++) {
    //readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));  // Convert the UID to a single String
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA();  // Stop reading
  return true;
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
      // lcd.print("*");
      data_count++;
    }
    delay(100);  // Small delay to debounce keypad input
  }

  // Update the Master password
  for (int i = 0; i < Password_Length; i++) {
    Master[i] = Data[i];
    EEPROM.put(i, Master[i]);
  }

  DisplayText(0, 0, "Pwd Changed");
}