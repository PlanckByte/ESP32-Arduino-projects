#include <SPI.h> // communicate with RFID module using SPI protocol
#include <MFRC522.h> //RF reader libary
#include <Wire.h> // communicate with LCD using I2C protocol
#include <LiquidCrystal_I2C.h> // LCD library
// set up RFID reader pins
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

//Defining and initializing an byte arrays that are the known UIDs of my cards to compare with the reading from the RC522 when I scan them.
byte START_PAUSE_UID[] = {0x83, 0x7C, 0xAC, 0xAA};
byte STOP_RESET_UID[] = {0xF7, 0x47, 0x77, 0x11};

// Define LCD settings
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables to track stopwatch state
bool stopwatchRunning = false;
unsigned long startTime = 0; // Variable to store start time
unsigned long pauseTime = 0; // Variable to store pause time

void setup() {
  Serial.begin(9600);  // Initialize serial communication to view if the RFID cards are scanned
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522

  lcd.init();          // Init LCD
  lcd.backlight();    // Turn on backlight
  lcd.setCursor(0, 0); // Set cursor on (x,y) = (0,0)
  lcd.print("RFID Stopwatch");
}

void loop() {
  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Check if the card UID matches START/PAUSE
    if (compareUID(mfrc522.uid.uidByte, START_PAUSE_UID, 4)) {
      if (!stopwatchRunning) {
        // Start the stopwatch if it's not already running
        stopwatchRunning = true;
        startTime = millis(); // Record start time
        lcd.clear();
        lcd.print("Stopwatch start");
      } else {
        // Pause the stopwatch if it's running
        stopwatchRunning = false;
        pauseTime = millis(); // Record pause time
        lcd.clear();
        lcd.print("Stopwatch paused:");
        displayTime(pauseTime - startTime);
      }
    }
    // Check if the card UID matches STOP/RESET
    else if (compareUID(mfrc522.uid.uidByte, STOP_RESET_UID, 4)) {
      // Reset the stopwatch regardless of its state
      stopwatchRunning = false;
      lcd.clear();
      lcd.print("Stopwatch reset");
      lcd.setCursor(0,1);
      lcd.print("0:00:00");
    }
    mfrc522.PICC_HaltA(); // Stop reading
  }

  // Update display with stopwatch time
  if (stopwatchRunning) {
    displayTime(millis() - startTime);
  }
}

// Function to compare two UIDs
bool compareUID(byte *uid1, byte *uid2, int size) {
  for (int i = 0; i < size; i++) {
    if (uid1[i] != uid2[i]) {
      return false;
    }
  }
  return true;
}

// Function to display elapsed time on LCD
void displayTime(unsigned long elapsedTime) {
  unsigned long seconds = elapsedTime / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long milliseconds = elapsedTime % 1000;

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds % 60 < 10) lcd.print("0");
  lcd.print(seconds % 60);
  lcd.print(":");
  if (milliseconds < 10) lcd.print("00");
  else if (milliseconds < 100) lcd.print("0");
  lcd.print(milliseconds);
}