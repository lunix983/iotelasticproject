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
RH_RF95 rf95;

//If you use Dragino IoT Mesh Firmware, uncomment below lines.
//For product: LG01. 
#define BAUDRATE 115200



float frequency = 868.0;
String dataString = "iotdata=";
int sf = 12;
int bw = 250000;
int cr = 8;




struct message{
  int idnode;
  int sequencenum;
  unsigned long time;
  int snr;
  int rssi;
  float lat;
  float lon;
  float delay;
  String message; 
}data;




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
    if (rf95.available())// Listen Data from LoRa Node
    {
        Console.println("Wait for available LoRa Packet: ");
        uint8_t rx_buf[RH_RF95_MAX_MESSAGE_LEN];//receive data buffer
        uint8_t len = sizeof(rx_buf);//data buffer length
        if (rf95.recv(rx_buf, &len))//Check if there is incoming data
        {
           // recdata( buf, len);
            Console.print("Received LoRa Packet from nodeID: ");
            Console.println(data.idnode);
            
            memcpy(&data, rx_buf, sizeof(data));
            data.message = "ACK";
            memcpy(tx_buf, &data, sizeof(data) );                           
         }
         else
         {
              Console.println("recv failed");
         }
     }
              
}






