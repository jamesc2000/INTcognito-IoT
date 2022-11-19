#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID "TMPLohv5UbVV"
#define BLYNK_DEVICE_NAME "Test"
#define BLYNK_AUTH_TOKEN "ni4uHKC-iMnNNLEq4aeQO0gDtsjpYzWq"

#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <Arduino.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "ni4uHKC-iMnNNLEq4aeQO0gDtsjpYzWq";

const char* host = "esp32";
char ssid[] = "HUAWEI-V4XU";
char pass[] = "Tataycruz";

WebServer server(80);

#include "otaLoginPage.h"

BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, millis() / (1000));
}

// Pin Assignments
constexpr int DOOR_SNS = 2;
constexpr int INDICATOR_LED = 33;

void setup() {
  // PIN Modes
  pinMode(DOOR_SNS, INPUT);
  pinMode(INDICATOR_LED, OUTPUT);
  
  Serial.begin(9600);

  // Connect to WiFi network
  WiFi.begin(ssid, pass);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();

  Blynk.config(auth);
  Blynk.connect(30);
  if (Blynk.connected()) {
    Serial.println("Successfully connected to Blynk");
  } else {
    Serial.println("Blynk connection request failed");
  }

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop() {
  server.handleClient();
  delay(1);
  if (Blynk.connected()) {
    Blynk.run();
    timer.run(); // Initiates BlynkTimer
  }

  if (digitalRead(DOOR_SNS) == 1) {
    digitalWrite(INDICATOR_LED, LOW); // Inverted logic for indicator led gpio 33
  } else {
    digitalWrite(INDICATOR_LED, HIGH);
  }
  
  //Serial.println("Message: ");
  //Serial.println(Serial.readString());
  // // Clears the trigPin condition
  // digitalWrite(trigPin, LOW);
  // delayMicroseconds(2);
  // // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  // digitalWrite(trigPin, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(trigPin, LOW);
  // // Reads the echoPin, returns the sound wave travel time in microseconds
  // duration = pulseIn(echoPin, HIGH);
  // // Calculating the distance
  // distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // // Displays the distance on the Serial Monitor
  // Serial.print("Distance: ");
  // Serial.print(distance);
  // Serial.println(" cm");
}


// TODO:
// Run the sensor at 3.3V
// MEASURE FIRST THE ECHO PIN!! MAKE SURE IT DOESNT GO 5V
