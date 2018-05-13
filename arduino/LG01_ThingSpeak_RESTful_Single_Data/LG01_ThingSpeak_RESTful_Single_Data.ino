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


uint16_t crcdata = 0;
uint16_t recCRCData = 0;
float frequency = 868.0;
String dataString = "iotdata=";
int sf = 12;
int bw = 250000;
int cr = 8;


void uploadData(); // Upload Data to ThingSpeak.

struct LoraPayLoad 
    {
      String idNode;
      String sequence;
      String snr;
      String rssi;
      String um;
      String temp;
      String lat;
      String lon;
    }loradata;


String* dataPtr = (String*)&loradata;

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
    
    Console.println("LoRa Gateway Example  --");
    Console.println("    Upload Single Data to ThinkSpeak");
}

uint16_t calcByte(uint16_t crc, uint8_t b)
{
    uint32_t i;
    crc = crc ^ (uint32_t)b << 8;
  
    for ( i = 0; i < 8; i++)
    {
      if ((crc & 0x8000) == 0x8000)
        crc = crc << 1 ^ 0x1021;
      else
        crc = crc << 1;
    }
    return crc & 0xffff;
}

uint16_t CRC16(uint8_t *pBuffer, uint32_t length)
{
    uint16_t wCRC16 = 0;
    uint32_t i;
    if (( pBuffer == 0 ) || ( length == 0 ))
    {
        return 0;
    }
    for ( i = 0; i < length; i++)
    {
        wCRC16 = calcByte(wCRC16, pBuffer[i]);
    }
    return wCRC16;
}

uint16_t recdata( unsigned char* recbuf, int Length)
{
    crcdata = CRC16(recbuf, Length - 2); //Get CRC code
    recCRCData = recbuf[Length - 1]; //Calculate CRC Data
    recCRCData = recCRCData << 8; //
    recCRCData |= recbuf[Length - 2];
}
void loop()
{   
    if (rf95.waitAvailableTimeout(2000))// Listen Data from LoRa Node
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];//receive data buffer
        uint8_t len = sizeof(buf);//data buffer length
        if (rf95.recv(buf, &len))//Check if there is incoming data
        {
            recdata( buf, len);
            Console.print("Get LoRa Packet: ");
            for (int i = 0; i < len; i++)
            {
                Console.print(buf[i],HEX);
                Console.print(" ");
            }
            Console.println();
            if(crcdata == recCRCData) //Check if CRC is correct
            { 
                
                    uint8_t data[] = "    GatewayDragino ACK";//Reply 
                    
                    rf95.send(data, sizeof(data));// Send Reply to LoRa Node
                    rf95.waitPacketSent();
        
                    loradata.idNode = String(buf[0]);            
                    
                    int snr; 
                    if (buf[2] == 1)
                    {
                      snr = buf[1] * -1;
                    }
                    else
                    {
                      snr = buf[1];
                    }
                    loradata.snr = snr;
                    //loradata.rssi = String(buf[3]);
                    int rssi;
                    if (buf[4] == 1)
                    {
                      rssi = buf[3] * -1;
                    }
                    else
                    {
                      rssi = buf[3];
                    }
                    //loradata.rssiSign = String(buf[4]);
                    loradata.rssi = rssi;
                    loradata.sequence = String(buf[5]);
                    loradata.um = cancactValue(buf[6],buf[7]);
                    loradata.temp = cancactValue(buf[8],buf[9]);
                    loradata.lat = concatGpsCoord(buf[10],buf[16],buf[15],buf[14],buf[13],buf[12],buf[11]);
                    loradata.lon = concatGpsCoord(buf[17],buf[23],buf[22],buf[21],buf[20],buf[19],buf[18]);
                    uploadData();
                    
            } 
            else 
              Console.println(" CRC Fail");     
         }
         else
         {
              Console.println("recv failed");
         }
     }

Console.print("UM: " );
Console.println(loradata.um);
Console.print("UM: " );
Console.println(loradata.um);
Console.print("TEMP: " );
Console.println(loradata.temp);
Console.print("LAT: " );
Console.println(loradata.lat);
Console.print("LON: " );
Console.println(loradata.lon);


//dataString=loradata.idNode;

}

String cancactValue(uint8_t intpart, uint8_t decpart){
  String data="";
  data=intpart;
  data+=".";
  data+=decpart;
  //return data.toFloat();
  return data;
}

String concatGpsCoord(uint8_t intpart, uint8_t decpart1, uint8_t decpart2,
                      uint8_t decpart3, uint8_t decpart4, uint8_t decpart5, uint8_t decpart6){
  String data="";
  data=intpart;
  data+=".";
  data+=decpart1;
  data+=decpart2;
  data+=decpart3;
  data+=decpart4;
  data+=decpart5;
  data+=decpart6;
  //return data.toFloat();
  return data;
}

String createCommand(String parameter)
{
  String echo = "echo ";
  String outFile = " >> /tmp/iotdata.txt";
  String command = "";
  command = echo;
  command += parameter;
  command += outFile;
  return command;
}


void uploadData() {

  Console.println("Call Linux Command to Send Data");  
      // Create a process and call it "p", this process will execute a Linux curl command
  Process p;
  
  int i;
  for (i=0; i<8; i++)
  {
   // Console.print("Struct data: ");
  //  Console.println(*(dataPtr + i));
    String command = createCommand(*(dataPtr + i));
    Console.println("COMMAND: ");
    Console.println(command);
    p.runShellCommand(command);
    p.close();
  }
  
  p.runShellCommand("/usr/bin/uploaddatasw.sh");
  p.close();
  //p.run();    // Run the process and wait for its termination
  p.runShellCommand("rm -f /tmp/iotdata.txt");
  p.close();


  Console.print("Feedback from Linux: ");
  // If there's output from Linux,
  // send it out the Console:
  while (p.available()>0) 
  {
    char c = p.read();
    Console.write(c);
  }
  p.close();
  Console.println("");
  Console.println("Call Finished");
  Console.println("####################################");
  Console.println("");
}

