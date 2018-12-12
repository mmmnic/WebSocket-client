#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

// Enter your wifi name and password here
const char* ssid = "UIT_Guest";
const char* password = "1denmuoi1";

// Enter you websocket server host and port here
char host[] = "192.168.0.107";
int port = 8000;

// Create WebSocket client
WiFiClient client;


//------------------------ SETUP ---------------------------
void setup()
{
  // put your setup code here, to run once:
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

  // Nani Delay???
  delay(200);

 
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
  String dataRecieve = "";

  // Convert byte received into string
  while (Serial.available()>0) {
    char c = Serial.read();
    if (c=='*')
    {
      break;
    }
    dataRecieve += c;
  }

  // If string is not empty send to server
  if (dataRecieve!="")
  {
    // Declare Json buffer size = 1024 MB
    StaticJsonBuffer<1023> jsonBuffer;

    // Decode the data string
    JsonObject& root = jsonBuffer.parseObject(dataRecieve);
    // Redecode if decode fail
    if (root.success())
    {
      const char* nuc_ID = root["ID"];
      const char* nuc_BUTTON = root["BUTTON"];

      // Create json object
      JsonObject& data = jsonBuffer.createObject();
      // assign data to json object
      data["ID"] = nuc_ID;
      data["BUTTON"] = nuc_BUTTON;

      // Send to selerver
      data.printTo(client);
    }
  }
}

void ServerToNuc()
{
  String dataRecieve = "";
  // Convert byte received into string
  while (client.available()>0) {
    char c = client.read();
    if (c=='*')
    {
      break;
    }
    dataRecieve += c;
  }

  // If string is not empty send to server
  if (dataRecieve!="")
  {
    // Declare Json buffer size = 1024 MB
    StaticJsonBuffer<1023> jsonBuffer;

    // Decode the data string
    JsonObject& root = jsonBuffer.parseObject(dataRecieve);
    // Redecode if decode fail
    if (root.success())
    {
      const char* nuc_ID = root["ID"];
      const char* nuc_BUTTON = root["BUTTON"];

      // Create json object
      JsonObject& data = jsonBuffer.createObject();
      // assign data to json object
      data["ID"] = nuc_ID;
      data["BUTTON"] = nuc_BUTTON;

      // Send to selerver
      data.printTo(Serial);
      Serial.print('*');
    }
  }
}
