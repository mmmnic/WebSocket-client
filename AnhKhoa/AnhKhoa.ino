#include <SerialCommand.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

// Enter your wifi name and password here
  const char* ssid = "UIT Public";
  const char* password = "";

// Enter you websocket server host and port here 
  char host[] = "10.45.95.227";  
  int port = 8000;

// Create WebSocket client
WiFiClient client;

// Create Serial to communicate with NUC
  const byte RX = 3; 
  const byte TX = 1;


 SoftwareSerial mySerial = SoftwareSerial(RX, TX, false, 256);
 SerialCommand sCmd(mySerial);

 //------------------------ SETUP ---------------------------
void setup()
{
  // put your setup code here, to run once:
  // Turn on red LED on esp8266
  pinMode(LED_BUILTIN, OUTPUT);

  // create serial to communicate with IDE for debug
  Serial.begin(9600); 
  mySerial.begin(9600);
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
      // Print on IDE
      Serial.println("Connected to Host");
      // Send esp8266's ip to webSocket server
      client.print(WiFi.localIP());
      client.println(" just connected");
  }

  // Catch signal from NUC
  sCmd.addDefaultHandler(SendToServer);
  Serial.println("Da san sang nhan lenh");
}

void loop()
{
  String data ="{\"ID\":\"3\",\"BUTTON\":\"LEFT\"}";
  SendToServer(data);
  delay(500);
  


  // if the server's disconnected
  if (!client.connected()) {
    Serial.println();
    Serial.println("Lost connection from server!");
    Serial.println("Reconnecting...");
    // Reconnect to WebSocket server
    while(!client.connect(host, port))
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

void SendToServer(String nucData)
{
      // Declare Json buffer size = 1024 MB
      StaticJsonBuffer<1023> jsonBuffer;

      // Decode the data string
      JsonObject& root = jsonBuffer.parseObject(nucData);
      // Redecode if decode fail
      while(!root.success()) {
      Serial.println("Decode data from NUC failed");
      JsonObject& root = jsonBuffer.parseObject(nucData);
      }
      const char* nuc_ID = root["ID"];
      const char* nuc_BUTTON = root["BUTTON"];

      // Create json object
      JsonObject& data = jsonBuffer.createObject();
      // assign data to json object
      data["ID"] = nuc_ID;
      data["BUTTON"] = nuc_BUTTON;

     // Send to server
     client.print("\r\n");
     data.printTo(client);
     
     // Generate the JSON string and print in IDE to debug
     Serial.println();
     data.printTo(Serial);
}
