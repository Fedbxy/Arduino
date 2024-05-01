// LIBRARIES
#include "DHT.h"

// MACROS
#define print Serial.print
#define println Serial.println

// DEFINE
#define DHT_PIN 2
#define DHT_TYPE DHT22

// GLONAL VARIABLES
// bool indicator = false;

// FUNCTIONS
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();

  for (int led = 4; led <= 13; led++) {
    pinMode(led, OUTPUT);
  }
}

void loop() {
  delay(2000);

  double h = dht.readHumidity();
  double tc = dht.readTemperature();
  double tf = dht.readTemperature(true);

  if (isnan(h) or isnan(tc) or isnan(tf)) {
    println("Failed to read data from DHT sensor...");
    displayBinary(0);
    return;
  }

  // for (int i = 10; i<= 13; i++) {
  //   if (indicator) {
  //     digitalWrite(i, HIGH);
  //   } else {
  //     digitalWrite(i, LOW);
  //   }
  // }
  // indicator = !indicator;

  double hic = dht.computeHeatIndex(tc, h, false);
  double hif = dht.computeHeatIndex(tf, h);
  
  displaySerial(h, tc, tf, hic, hif);

  displayBinary(tc);
}

void displayBinary(double num) {
  bool integer[6];
  bool fraction[4];

  for (int i = 0; i < 6; i++) {
    integer[i] = int(num) & (1 << i);
  }

  for(int i = 0; i < 4; i++) {
    fraction[i] = int((num - int(num)) * 10) & (1 << i);
  }

  for (int i = 0; i < 6; i++) {
    if (integer[i]) {
      digitalWrite(9 - i, HIGH);
    } else {
      digitalWrite(9 - i, LOW);
    }
  }

  for (int i = 0; i < 4; i++) {
    if (!fraction[i]) {
      digitalWrite(13 - i, HIGH);
    } else {
      digitalWrite(13 - i, LOW);
    }
  }
}

void displaySerial(double h, double tc, double tf, double hic, double hif) {
  String category = "null";
  String risk = "null";

  if (hic >= 54) {
    category = "Extreme Danger";
    risk = "Heat stroke or sunstroke likely.";
  } else if (hic >= 41) {
    category = "Danger";
    risk = "Sunstroke, muscle cramps, and/or heat exhaustion likely. Heatstroke possible with prolonged exposure and/or physical activity.";
  } else if (hic >= 32) {
    category = "Extreme Caution";
    risk = "Sunstroke, muscle cramps, and/or heat exhaustion possible with prolonged exposure and/or physical activity.";
  } else if (hic >= 27) {
    category = "Caution";
    risk = "Fatigue possible with prolonged exposure and/or physical activity.";
  } else {
    category = "Normal";
    risk = "None.";
  }

  println();
  println();
  println();

  print("Humidity: ");
  print(h);
  println("%");

  print("Temperature: ");
  print(tc);
  print("°C (");
  print(tf);
  println("°F)");


  print("Heat Index: ");
  print(hic);
  print("°C (");
  print(hif);
  println("°F)");

  println();

  print("Heat Index Category: ");
  println(category);

  print("Category Risk: ");
  println(risk);
}