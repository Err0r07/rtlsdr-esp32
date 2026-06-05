#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("START");
  tft.begin();
  Serial.println("TFT begin done");
  tft.setRotation(0);
  tft.fillScreen(TFT_RED);
  Serial.println("RED done");
}

void loop() {
  tft.fillScreen(TFT_RED);
  delay(1000);
  tft.fillScreen(TFT_GREEN);
  delay(1000);
  tft.fillScreen(TFT_BLUE);
  delay(1000);
}
