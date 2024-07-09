# Safe Lock Program

## Overview

The Safe Lock program is a secure access control system using an Arduino Uno, combining RFID and keypad authentication to manage access. This system uses an RFID reader, a keypad, an LCD display, a servo motor, and EEPROM storage to provide security for various applications.

## Components

- **Arduino Uno**
- **RFID Reader (MFRC522)**
- **Keypad (4x4 matrix)**
- **LiquidCrystal_I2C Display**
- **Servo Motor**
- **EEPROM**

## Features

- **RFID Authentication:** Uses RFID tags for contactless identification.
- **Keypad Password Entry:** Uses numeric password to unlock.
- **LCD Display:** Displays prompts and messages to the user.
- **Servo Motor Control:** Manages the physical locking mechanism.
- **EEPROM Storage:** Retains master password and lock state across power cycles.

## How It Works

### Initialization
1. The system initializes the LCD, RFID reader, servo motor, and reads the master password and lock state from EEPROM.
2. The LCD displays "Waiting..." indicating that the system is ready for user input.

### RFID Authentication
1. The system continuously scans for RFID tags.
2. If a valid RFID tag is detected:
   - If the lock is open, the system informs the user that the door is already open.
   - If the lock is closed, the system unlocks the door and updates the lock state in EEPROM.
3. If an invalid RFID tag is detected, the system displays "Invalid card."

### Keypad Password Entry
1. Press '*' to enter the password mode.
2. Enter the password using the keypad.
3. The system compares the entered password with the stored master password.
   - If the password matches and the lock is closed, the system unlocks the door.
   - If the password matches and the lock is open, the system informs the user that the door is already open.
   - If the password is incorrect, the system displays "Incorrect."

### Change Password
1. Press '#' to enter the password change mode.
2. Enter the old password.
3. If the old password is correct, the system prompts for a new password.
4. The new password is saved to EEPROM.

### Manual Locking
1. Press 'D' to manually lock the door if it is not already locked.

## Setup

### Hardware Connections

- **RFID Reader:**
  - SS_PIN to A2
  - RST_PIN to A3
  - Connect other pins according to the MFRC522 datasheet

- **Keypad:**
  - Connect row pins to Arduino pins 9, 8, 7, 6
  - Connect column pins to Arduino pins 5, 4, 3, 2

- **LCD Display:**
  - Connect according to the LiquidCrystal_I2C library documentation

- **Servo Motor:**
  - Signal pin to Arduino pin 10

- **EEPROM:**
  - No external connections needed (built-in to Arduino)

### Software Installation

1. **Arduino IDE:** 
2. **Libraries:** Install the following libraries via the Arduino Library Manager:
   - MFRC522
   - LiquidCrystal_I2C
   - Keypad
   - Servo