#include <SPI.h>
#include <RH_RF95.h>
#include <Console.h>
#include <Process.h>
RH_RF95 rf95;

//If you use Dragino IoT Mesh Firmware, uncomment below lines.
//For product: LG01. 
#define BAUDRATE 115200

uint16_t crcdata = 0;
uint16_t recCRCData = 0;
float frequency = 868.0;
String dataString = "";
String temp_send = "";
String umid_send = "";
void uploadData(); // Upload Data to tempIntPartingSpeak.
int sf = 12;
long bw = 250000;
int cr = 8;

void setup()
{
    Bridge.begin(BAUDRATE);
    Console.begin(); 
   // while(!Console);
    if (!rf95.init())
        Console.println("init failed");
    
    // Setup ISM frequency
    rf95.setFrequency(frequency);
    // Setup Power,dBm
    rf95.setTxPower(13);    
    rf95.setSpreadingFactor(sf);
    rf95.setSignalBandwidth(bw);
    rf95.setCodingRate4(cr);
    Console.println("LoRa Gateway Example  --");
    Console.println("    Upload Gps Coord and DHT values to LuixCloud");
}

void loop()
{
  Console.print("Waiting LoRa Packet ... ");
}

