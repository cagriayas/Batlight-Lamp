#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>


WebSocketsClient webSocket;

#define PIN D2
#define NUMPIXELS 60

#define USE_SERIAL Serial
#define deviceId 3
//Cartıllı's Home
String animationControl="";
int currentDevice = 0;
int first;
StaticJsonDocument<200> doc;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      Serial.println("Connected to Socket");
      emit("{\"deviceid\":\"2\",\"connection\":\"allive\"}");
    }
      break;
    case WStype_TEXT:
      String message=converter(payload);
      Control(message);
      break;
    }

}
void Control(String message){
      Serial.println(message);
      DeserializationError error = deserializeJson(doc, message);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      int device = doc["deviceid"];
      String animation = doc["animation"];
        if(device == deviceId){
          currentDevice = device;
          if(animation == "") {
            animationControl = "";
            byte R=doc["R"];
            byte G=doc["G"];
            byte B=doc["B"];
            setColor(R,G,B,0,NUMPIXELS);
        }
        else {
          animationControl = animation;  
        } 
      }
 
}
void emit(String data){
webSocket.sendTXT(data);
}
void connectToSocket(){
  webSocket.sendTXT("{\"deviceid\":\"1\",\"connection\":\"allive\"}");
}
String converter(uint8_t *str){
    return String((char *)str);
}

void setColor(byte R, byte G,byte B,int first, int last){
  for(int i=first;i<last;i++){
  pixels.setPixelColor(i,R,G,B);
  pixels.show();
  }
}


void setup() {
  USE_SERIAL.begin(9600);
  pixels.begin();
  USE_SERIAL.println();
  WiFi.begin("batuatalay", "1.(Batuatalay)");
  
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // server address, port and URL
  webSocket.begin("142.93.171.4", 3000);
  // event handler
  webSocket.onEvent(webSocketEvent);

//   try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(100, 3000, 2);
  first=0;
}

void loop() {
  if (first==0) {
    setColor(255,119,0,0,NUMPIXELS);
    first++;
    }
  webSocket.loop();
    if(animationControl=="2color"){
      byte fR=doc["fr"];
      byte fG=doc["fg"];
      byte fB=doc["fb"];
      
      byte sR=doc["sr"];
      byte sG=doc["sg"];
      byte sB=doc["sb"];
      setColor(fR, fG, fB, 0,NUMPIXELS/2);
      setColor(sR, sG, sB, NUMPIXELS/2, NUMPIXELS);
      while(true){
        webSocket.loop();
        if(animationControl == "" || animationControl != "2color") {
              break;
           }
      }
    }
    else if(animationControl == "rainbow") {
      int wait=20;
      for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
        webSocket.loop();
       if(animationControl == "" || animationControl != "rainbow") {
              break;
           }
        for(int i=0; i<pixels.numPixels(); i++) { 
          int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
          pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
        }
        pixels.show();
        delay(wait);
      }
    }
    else if(animationControl == "fallin") {
        while(true) {
          webSocket.loop();
          if(animationControl == "" || animationControl != "fallin") {
              break;
           }
          for(int i=NUMPIXELS+3;i>-6;i--){
            pixels.setPixelColor(i,255,0,0);
            pixels.show();
            pixels.setPixelColor(i+5,0,0,0);
            pixels.show();
            delay(10);
          }
          delay(1000);
        }
      }
     else if(animationControl == "redVibe") {
        int flag = 0;
        int delayTime = 30;
        while(true) {
            webSocket.loop();
            if(animationControl == "" || animationControl != "redVibe") {
                break;
             }
             if(flag == 0) {
                pixels.setPixelColor(NUMPIXELS/2,255,0,0);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,255,0,0);
                  pixels.setPixelColor(NUMPIXELS/2+i,255,0,0);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 1) {
                for(int i=0; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS-i,255,10,0);
                  pixels.setPixelColor(0+i,255,10,0);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 2) {
                pixels.setPixelColor(NUMPIXELS/2,255,50,0);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,255,50,0);
                  pixels.setPixelColor(NUMPIXELS/2+i,255,50,0);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 3) {
                for(int i=0; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS-i,255,0,10);
                  pixels.setPixelColor(0+i,255,0,10);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 4) {
                pixels.setPixelColor(NUMPIXELS/2,255,20,0);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,255,20,0);
                  pixels.setPixelColor(NUMPIXELS/2+i,255,20,0);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 5) {
                for(int i=0; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS-i,255,0,50);
                  pixels.setPixelColor(0+i,255,0,50);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 6) {
                pixels.setPixelColor(NUMPIXELS/2,255,10,10);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,255,10,10);
                  pixels.setPixelColor(NUMPIXELS/2+i,255,10,10);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 7) {
                for(int i=0; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS-i,255,10,50);
                  pixels.setPixelColor(0+i,255,10,50);
                  pixels.show();
                  delay(delayTime);
                  flag = 0;
                }
             }
          flag = flag + 1;
          webSocket.loop();
          if(animationControl == "" || animationControl != "redVibe") {
              break;
           }
          delay(5000);
           }
        }
    else if(animationControl == "partyVibe") {
        int flag = 0;
        int delayTime = 30;
        while(true) {
            webSocket.loop();
            if(animationControl == "" || animationControl != "partyVibe") {
                break;
             }
             if(flag == 0) {
                pixels.setPixelColor(NUMPIXELS/2,255,0,0);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,255,0,0);
                  pixels.setPixelColor(NUMPIXELS/2+i,255,0,0);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 1) {
                pixels.setPixelColor(NUMPIXELS/2,0,0,250);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,0,0,250);
                  pixels.setPixelColor(NUMPIXELS/2+i,0,0,250);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 2) {
                pixels.setPixelColor(NUMPIXELS/2,0,250,0);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,05,250,0);
                  pixels.setPixelColor(NUMPIXELS/2+i,0,250,0);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 3) {
                pixels.setPixelColor(NUMPIXELS/2,255,0,250);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,255,0,250);
                  pixels.setPixelColor(NUMPIXELS/2+i,255,0,250);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 4) {
                pixels.setPixelColor(NUMPIXELS/2,255,250,0);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,255,250,0);
                  pixels.setPixelColor(NUMPIXELS/2+i,255,250,0);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 5) {
                pixels.setPixelColor(NUMPIXELS/2,0,250,250);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,0,250,250);
                  pixels.setPixelColor(NUMPIXELS/2+i,0,250,250);
                  pixels.show();
                  delay(delayTime);
                }
             }
             else if(flag == 6) {
                pixels.setPixelColor(NUMPIXELS/2,255,250,210);
                pixels.show();
                for(int i=1; i<=NUMPIXELS/2+1; i++) {
                  pixels.setPixelColor(NUMPIXELS/2-i,255,250,210);
                  pixels.setPixelColor(NUMPIXELS/2+i,255,250,210);
                  pixels.show();
                  delay(delayTime);
                  flag = -1;
                }
             }
          flag = flag + 1;
          webSocket.loop();
          if(animationControl == "" || animationControl != "partyVibe") {
              break;
           }
           }
        }
}
