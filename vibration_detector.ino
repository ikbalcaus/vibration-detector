#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266Firebase.h>
#include <Arduino.h>
#include <Hash.h>
#include <math.h>
#include "web_page.h"

Adafruit_MPU6050 mpu;
const char* ssid = "";
const char* pass = "";
ESP8266WebServer server(80);
Firebase firebase("");
int delayTime = 200;
float x = 12.5, y = 0, z = 0, diffX, diffY, diffZ, total;
bool enableFirebase = false, measureMode = true;

void setup(void) {
  pinMode(D0, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  Serial.begin(115200);

  if(!mpu.begin()) {
    Serial.println("");
    Serial.print("Failed to find MPU6050 chip!");
    while (true) {
      delay(10);
    }
  }
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);

  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi Connected! ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/html", indexHTML);
  });

  server.on("/power", []() {
    server.send(200, "text/plain", String(total).c_str());
  });

  server.on("/measuremode", []() {
    measureMode = true;
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plane", ""); 
  });

  server.on("/blinkmode", []() {
    measureMode = false;
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plane", ""); 
  });

  server.begin();
  Serial.println("HTTP Server Started!");

  delay(100);
}

void loop() {
  server.handleClient();

  total = 0;
  digitalWrite(D0, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  delayTime = 200;

  if(mpu.getMotionInterruptStatus()) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    diffX = abs(a.acceleration.x - x);
    diffY = abs(a.acceleration.y - y);
    diffZ = abs(a.acceleration.z - z);
    total = diffX + diffY + diffZ;
    x = a.acceleration.x;
    y = a.acceleration.y;
    z = a.acceleration.z;

    if(measureMode) {
      delayTime = 200;
      digitalWrite(D0, HIGH);
      if(total > 0.5) digitalWrite(D3, HIGH);
      if(total > 0.9) digitalWrite(D4, HIGH);
      if(total > 1.4) digitalWrite(D5, HIGH);
      if(total > 2) digitalWrite(D6, HIGH);
    }
    else {
      delayTime = 500;
      if(total > 0.5) delayTime = 400;
      if(total > 0.9) delayTime = 300;
      if(total > 1.4) delayTime = 200;
      if(total > 2) delayTime = 100;
      delay(delayTime);
      digitalWrite(D0, HIGH);
      digitalWrite(D3, HIGH);
      digitalWrite(D4, HIGH);
      digitalWrite(D5, HIGH);
      digitalWrite(D6, HIGH);
    }
  }

  if(enableFirebase) firebase.pushFloat("total", total);
  delay(delayTime);
}
