// ESP8266

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* SSID = "";
const char* PASSWORD = "";

const uint8_t LED_PIN = 7;
bool status_led = false;

ESP8266WebServer server(80);

const char* htmlContent = R"(
<!DOCTYPE html>
<html lang='en'>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>Arduino Web Server</title>
  <style>
    body { 
      font-family: Arial, sans-serif; 
      margin: 0; 
      padding: 0; 
      text-align: center; 
    }
    h1 { 
      color: #333; 
    }
    p { 
      font-size: 18px; 
    }
    .btn { 
      display: inline-block; 
      padding: 10px 20px;  
      color: white; 
      text-decoration: none; 
      font-size: 16px; 
      border-radius: 5px; 
      margin: 10px; 
    }
    .on {
      background-color: #4CAF50;
    }
    .off {
      background-color: #ff3b44; 
    }
  </style>
</head>
<body>
  <h1>LED Control</h1>
  <h2>Arduino ATmega328ESP8266</h2>
  <p>Status: %STATUS%</p>
  <p><a href="/%LINK%" class="btn %CLASS%">LED %BUTTON%</a></p>
</body>
</html>
)";

void setup() {
  Serial.begin(115200);
  initWiFi();
  server.begin();
  Serial.println("Server started");

  server.on("/", handleRootPage);
  server.on("/ledon", []() {
    status_led = true;
    digitalWrite(LED_PIN, HIGH);
    handleLEDPage();
  });
  server.on("/ledoff", []() {
    status_led = false;
    digitalWrite(LED_PIN, LOW);
    handleLEDPage();
  });

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  server.handleClient();
}

void initWiFi() {
  WiFi.begin(SSID, PASSWORD);
  WiFi.mode(WIFI_STA);

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\n");

  Serial.print("WiFi connected: ");
  Serial.println(SSID);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void handleRootPage() {
  String url = "/ledoff";
  server.sendHeader("Location", url, true);
  server.send(302);
}

void handleLEDPage() {
  String htmlRes = htmlContent;
  String statusText = status_led ? "On" : "Off";
  bool btn = !status_led;
  String link = btn ? "ledon" : "ledoff";
  String button = btn ? "On" : "Off";
  String btnClass = btn ? "on" : "off";
  htmlRes.replace("%STATUS%", statusText);
  htmlRes.replace("%LINK%", link);
  htmlRes.replace("%BUTTON%", button);
  htmlRes.replace("%CLASS%", btnClass);
  
  server.send(200, "text/html", htmlRes);
}