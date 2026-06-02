cat > src/main.cpp << 'EOF'
#include <Arduino.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <Adafruit_FT6206.h>

// WiFi
#define WIFI_SSID "Enfendav"
#define WIFI_PASS "rolex2019"

// RTL TCP
#define RTL_TCP_HOST "192.168.43.1"
#define RTL_TCP_PORT 14423

// Display pins
#define TFT_CS   10
#define TFT_DC   46
#define TFT_SCLK 12
#define TFT_MOSI 11
#define TFT_BL   45

TFT_eSPI tft = TFT_eSPI();
Adafruit_FT6206 touch = Adafruit_FT6206();
WiFiClient client;

float frequency = 433.92;
bool connected = false;

void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("RTL-SDR Controller");
  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.print("Freq: ");
  tft.print(frequency);
  tft.print(" MHz");
  
  tft.setCursor(10, 60);
  tft.print("Status: ");
  tft.print(connected ? "Connected" : "Disconnected");

  // Կոճակներ
  tft.fillRect(10, 100, 100, 40, TFT_BLUE);
  tft.setCursor(25, 115);
  tft.setTextColor(TFT_WHITE);
  tft.print("433 MHz");

  tft.fillRect(120, 100, 100, 40, TFT_BLUE);
  tft.setCursor(135, 115);
  tft.print("1090 MHz");

  tft.fillRect(10, 155, 100, 40, TFT_RED);
  tft.setCursor(30, 170);
  tft.print("FM 100");

  tft.fillRect(120, 155, 100, 40, TFT_DARKGREEN);
  tft.setCursor(130, 170);
  tft.print("Connect");
}

void setFrequency(float freq) {
  frequency = freq;
  if (client.connected()) {
    uint8_t cmd[5];
    uint32_t f = (uint32_t)(freq * 1e6);
    cmd[0] = 0x01;
    cmd[1] = (f >> 24) & 0xFF;
    cmd[2] = (f >> 16) & 0xFF;
    cmd[3] = (f >> 8) & 0xFF;
    cmd[4] = f & 0xFF;
    client.write(cmd, 5);
  }
  drawUI();
}

void connectRTL() {
  connected = client.connect(RTL_TCP_HOST, RTL_TCP_PORT);
  drawUI();
}

void setup() {
  Serial.begin(115200);

  // Backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // Display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 10);
  tft.print("Starting...");

  // Touch
  Wire.begin(16, 15);
  touch.begin(40);

  // WiFi
  tft.setCursor(10, 30);
  tft.print("WiFi connecting...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    tries++;
  }

  drawUI();
}

void loop() {
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    int x = p.x;
    int y = p.y;

    if (x > 10 && x < 110 && y > 100 && y < 140) {
      setFrequency(433.92);
    } else if (x > 120 && x < 220 && y > 100 && y < 140) {
      setFrequency(1090.0);
    } else if (x > 10 && x < 110 && y > 155 && y < 195) {
      setFrequency(100.0);
    } else if (x > 120 && x < 220 && y > 155 && y < 195) {
      connectRTL();
    }
    delay(200);
  }
}
EOF
