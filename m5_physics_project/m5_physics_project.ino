#include <ezButton.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include <TridentTD_LineNotify.h>


const uint8_t TOGGLE_LED_PIN = 12;
const uint8_t WARNING_PIN = 13;
const uint8_t HALL_SENSOR_PIN = 26;
const uint8_t TOGGLE_BUTTON_PIN = 25;

const uint8_t I2C_ADDRESS = 0x3c;
const uint16_t SCREEN_WIDTH = 128;
const uint16_t SCREEN_HEIGHT = 64;
const uint8_t OLED_RESET = -1;

const char* SSID = "";
const char* PASSWORD = "";
const char* LINE_TOKEN = "";


void displayText(String text, bool isCenter = false, unsigned int x = 0, unsigned int y = 0, bool isDark = false);


const uint8_t WARNING_DEFAULT = LOW;
const uint8_t HALL_SENSOR_DEFAULT = HIGH;
const bool isWarned_DEFAULT = false;
const bool isSensorChecked_DEFAULT = false;
const unsigned int triggerTime = 10;

bool ledState = LOW;
bool defaultValueSet = false;
bool currentSensorState = HALL_SENSOR_DEFAULT;
bool previousSensorState = !HALL_SENSOR_DEFAULT;
unsigned long startMillis = 0;
bool isWarned = isWarned_DEFAULT;
bool isSensorChecked = isSensorChecked_DEFAULT;


ezButton button(TOGGLE_BUTTON_PIN);
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(9600);
  Serial.println("Starting system...");

  display.begin(I2C_ADDRESS);
  display.clearDisplay();
  displayText("Starting system...");
  delay(500);

  pinMode(TOGGLE_LED_PIN, OUTPUT);
  pinMode(WARNING_PIN, OUTPUT);
  pinMode(HALL_SENSOR_PIN, INPUT);
  
  button.setDebounceTime(50);

  initWiFi();
  LINE.setToken(LINE_TOKEN);

  Serial.println("System started");
  display.clearDisplay();
  displayText("System started\n\nTurn on to continue");
}


void loop() {
  button.loop();
  
  if (button.isPressed()) {
    ledState = !ledState;
    digitalWrite(TOGGLE_LED_PIN, ledState);
    Serial.println("System Status: " + String((ledState ? "ON" : "OFF")));
    displayStatus();
  }

  if (ledState) {
    defaultValueSet = false;
    hallSensorHandler();
  } else if(!defaultValueSet) {
    defaultValueSet = true;

    digitalWrite(WARNING_PIN, WARNING_DEFAULT);
    currentSensorState = HALL_SENSOR_DEFAULT;
    previousSensorState = !HALL_SENSOR_DEFAULT;
    isWarned = isWarned_DEFAULT;
    isSensorChecked = isSensorChecked_DEFAULT;

    Serial.println("+ DEFAULT VALUE SET");
  }
}


void hallSensorHandler() {
  currentSensorState = digitalRead(HALL_SENSOR_PIN);

  if (currentSensorState != previousSensorState || !isSensorChecked) {
    if (currentSensorState) {
      Serial.println("- MAGNET NO LONGER DETECTED");
      displayStatus();
      startMillis = millis();
    } else {
      Serial.println("+ MAGNET DETECTED");
      displayStatus();
      digitalWrite(WARNING_PIN, LOW);
      isWarned = false;
    }

    if (!isSensorChecked) isSensorChecked = true;
    previousSensorState = currentSensorState;
  }

  if (!isWarned && currentSensorState && millis() - startMillis >= triggerTime * 1000) {
    Serial.println("- MAGNET NOT DETECTED LONGER THAN " + String(triggerTime) + " SECONDS");
    digitalWrite(WARNING_PIN, HIGH);
    LINE.notify("ปิดประตูด้วยจ้าาา😠😠");
    isWarned = true;
  }
}


void displayText(String text, bool isCenter, unsigned int x, unsigned int y, bool isDark) {
  if (isDark) display.setTextColor(SH110X_BLACK, SH110X_WHITE);
  else display.setTextColor(SH110X_WHITE);

  if (isCenter) display.setCursor((SCREEN_WIDTH - (text.length() * 6)) / 2, y);
  else display.setCursor(x, y);

  display.println(text);
  display.display();
}


void displayStatus() {
  display.clearDisplay();
  displayText("System Status: " + String((ledState ? "ON" : "OFF")));
  if (ledState) displayText("Door Status: " + String((currentSensorState ? "OPENED" : "CLOSED")), false, 0, 15);
}


void initWiFi() {
  Serial.print("Connecting WiFi...");
  display.clearDisplay();
  displayText("Connecting WiFi...");
  // WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\n");

  Serial.println("WiFi connected");
  display.clearDisplay();
  displayText("WiFi connected");
  delay(500);
}