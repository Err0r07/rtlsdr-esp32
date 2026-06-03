#include <Arduino.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <Adafruit_FT6206.h>

#define WIFI_SSID "Enfendav"
#define WIFI_PASS "rolex2019"
#define RTL_TCP_HOST "192.168.43.1"
#define RTL_TCP_PORT 14423

TFT_eSPI tft = TFT_eSPI();
Adafruit_FT6206 touch = Adafruit_FT6206();
WiFiClient client;

float frequency = 433.92;
bool isConnected = false;
char statusText[50] = "Disconnected";

void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.print("RTL-SDR");
  
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(20, 60);
  tft.print("Frequency:");
  tft.setCursor(20, 75);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(2);
  tft.print(frequency, 2);
  tft.print(" MHz");
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(20, 120);
  tft.print("Status: ");
  tft.print(statusText);
  
  tft.drawRect(20, 160, 90, 40, TFT_BLUE);
  tft.setCursor(30, 175);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print("433 MHz");
  
  tft.drawRect(130, 160, 90, 40, TFT_BLUE);
  tft.setCursor(140, 175);
  tft.print("1090 MHz");
  
  tft.drawRect(20, 220, 90, 40, TFT_RED);
  tft.setCursor(35, 235);
  tft.print("FM 100");
  
  tft.drawRect(130, 220, 90, 40, TFT_DARKGREEN);
  tft.setCursor(140, 235);
  tft.print("Connect");
}

void setFrequency(float freq) {
  frequency = freq;
  if (isConnected && client.connected()) {
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

void connectToRTL() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 100);
  tft.print("Connecting...");
  
  if (client.connect(RTL_TCP_HOST, RTL_TCP_PORT)) {
    isConnected = true;
    strcpy(statusText, "Connected");
  } else {
    isConnected = false;
    strcpy(statusText, "Failed");
  }
  delay(1000);
  drawUI();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(45, OUTPUT);
  digitalWrite(45, HIGH);
  
  SPI.begin(12, -1, 11, 10);
  
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 50);
  tft.print("Initializing...");
  
  Wire.begin(16, 15);
  if (!touch.begin(40)) {
    Serial.println("FT6206 not found");
  }
  
  Serial.println("WiFi connecting...");
  tft.setCursor(20, 100);
  tft.print("WiFi...");
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    attempts++;
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
  }
  
  delay(1000);
  drawUI();
}

void loop() {
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    int x = p.x;
    int y = p.y;
    
    Serial.print("Touch: ");
    Serial.print(x);
    Serial.print(" ");
    Serial.println(y);
    
    if (x >= 20 && x <= 110 && y >= 160 && y <= 200) {
      setFrequency(433.92);
    } 
    else if (x >= 130 && x <= 220 && y >= 160 && y <= 200) {
      setFrequency(1090.0);
    } 
    else if (x >= 20 && x <= 110 && y >= 220 && y <= 260) {
      setFrequency(100.0);
    } 
    else if (x >= 130 && x <= 220 && y >= 220 && y <= 260) {
      connectToRTL();
    }
    
    delay(300);
  }
  delay(10);
}
