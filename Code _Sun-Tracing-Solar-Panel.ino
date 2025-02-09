#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SD.h>
#include <Servo.h>

// Define pins
#define SD_CS_PIN 4
#define SERVO_X_PIN 9
#define SERVO_Y_PIN 10
#define CURRENT_SENSOR_PIN A0
#define VOLTAGE_SENSOR_PIN A1

// Initialize components
Adafruit_SSD1306 display(128, 64, &Wire);
RTC_DS3231 rtc;
Servo servoX;
Servo servoY;
File dataFile;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Initialize servos
  servoX.attach(SERVO_X_PIN);
  servoY.attach(SERVO_Y_PIN);

  // Display welcome message
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Welcome to the");
  display.println("Sun Tracking Solar");
  display.println("Panel Project by H.R.");
  display.display();
  delay(3000);
  display.clearDisplay();
}

void loop() {
  // Get current time
  DateTime now = rtc.now();

  // Read sensors
  float current = analogRead(CURRENT_SENSOR_PIN) * (5.0 / 1023.0);
  float voltage = analogRead(VOLTAGE_SENSOR_PIN) * (5.0 / 1023.0) * 2; // Assuming a voltage divider

  // Get servo positions
  int servoXPos = servoX.read();
  int servoYPos = servoY.read();

  // Display data
  display.setCursor(0, 0);
  display.println("Sun Tracking Solar");
  display.println("Panel Project by H.R.");
  display.print("Time: ");
  display.print(now.hour());
  display.print(":");
  display.print(now.minute());
  display.print(":");
  display.println(now.second());
  display.print("Power: ");
  display.print(current * voltage);
  display.println(" W");
  display.print("Voltage: ");
  display.print(voltage);
  display.println(" V");
  display.print("Servo X: ");
  display.print(servoXPos);
  display.println(" deg");
  display.print("Servo Y: ");
  display.print(servoYPos);
  display.println(" deg");
  display.display();
  delay(1000);
  display.clearDisplay();

  // Log data to SD card
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(now.timestamp());
    dataFile.print(", ");
    dataFile.print(current * voltage);
    dataFile.print(" W, ");
    dataFile.print(voltage);
    dataFile.print(" V, ");
    dataFile.print(servoXPos);
    dataFile.print(" deg, ");
    dataFile.print(servoYPos);
    dataFile.println(" deg");
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.txt");
  }

  // End of day message
  if (now.hour() == 23 && now.minute() == 59 && now.second() == 59) {
    display.setCursor(0, 0);
    display.println("Good night!");
    display.println("See you tomorrow!");
    display.display();
    delay(10000);
    display.clearDisplay();
  }
}