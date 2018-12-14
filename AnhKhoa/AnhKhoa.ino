#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

// Nhap ID cua tay cam o day, 1 hoac 2
  const char* nuc_ID = "2";
  
// Nhap ten wifi va mat khau wifi o day
  const char* ssid = "UIT_Guest";
  const char* password = "1denmuoi1";

// Nhap dia chi va port cua server o day 
  char host[] = "14.186.70.185";
  int port = 3000;

// define string to send
   String s_head = "{\"ID\":\"";
   String s_middle = "\",\"BUTTON\":\"";
   String s_end = "\"}";
   
// RX is pin D5
#define RXpin D5
// TX is pin D6
#define TXpin D6
// Define button
#define UP '1'
#define DOWN '2'
#define LEFT '3'
#define RIGHT '4'
#define OK '5'
#define CANCEL '6'
#define RUNG_YEU 'r'
#define RUNG_MANH 'R'

// Create WebSocket client
WiFiClient client;

// Create Serial communicate with NUC
SoftwareSerial NUCSerial(RXpin, TXpin);
    
//------------------------ SETUP ---------------------------
void setup()
{
  // put your setup code here, to run once:
  // Turn on red LED on esp8266
  pinMode(LED_BUILTIN, OUTPUT);

  // create serial to communicate with IDE for debug
  Serial.begin(115200);
  NUCSerial.begin(115200);
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

  // Print esp8266's IP
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  //----------------- CONNECT TO WEBSOCKET ----------------------
  // Conect to webSocket server also check if fail
  while (!client.connect(host, port)) {
    Serial.println(F("Ket noi den socket server that bai!"));
  }

  // If connection's successed
  if (client.connected()) {
    Serial.println("Connected to Host");
  }
}

void loop()
{
  // Recieve data from server
  NucToServer();
  // Check if lost connection to server
  CheckServer();
  // Check if lost connectio to wifi
  CheckWifi();
}


// Decode string then encode to json object and send to Server
void NucToServer()
{
  String nuc_BUTTON = "";

  // check if receive data from NUC
  if (NUCSerial.available()>0)
  {
    // Read data from NUC
    char c = NUCSerial.read();
    /*
     * number 1 is up
     * number 2 is down
     * number 3 is left
     * number 4 is right
     * number 5 is ok
     * number 6 is cancel
     */
     switch (c)
     {
      case '1':
      {
        nuc_BUTTON = "UP";
        break;
      }
      case '2':
      {
        nuc_BUTTON = "DOWN";
        break;
      }
        case '3':
      {
        nuc_BUTTON = "LEFT";
        break;
      }
      case '4':
      {
        nuc_BUTTON = "RIGHT";
        break;
      }
      case '5':
      {
        nuc_BUTTON = "OK";
        break;
      }
      case '6':
      {
        nuc_BUTTON = "CANCEL";
        break;
      }
     }
  }
  
  // If button is not empty then send to server
  if (nuc_BUTTON != "")
  {
    // add head part to jsonString
    String jsonString = s_head;
    // add ID to jsonString 
    jsonString += nuc_ID;
    // add middle part
    jsonString += s_middle;
    // add BUTTON to jsonString
    jsonString += nuc_BUTTON;
    // add end to jsonString
    jsonString += s_end;
    
    // Send data to server
    client.print(jsonString);
  }
}

void CheckServer()
{
  // check server connection
  if (!client.connected())
  {
    // if lost connection
    Serial.println("Lost connection from server!");
    Serial.println("Reconnecting...");
    // Reconnect to WebSocket server
    while (!client.connect(host, port))
    {
      Serial.println("Reconnecting...");
    }
    // If Reconnect's successed
    if (client.connected()) {
      Serial.println("Connected to Host");
    }
  }
}


void CheckWifi()
{
  // check wifi connection
  if (WiFi.status() != WL_DISCONNECTED)
  {
      // Reconnect to wifi
      WiFi.begin(ssid, password);

      // Waiting for connected
      while (WiFi.status() != WL_CONNECTED)
      {
         delay(1000);
         Serial.println("Connecting...");
      }
      // Print esp8266's IP
      Serial.print("WiFi connected with IP: ");
      Serial.println(WiFi.localIP());
  }
}
