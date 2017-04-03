/** ESP-01 Compile as Wemos !!!!  
 * Goes to sleep after 30 seconds display
 * OLED Display for Wifi Voltmeter
 * Set the Network addAP(SSID,Pwd) and the voltmeter sender's url
 */

#include <ESP8266WiFi.h>
#include "SSD1306Brzo.h"
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
// Initialize the OLED display using brzo_i2c
SSD1306Brzo display(0x3c, D3, D4);  //(0x3c, SDA, SCL) Where D3=GPIO0, D4=GPIO2
ESP8266WiFiMulti WiFiMulti;

String swVersion = "1.04.03e";
String voltage = "0";
HTTPClient http;
String  url = "http://192.168.4.1/volts";
int counter = 1;
int cycles = 0;
String payload;

void setup() {
  Serial.begin(115200);
  WiFiMulti.addAP("SSID", "password");
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  Serial.println("Version: " +swVersion);
  Serial.println("");
   //Start screen
display.setFont(ArialMT_Plain_16);  
display.drawString(0, 0, "WiFi");
display.drawString(12, 20, "Voltmeter");
display.drawString(16, 40, swVersion);
display.display();
delay(1500);
display.clear();
}

void drawProgressBar() {
  int progress = (counter / 5) % 100;
  display.drawProgressBar(0, 44, 120, 10, progress);
}

void loop() {
  while (cycles < 3) { //4 = About 20 seconds then sleep
  //line = "";
  Serial.print("connecting to ");
  Serial.println(url);
  
 //Status bar 
 display.drawString(0, 0, "Attaching to :");
  display.setFont(ArialMT_Plain_10);
 display.drawString(0,18,String(url));
 //Draw progress bar
 while (counter < 1000) {
 drawProgressBar();
 counter++;
display.display();
delay(1);
 }
getvoltage(url);
      display.clear();
     display.setTextAlignment(TEXT_ALIGN_LEFT);
     display.setFont(ArialMT_Plain_16);
     display.drawString(0, 0, "Voltmeter");
     
     if (voltage != 0) {
     display.setFont(Courier_10_Pitch_32);
     display.drawString(5,22, payload+"v");        
     } else {
      display.setFont(ArialMT_Plain_16);
      display.drawString(5,22, "Searching..."); 
     }
   display.display();
  display.clear();
 delay(5000);
cycles++;
}

//Sleep until reset
Serial.println("Going to sleep");
  display.setFont(ArialMT_Plain_16);
 display.drawString(5,22, "Sleeping"); 
 display.display();
 delay(1000);
 display.clear();
 display.display();
 ESP.deepSleep(0); 
}

void getvoltage(String url) {
// digitalWrite(4,0);
  display.clear();
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {
   HTTPClient http;     
      http.begin(url); //HTTP
      int httpCode = http.GET();
      if(httpCode > 0) {
          if(httpCode == HTTP_CODE_OK) {
              payload = http.getString();
              Serial.println(payload);
// Get the payload          
   if (payload != "") {
   Serial.println(payload);
     } else {
      display.setFont(ArialMT_Plain_16);
      display.drawString(5,22, "Searching...");
     }       
      display.display();     
          }
        } else {
            Serial.println("[HTTP] GET... failed, error: \n");
        }
        http.end();
    }
}

