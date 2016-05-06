//
// Copyright 2015 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// FirebaseStream_ESP8266 is a sample that stream bitcoin price from a
// public Firebase and optionally display them on a OLED i2c screen.

#include <Firebase.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 3
Adafruit_SSD1306 display(OLED_RESET);

FirebaseStream stream;

// Set these to run example.
#define FIREBASE_HOST "example.firebaseio.com"
#define FIREBASE_AUTH "token_or_secret"
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

Firebase fbase = Firebase(FIREBASE_HOST, FIREBASE_AUTH);

void ConnectWifi(const char* ssid, const char* password = "") {
  if (password != "") {
    WiFi.begin(ssid, password);
  } else {
    WiFi.begin(ssid);
  }

  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  ConnectWifi(WIFI_SSID, WIFI_PASSWORD);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();

  stream = fbase.stream("/bitcoin/last");  
}


void loop() {
  if (stream.error()) {
    Serial.println("streaming error");
    Serial.println(stream.error().message());
  }
  
  if (stream.available()) {
     String event;
     auto type = stream.read(event);
     Serial.print("event: ");
     Serial.println(type);
     if (type == FirebaseStream::Event::PUT) {
       StaticJsonBuffer<200> buf;
       Serial.print("data: ");
       Serial.println(event);
       JsonObject& json = buf.parseObject((char*)event.c_str());
       String path = json["path"];
       float data = json["data"];

       // TODO(proppy): parse JSON object.
       display.clearDisplay();
       display.setTextSize(2);
       display.setTextColor(WHITE);
       display.setCursor(0,0);
       display.println(path.c_str()+1);
       display.println(data);
       display.display();
     }
  }   
}
