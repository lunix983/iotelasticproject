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

//#include <SPI.h>
#include <RH_RF95.h>
#include <Console.h>
#include <Process.h>
#include <ArduinoJson.h>



RH_RF95 rf95;

//If you use Dragino IoT Mesh Firmware, uncomment below lines.
//For product: LG01. 
#define BAUDRATE 115200



float frequency = 868.0;
//String dataString = "iotdata=";
int sf = 12;
//int bw = 250000;
int cr = 8;
//Process process;
String uploadurl="http://83.212.126.194:50000/logstashmetricinput";
<<<<<<< HEAD
const char filename = "/tmp/iotjsondata.json";
=======
//const char *filename = "/tmp/iotdatajson.json";
uint8_t datasize;
Process process;
>>>>>>> 84e99cf3ef175f68866eca53d3fcc4aeefe58856

struct message{
  int idnode;
  unsigned long experimentid;
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
 
//byte* dataPtr = (byte*)&data;
struct message *dataPtr;

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
           
            Console.print("Received LoRa Packet from nodeID: "); Console.println(data.idnode);
            
            Console.print("data buffer length ");  Console.println(len);
            
            Console.print("data sizeof ");  Console.println(sizeof(data));  
   
            datasize = len;
           
                     
            memcpy(&data, rx_buf, sizeof(data));
            data.status = 1;     
            memcpy(tx_buf, &data, sizeof(data) );
            rf95.send((uint8_t *)tx_buf, sizeof(data));
            
            printData();
            uploaddata();
                          
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

String createCommand(String parameter)
{
<<<<<<< HEAD
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

=======
  String echo = "echo \"";
  String outFile = "\" >> /tmp/iotdata.txt";
  String command = "";
  command = echo;
  command += parameter;
  command += outFile;
 // Console.print("COMMAND RETURNED: ");  Console.println(command );
  return command;
}
>>>>>>> 84e99cf3ef175f68866eca53d3fcc4aeefe58856


void printData(){
  
  Console.print("MILLIS: ");  Console.println(data.time );
  Console.print("SEQUENCE: ");  Console.println(data.sequencenum );
  Console.print("LAT: ");  Console.println(data.lat,6);
  Console.print("LON: ");  Console.println(data.lon,6);
  Console.print("RSSI: ");  Console.println(data.rssi);
  Console.print("SNR: ");  Console.println(data.snr);  
  Console.print("DELAY: ");  Console.println(data.delay);
  Console.print("Experiment ID: ");  Console.println(data.experimentid);
  Console.print("LORA SETUP: ");  Console.println(data.lorasetup); 
  Console.print("DATA SIZE ");  Console.println(String(datasize));
  Console.println();
 
}


void uploaddata()
{ 

    process.runShellCommand("if [ -f /tmp/iotdata.txt ]; then rm -f /tmp/iotdata.txt; fi");
    process.runShellCommand(createCommand(String(data.idnode)));
    process.runShellCommand(createCommand(String(data.sequencenum)));
    process.runShellCommand(createCommand(String(data.snr)));
    process.runShellCommand(createCommand(String(data.rssi)));
    process.runShellCommand(createCommand(String(data.temp)));
    process.runShellCommand(createCommand(String(data.umidity)));
    process.runShellCommand(createCommand(String(data.lat,6)));
    process.runShellCommand(createCommand(String(data.lon,6)));
    process.runShellCommand(createCommand(String(data.delay)));
    process.runShellCommand(createCommand(String(data.lorasetup)));
    process.runShellCommand(createCommand(String(datasize)));
    process.runShellCommand(createCommand(String(data.experimentid)));
     
    
    int res = process.runShellCommand("uploaddatajson.sh");
    process.close();   
    Console.print("EXIT CODE : ");  Console.println(res);
      
}

