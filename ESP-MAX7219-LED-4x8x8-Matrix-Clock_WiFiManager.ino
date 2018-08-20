#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiManager.h> 
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <time.h>

int pinCS = D4; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays   = 1;
char time_value[20];

// LED Matrix Pin -> ESP8266 Pin
// Vcc            -> 3v  (3V on NodeMCU 3V3 on WEMOS)
// Gnd            -> Gnd (G on NodeMCU)
// DIN            -> D7  (Same Pin for WEMOS)
// CS             -> D4  (Same Pin for WEMOS)
// CLK            -> D5  (Same Pin for WEMOS)

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

int wait = 70; // In milliseconds

int spacer = 1;
int width  = 5 + spacer; // The font width is 5 pixels

void setup() {
  Serial.begin(115200);
  WiFi_Setup();
  configTime(0 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02",1);
  matrix.setIntensity(0); // Use a value between 0 and 15 for brightness
  matrix.setRotation(0, 1);    // The first display is position upside down
  matrix.setRotation(1, 1);    // The first display is position upside down
  matrix.setRotation(2, 1);    // The first display is position upside down
  matrix.setRotation(3, 1);    // The first display is position upside down
}

void loop() {
  matrix.fillScreen(LOW);
  time_t now = time(nullptr);
  String time = String(ctime(&now));
  time.trim();
  Serial.println(time);
  time.substring(11,19).toCharArray(time_value, 10); 
  matrix.drawChar(2,0, time_value[0], HIGH,LOW,1); // H
  matrix.drawChar(8,0, time_value[1], HIGH,LOW,1); // HH
  matrix.drawChar(14,0,time_value[2], HIGH,LOW,1); // HH:
  matrix.drawChar(20,0,time_value[3], HIGH,LOW,1); // HH:M
  matrix.drawChar(26,0,time_value[4], HIGH,LOW,1); // HH:MM
  matrix.write(); // Send bitmap to display
  delay(2000);
  display_message(time); // Display time in format 'Wed, Mar 01 16:03:20 2017
}

void display_message(String message){
   for ( int i = 0 ; i < width * message.length() + matrix.width() - spacer; i++ ) {
    //matrix.fillScreen(LOW);
    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically
    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < message.length() ) {
        matrix.drawChar(x, y, message[letter], HIGH, LOW, 1); // HIGH LOW means foreground ON, background off, reverse to invert the image
      }
      letter--;
      x -= width;
    }
    matrix.write(); // Send bitmap to display
    delay(wait/2);
  }
}

void WiFi_Setup(){
  WiFiManager wifiManager; // Connect to Wi-Fi 
  // A new OOB ESP has no Wi-Fi credentials so will connect and not need the next command to be uncommented and compiled in, a used one with incorrect credentials will 
  // so restart the ESP and connect your PC to the wireless access point called 'ESP_AP' or whatever you call it below in "" 
  // wifiManager.resetSettings(); // Command to be included if needed, then connect to http://192.168.4.1/ and follow instructions to make the WiFi connection 
  // Set a timeout until configuration is turned off, useful to retry or go to sleep in n-seconds 
  wifiManager.setTimeout(180); 
  //fetches ssid and password and tries to connect, if connections succeeds it starts an access point with the name called "ESP8266_AP" and waits in a blocking loop for configuration 
  if(!wifiManager.autoConnect("ESP_AP")) { 
    Serial.println(F("failed to connect and timeout occurred")); 
    delay(6000); 
    ESP.restart(); //reset and try again 
  } 
  // At this stage the WiFi manager will have successfully connected to a network, or if not will try again in 180-seconds 
  Serial.println(F("WiFi connected...")); 
  //---------------------------------------------------------------------- 
  Serial.println(F("Use this URL to connect: http://")); Serial.println(WiFi.localIP().toString()+"/");// Print the IP address 
}

