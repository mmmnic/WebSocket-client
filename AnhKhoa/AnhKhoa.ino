#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <SerialCommands.h>
#include <ArduinoJson.h>

//include user_interface.h library to check free RAM on esp8266
extern "C" {
  #include "user_interface.h"
}

// Enter your wifi name and password here
const char* ssid = "            ";
const char* password = "khongphaicaido";

// Enter you websocket server host and port here 
char host[] = "192.168.1.9";  
int port = 8000;

// Create WebSocket client
WiFiClient client;

// Create Serial to communicate with NUC
const byte RX = D1;
const byte TX = D2;
SoftwareSerial mySerial = SoftwareSerial(RX, TX, false, 256); 

void setup()
{
  // put your setup code here, to run once:
  //--------------------------------------------------------
  // Turn on red LED on esp8266
  pinMode(LED_BUILTIN, OUTPUT);

  // create serial to communicate with IDE for debug
  Serial.begin(9600); 
  delay(50);

  //----------------- CONNECT TO WIFI ----------------------
  // Print wifi name which is connecting to
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  // Start connect to wifi
  WiFi.begin(ssid, password);

  // Waiting for connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }

  // Print esp8266's ip
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  //----------------- CONNECT TO WEBSOCKET ----------------------
  // Conect to webSocket server also check if fail
  while (!client.connect(host, port)) {
        Serial.println(F("Ket noi den socket server that bai!"));
  }
 
  // If connection's successed
  if (client.connected()) {
      // Print on IDE con
      Serial.println("Connected to Host");
      // Send esp8266's ip to webSocket server
      client.print(WiFi.localIP());
      client.println(" just connected");
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }


  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("Lost connection from server.");
    Serial.println("Reconnecting...");
    // Reconnect to WebSocket server
    while(!client.connect(host, port))
    {
      Serial.println("Reconnecting...");
    }
    // If Reconnect's successed
    if (client.connected()) {
      // Print on IDE con
      Serial.println("Connected to Host");
      // Send text to webSocket server
      client.print(WiFi.localIP());
      client.println(" just connected");
    }
  }
}
