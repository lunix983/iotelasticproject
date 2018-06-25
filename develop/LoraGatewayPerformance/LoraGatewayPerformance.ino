/*
  Upload Data to IoT Server ThingSpeak (https://thingspeak.com/):
  Support Devices: LG01 
  
  Example sketch showing how to get data from remote LoRa node, 
  Then send the value to IoT Server

  It is designed to work with the other sketch dht11_client. 

  modified 24 11 2016
  by Edwin Chen <support@dragino.com>
  Dragino Technology Co., Limited
*/

#include <SPI.h>
#include <RH_RF95.h>
#include <Console.h>
#include <Process.h>
#include <ArduinoJson.h>

RH_RF95 rf95;

//If you use Dragino IoT Mesh Firmware, uncomment below lines.
//For product: LG01. 
#define BAUDRATE 115200



float frequency = 868.0;
String dataString = "iotdata=";
int sf = 12;
//int bw = 250000;
int cr = 8;
Process process;
String uploadurl="http://83.212.126.194:50000/logstashmetricinput";
const char filename = "/tmp/iotjsondata.json";

struct message{
  int idnode;
  int sequencenum;
  unsigned long time;
  int snr;
  int rssi;
  float lat;
  float lon;
  float delay;
  int status;
  float umidity;
  float temp;
  int lorasetup;
}data;

//JsonObject& root;


byte tx_buf[sizeof(data)] = {0};

void setup()
{
    Bridge.begin(BAUDRATE);
    Console.begin(); 
    // while(!Console);
    if (!rf95.init())
        Console.println("init failed");
    ;
    // Setup ISM frequency
    rf95.setFrequency(frequency);
    // Setup Power,dBm
    rf95.setTxPower(13);
   // rf95.setSpreadingFactor(sf);
  //  rf95.setSignalBandwidth(bw);
//    rf95.setCodingRate(cr);
    
    Console.println("LoRa Gateway Perfomance  --");
    Console.println(" Luix Progetto tesi");
   
}


void loop()
{   
    Console.println("Wait for available LoRa Packet: ");
    if (rf95.waitAvailableTimeout(2000))
   // if (rf95.available())// Listen Data from LoRa Node
    {
        
        uint8_t rx_buf[RH_RF95_MAX_MESSAGE_LEN];//receive data buffer
        uint8_t len = sizeof(rx_buf);//data buffer length
        if (rf95.recv(rx_buf, &len))//Check if there is incoming data
        {
           // recdata( buf, len);
            Console.print("Received LoRa Packet from nodeID: ");
            Console.println(data.idnode);
            
            memcpy(&data, rx_buf, sizeof(data));
            data.status = 1;
            Console.println("RSSI received: "); Console.println(data.rssi);
            memcpy(tx_buf, &data, sizeof(data) );
            rf95.send((uint8_t *)tx_buf, sizeof(data));
            uploaddata();
            //test();
                                       
         }
         else
         {
              Console.println("recv failed");
         }
     }
              
}


void test()
{
  Console.println("TEST");
}

void uploaddata()
{
    Console.println("UPLOADDATA: ");
    /*
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["idnode"] = data.idnode;
    root["sequencenum"] = data.sequencenum;
    root["snr"] = data.snr;
    root["rssi"] = data.rssi;
    root["temperature"] = data.temp;
    root["umidity"] = data.umidity;
    root["latitude"] = data.lat;
    root["longitude"] = data.lon;
    root["delay"] = data.delay;
    */
    
   // root.printTo(Console);
   /*
    String jsonstring="'{";
    root.printTo(jsonstring);
    
   
    process.begin( "curl" );
    process.addParameter( "-k" );
    process.addParameter( "-i" );
    process.addParameter( "-X" );
    process.addParameter( "POST" );
    process.addParameter( "-H" );
    process.addParameter( "Content-Type:application/json" );
    process.addParameter( "-d" );
    jsonstring += "\"idnone\":";
    jsonstring += data.idnode;
    jsonstring += ",\"sequencenum\":";
    jsonstring += data.sequencenum;
    jsonstring += ",\"snr\":";
    jsonstring += data.snr;
    jsonstring += ",\"rssi\":";
    jsonstring += data.rssi;
    jsonstring += ",\"temperature\":";
    jsonstring += data.temp;
    jsonstring += ",\"umidity\":";
    jsonstring += data.umidity;
    jsonstring += ",\"latitude\":";
    jsonstring += data.lat;
    jsonstring += ",\"longitude\":";
    jsonstring += data.lon;
    jsonstring += ",\"delay\":";
    jsonstring += data.delay;
    jsonstring += "'}";*/
   // Console.println(jsonstring);
    Console.println();





    
//  process.addParameter(root);



    
    /*
    root[""] = data. ;
    root[""] = data. ;
    root[""] = data. ;*/
//  String command = "curl -i -H \"Content-Type: application/json\" -X POST -d '{\"idnode\":"data.idnode",\"sequencenum\":"data.sequencenum",\"snr\":"data.snr",\"rssi\":"data.rssi",\"temperature\":"data.temperature",\"umidity\":"data.umidity",\"latitude\":"data.latitude",\"longitude\":"data.logitude",\"delay\":"data.delay"}'";
  
}

//p.runShellCommand("curl -k -X POST https://xxx.firebaseio.com/data.json -d    '{ \"Timestamp\" : "timer",\"Humidity\" : " + String(humidity) + ",\"Temperature\"  : " + String(temperature) + ",\"Lightlevel\" : " + String(lightLevel) + "}'");

// PAYLOAD=\'{\"idnode\":\"$IDNODE\",\"sequencenum\":\"$SEQUENCENUM\",\"snr\":\"$SNR\",\"rssi\":\"$RSSI\",\"temperature\":\"$TEMPERATURE\",\"umidity\":$UMIDITY,\"latitude\":\"$LATITUDE\",\"longitude\":\"$LONGITUDE\"}\'

