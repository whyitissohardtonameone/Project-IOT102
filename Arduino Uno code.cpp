#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

// Initialize LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Initialize RTC object
RTC_DS1307 rtc;

// Define LM35 sensor pin and buzzer/button pins
const int sensorPin = A0;
const int buzzerPin = 8;
const int buttonPin = 7;

// Variables for alarm
int alarmHour = -1;
int alarmMinute = -1;
bool alarmTriggered = false;
bool alarmDisabled = false;
int lastDisableDay = -1; // Tracks the day when alarm was disabled

void setup() {
  // Set up LCD and pins
  lcd.begin(16, 2);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize RTC
  if (!rtc.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("RTC Error!");
    while (1); // Halt if RTC fails
  }

  if (!rtc.isrunning()) {
    lcd.setCursor(0, 0);
    lcd.print("Setting RTC...");
    //rtc.adjust(DateTime(F(__DATE__, __TIME__))); // Set to compile time
  }

  // Begin serial communication
  Serial.begin(9600);
  Serial.println("Enter date and time in format: YYYY-MM-DD HH:MM:SS");
  Serial.println("Enter alarm time in format: AL HH:MM");
}

void loop() {
  // Read temperature
  int sensorValue = analogRead(sensorPin);
  float voltage = sensorValue * (5.0 / 1023.0);
  float temperature = voltage * 100;

  // Get current date and time from RTC
  DateTime now = rtc.now();

  // Reset alarmDisabled daily
  if (now.day() != lastDisableDay) {
    alarmDisabled = false;
    lastDisableDay = now.day();
  }

  // Display date and time on LCD
  lcd.setCursor(0, 0);
  lcd.print(now.year());
  lcd.print("-");
  lcd.print(now.month());
  lcd.print("-");
  lcd.print(now.day());

  lcd.setCursor(0, 1);
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());
  lcd.print(" | ");
  lcd.print(temperature, 1);
  lcd.print(" C");

  // Allow setting date/time and alarm via Serial
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    if (input.startsWith("AL")) {
      setAlarm(input);
    } else {
      setDateTime(input);
    }
  }

  // Check alarm condition
  if (alarmHour == now.hour() && alarmMinute == now.minute() && !alarmTriggered && !alarmDisabled) {
    alarmTriggered = true;
    digitalWrite(buzzerPin, HIGH);
  }

  // Stop alarm and disable for the rest of the day when button is pressed
  if (alarmTriggered && digitalRead(buttonPin) == LOW) {
    alarmTriggered = false;
    alarmDisabled = true;
    digitalWrite(buzzerPin, LOW);
    lastDisableDay = now.day();
    //Debug
    Serial.println("Alarm stopped until tomorrow!");
  }

  delay(1000); // 1-second delay
}

void setDateTime(String datetime) {
  int year = datetime.substring(0, 4).toInt();
  int month = datetime.substring(5, 7).toInt();
  int day = datetime.substring(8, 10).toInt();
  int hour = datetime.substring(11, 13).toInt();
  int minute = datetime.substring(14, 16).toInt();
  int second = datetime.substring(17, 19).toInt();

  rtc.adjust(DateTime(year, month, day, hour, minute, second));

  //Debug
  Serial.println("Date and time updated!");
}

void setAlarm(String alarmTime) {
  alarmHour = alarmTime.substring(3, 5).toInt();
  alarmMinute = alarmTime.substring(6, 8).toInt();
  alarmTriggered = false;
  alarmDisabled = false;
  //Debug
  Serial.print("Alarm set for: ");
  Serial.print(alarmHour);
  Serial.print(":");
  Serial.println(alarmMinute);
}
