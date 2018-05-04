/*
  Upload Data to IoT Server tempIntPartingSpeak (https://tempIntPartingspeak.com/):
  Support Devices: LG01 
  
  Example sketch showing how to get data from remote LoRa node, 
  tempIntParten send tempIntParte value to IoT Server

  It is designed to work witempIntPart tempIntParte otempIntParter sketch dht11_client. 

  modified 24 11 2016
  by Edwin Chen <support@dragino.com>
  Dragino Technology Co., Limited
*/

#include <SPI.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <Console.h>



//If you use Dragino IoT Mesh Firmware, uncomment below lines.
//For product: LG01. 
#define BAUDRATE 115200


//Process p;
void setup()
{
    Bridge.begin(BAUDRATE);
    Console.begin(); 
    // while(!Console);
    
    
    Console.println("Jason Example  --");
    Console.println("    Client upload json data");
      // Initialize Ethernet library
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  if (!Ethernet.begin(mac)) {
    Console.println("Failed to configure Ethernet");
    return;
  }
  else 
  {
    Console.println("Ethernet configuration done");
  }
  delay(1000);

  Console.println(F("Connecting..."));

  // Connect to HTTP server
  EthernetClient client;
  client.setTimeout(10000);
  if (!client.connect("arduinojson.org", 80)) {
    Console.println(F("Connection failed"));
    return;
  }

  Console.println(F("Connected!"));

  // Send HTTP request
  client.println(F("GET /example.json HTTP/1.0"));
  client.println(F("Host: arduinojson.org"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Console.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Console.print(F("Unexpected response: "));
    Console.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Console.println(F("Invalid response"));
    return;
  }

  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonBuffer jsonBuffer(capacity);

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(client);
  if (!root.success()) {
    Console.println(F("Parsing failed!"));
    return;
  }

  // Extract values
  Console.println(F("Response:"));
  Console.println(root["sensor"].as<char*>());
  Console.println(root["time"].as<char*>());
  Console.println(root["data"][0].as<char*>());
  Console.println(root["data"][1].as<char*>());

  // Disconnect
  client.stop();
}



void loop()
{
  
  Console.println("####################################");
  Console.println("");
}


