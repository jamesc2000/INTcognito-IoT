#define BLYNK_TEMPLATE_ID "TMPLohv5UbVV"
#define BLYNK_DEVICE_NAME "James ESP32"

#include <Arduino.h>

#define LED 2

void setup()
{
    pinMode(LED, OUTPUT);
}

void loop()
{
    delay(1000);
    Serial.println("On");
    digitalWrite(LED, HIGH);
    delay(1000);
    Serial.println("Off");
    digitalWrite(LED, LOW);
}