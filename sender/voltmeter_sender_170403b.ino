/* ESP8266 battery monitor 
Based on 192.168.1.188
Sets up as a Wireless Access Point
Sets the Name of the charger in EEPROM
*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
String softwareVersion = "v1.70403ap";
const char* AP_NameString = "Voltmeter4321";
const char* AP_Pass = "volts0000"; //PWD - Sets up as its own Access Point
String chargerName = ""; //Name of charger for logging
float chargerTime = 30 * 60000; // 10 * 60000 is 10 minutes
float vPow = 3.44; //Why not 3.3 ? Adjusted to match DMM
float r1 = 100000;
float r2 = 10000;
float v2 = 0;
IPAddress    apIP(192, 168, 4, 1); 
//Display elements
String l0 = "<html><head><style>h2{font-size:80px;font-weight:bold;}h3,label{font-size:35px;}input,select,button{font-size:25px;}a:link{text-decoration: none;}</style>";
String l1 = "<meta name='apple-mobile-web-app-capable' content='yes' /><meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />";
String l2 = "<meta http-equiv='refresh' content=\"5;URL='/'\" /></head> ";
String l3 = "<body><hr><hr><center><h2>Voltmeter</h2>";
String l3a = "<body><hr><hr><center><h3> Voltmeter - adjusting settings</h3>";
String l4 = "<h3>"+softwareVersion+"</h3><hr><hr>";
String l5 = "<h2>" + String(v2) +" volts</h2>";
String l8 = "<h3><a href='/name'><button>Name</button></a></h3>";
String l9 = "<h3><a href='/'><button>Cancel</button>";
String form1 ="";
String form3 = "";
String tail = "<hr><hr><h3><center>Footer text</center></h3></center>";
String disp = "";
// HTTP server will listen at port 80
ESP8266WebServer server(80);

void handle_root(){ //Webpage view for browsers or phones connected to this AP
 // get the value of request argument "trigger" and convert it to float
 float v = (analogRead(0) * vPow) / 1024.0;
 float v2 = v / (r2 / (r1 + r2));
 if (v2 < 0.25){  //Discard errant readings
    v2 = 0;
  }
  String l3 = "<body><hr><hr><center><h2>" + chargerName + " Voltmeter</h2>";
  String l4 = "<h3>"+softwareVersion+"</h3><hr><hr>";
 l5 = "<h2>" + String(v2) +" volts</h2>";

disp = l0 + l1 + l2 + l3 + l4 + l5 + l8 + tail;
server.send(200, "text/html", disp); 
} 
void handle_volts(){  //Shortform display for the OLED monitor (volts only)
  float v = (analogRead(0) * vPow) / 1024.0;
  float v2 = v / (r2 / (r1 + r2));
 l5 = String(v2);
 disp = l5;
  server.send(200, "text/html", disp); 
  }

void handle_name(){  //Can change the name of the Voltmeter in webpage views
form1 = "<h3><form action='/name2'><label>Set charger name (" + chargerName + "): </label><input name='chgid' maxlength='9'>";
form3 ="<input type='submit' value='Set'></form></h3>";
 disp = l0 + l1 + l3a + l4 +form1 +form3 +l9 +tail;
  server.send(200, "text/html", disp); 
  }

void handle_name2(){  //Save the new name into EEPROM
String chgid = server.arg("chgid");
 int charLength=chgid.length();
  chargerName=chgid;
  chgid = chgid.length() + chgid;
  Serial.println("----Writing eeprom chargerName:");
          for (int i = 0; i < chgid.length(); ++i)
            {
              EEPROM.write(i, chgid[i]);
              Serial.print("-----Wrote: ");
              Serial.println(chgid[i]);
            delay(200);
            }           
            EEPROM.commit();
  String  disp = l0 + l1 + l2 + l3a + tail;
 server.send(200, "text/html", disp); 
}


void setup() {
  Serial.begin(115200);
  //////
WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00
  WiFi.softAP(AP_NameString, AP_Pass);
  ///
  //setupWiFi();
  server.begin();
  Serial.println("");
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.print("IP address: ");
  Serial.println(apIP);
  float vdd = ESP.getVcc() / 1000.0; 

   Serial.println("Server started");
   Serial.println("Version: "+softwareVersion);
   
   Serial.println("--------------------");
   Serial.println("DC VOLTMETER");
   Serial.print("Maximum Voltage: ");
   Serial.print((int)(vPow / (r2 / (r1 + r2))));
   Serial.println("V");
   Serial.println("--------------------");
   Serial.println("");
  delay(100);
  // Read the chargerName from EEPROM
  EEPROM.begin(512);
  Serial.println("Reading EEPROM chgid");
  String chgid;
  char chglen = char(EEPROM.read(0));
  int chglen1 = int(chglen-47);
  Serial.print("Length of the chargerName string: "); //Includes leading integer (length of the text string)
  Serial.println(chglen1);
      for (int i = 1; i < chglen1; ++i)
      { chgid += char(EEPROM.read(i)); }
  Serial.print("chargerName is: "); //Got the name
  Serial.println(chgid);
  chargerName = chgid;
  // And as regular external function:
  server.on("/", handle_root);
    server.on("/volts", handle_volts);
  server.on("/name", handle_name);
  server.on("/name2", handle_name2);
  // Start the server 
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void) {
 float v = (analogRead(0) * vPow) / 1024.0;
 float v2 = v / (r2 / (r1 + r2));
 checkvolts(v2);
  //Get the relay status  optional charger relay control goes here
  
  // check for incomming client connections frequently in the main loop:
  server.handleClient();
}
  
int checkvolts(float v2)  {     
    
    Serial.print("Voltage now: ");
    Serial.println(v2);

    delay(3000);
   }
   
 
