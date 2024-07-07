#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

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
  lcd.print("Stand by...");

  //          for(i=0 ; i<sizeof(Master);i++){        //When   you upload the code the first time keep it commented
  //            EEPROM.get(i,   Master[i]);             //Upload the code and change it to store it in the EEPROM
  //              }                                  //Then uncomment this for loop and   reupload the code (It's done only once)
}

void loop() {

  // lcd.setCursor(0,0);
  // lcd.print("Enter Password:");

  customKey = customKeypad.getKey();
  if (customKey == '*') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Password:");
    // while (1)
    // {
    //   /* code */
    //   if (data_count == Password_Length - 1)
    //   {
    //     break;
    //   }
    //   customKey = customKeypad.getKey();
    //   if (customKey != NULL)
    //   {
    //     Data[data_count] = customKey;
    //     lcd.setCursor(data_count, 1);
    //     lcd.print(Data[data_count]);
    //     data_count++;
    //   }
    // }
    GetCode();

    if (data_count == Password_Length - 1) {
      lcd.clear();

      if (!strcmp(Data, Master)) {
        lcd.print("Correct");
        if (isOpen) {
          lcd.clear();
          lcd.print("Door is opened!");
          delay(2000);
        } else {

          myServo.write(0);  // Rotate servo to unlock (or your "unlock" position)
          delay(120);        // Wait for 5 seconds
          myServo.write(91);
          delay(2000);
          isOpen = true;
        }
      } else {
        lcd.print("Incorrect");
        delay(2000);
      }

      WaitKey();
    }
  }

  if (customKey == '#') {  // To change the code it calls the changecode function
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Old Password:");
    GetCode();
    if (data_count == Password_Length - 1) {
      lcd.clear();

      if (!strcmp(Data, Master)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("New Password:");
        ChangePassword();
      } else {
        lcd.print("Incorrect");
        delay(2000);
      }

      WaitKey();
    }
  }

  if (customKey == 'D') {
    if (isOpen) {

      myServo.write(180);  // Rotate servo to "lock" position
      delay(120);          // Wait
      myServo.write(91);
      isOpen = false;

      lcd.clear();
      lcd.print("Door locked");
      delay(2000);
    } else {
      lcd.clear();
      lcd.print("Door locked!");
      delay(2000);
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

void WaitKey() {
  lcd.clear();
  clearData();
  lcd.print("Standby...");
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
  }

  lcd.clear();
  lcd.print("Pwd Changed!");
  delay(2000);  // Show confirmation for 2 seconds
}