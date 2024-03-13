// ATmega328

const uint8_t LED_PIN = 7;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    char data = Serial.read();
    if (data == '1') {
      digitalWrite(LED_PIN, HIGH);
    } else if (data == '0') {
      digitalWrite(LED_PIN, LOW);
    }
  }
}