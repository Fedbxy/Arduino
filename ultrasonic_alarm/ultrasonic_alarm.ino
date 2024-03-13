const uint8_t TRIG_PIN = 13;
const uint8_t ECHO_PIN = 12;
const uint8_t BUZZER_PIN = 9;
const uint8_t LED_PIN = 8;

int detect_cnt = 0;
int avg = 0;
unsigned long cnt = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration / 29.1 / 2;

  Serial.println("--------------------");
  Serial.print("DISTANCE: ");
  Serial.print(distance);
  Serial.println(" cm");

  cnt++;
  avg = (avg * (cnt - 1) + distance) / cnt;
  Serial.print("AVG: ");
  Serial.print(avg);
  Serial.println(" cm");

  Serial.print("Diff: ");
  Serial.print(abs(distance - avg));
  Serial.println(" cm");

  if (abs(distance - avg) >= 10) {
    if(detect_cnt < 10) detect_cnt++;
  } else if(detect_cnt > 0) detect_cnt--;

  if (detect_cnt >= 10) {
    warn();
    detect_cnt = 0;
    cnt = 0;
  }
}

void warn() {
  for(int i=0; i<100; i++) {
    tone(BUZZER_PIN, 500);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    tone(BUZZER_PIN, 1000);
    delay(100);
    tone(BUZZER_PIN, 1500);
    delay(100);
    tone(BUZZER_PIN, 2500);
    digitalWrite(LED_PIN, LOW);
    delay(100);
    tone(BUZZER_PIN, 1500);
    delay(100);
    tone(BUZZER_PIN, 1000);
    delay(100);
  }
  noTone(BUZZER_PIN);
}