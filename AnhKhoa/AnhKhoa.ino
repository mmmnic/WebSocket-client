#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

// Nhap ID cua tay cam o day, 1 hoac 2
  const char* nuc_ID = "1";
  
// Nhap ten wifi va mat khau wifi o day
const char* ssid = "TenWifi";
const char* password = "MatKhauWifi";

// Nhap dia chi va port cua server o day 
char host[] = "192.168.0.107";
int port = 8000;

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
    // Send esp8266's ip to webSocket server
    client.print(WiFi.localIP());
    client.println(" just connected");
  }
}

void loop()
{
  
  //Receive from server and send to NUC
  ServerToNuc();
  //Reveive from NUC and send to Server
  NucToServer();

  // if the server's disconnected
  if (!client.connected()) {
    Serial.println();
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
      client.print(WiFi.localIP());
      client.println(" just connected");
    }
  }
}


// Decode string then encode to json object and send to Server
void NucToServer()
{
  String nuc_BUTTON = "";
  
  if (NUCSerial.available()>0)
  {
    char c = NUCSerial.read();
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
    // Declare JSON buffer size
    StaticJsonBuffer<1023> jsonBuffer;

    // Create json object
    JsonObject& data = jsonBuffer.createObject();
    // assign data to json object
    data["ID"] = nuc_ID;
    data["BUTTON"] = nuc_BUTTON;

    // Send to server
    data.printTo(client);
   }
}

void ServerToNuc()
{
  String dataRecieve = "";
  // Convert byte received into string
  while (client.available()>0) {
    char c = client.read();
    dataRecieve += c;
  }

  // If string is not empty send to NUC
  if (dataRecieve != "")
  {
    // Declare JSON buffer size
    StaticJsonBuffer<1023> jsonBuffer;
    
    // Decode the data string
    JsonObject& data = jsonBuffer.parseObject(dataRecieve);

    if (data.success())
    {
      const char* receive_ID = data["ID"];
      const char* nuc_BUTTON = data["BUTTON"];
      if (strcmp(receive_ID,nuc_ID) == 0)
      {
          if (strcmp(nuc_BUTTON, "R") ==0)
            NUCSerial.println(RUNG_MANH);
          else if (strcmp(nuc_BUTTON, "r") ==0)
            NUCSerial.println(RUNG_YEU);
      }
    }
  }
}
