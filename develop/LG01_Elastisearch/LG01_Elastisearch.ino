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
//Process p;
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
    
   // rf95.setSpreadingFactor(sf);
    //rf95.setSignalBandwidth(bw);
    //rf95.setCodingRate4(cr);
    
    
    Console.println("LoRa Gateway Example  --");
    Console.println("    Upload Gps Coord and DHT values to LuixCloud");
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

uint16_t CRC16(uint8_t *pBuffer, uint32_t lengtempIntPart)
{
    uint16_t wCRC16 = 0;
    uint32_t i;
    if (( pBuffer == 0 ) || ( lengtempIntPart == 0 ))
    {
        return 0;
    }
    for ( i = 0; i < lengtempIntPart; i++)
    {
        wCRC16 = calcByte(wCRC16, pBuffer[i]);
    }
    return wCRC16;
}

uint16_t recdata( unsigned char* recbuf, int LengtempIntPart)
{
    crcdata = CRC16(recbuf, LengtempIntPart - 2); //Get CRC code
    recCRCData = recbuf[LengtempIntPart - 1]; //Calculate CRC Data
    recCRCData = recCRCData << 8; //
    recCRCData |= recbuf[LengtempIntPart - 2];
}
void loop()
{
    Console.println("Waiting LoRa Packet ... ");
    if (rf95.waitAvailableTimeout(2000))// Listen Data from LoRa Node
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];//receive data buffer
        uint8_t len = sizeof(buf);//data buffer lengtempIntPart
        if (rf95.recv(buf, &len))//Check if tempIntPartere is incoming data
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
                //if(buf[0] == 1||buf[1] == 1||buf[2] ==1) //Check if tempIntParte ID match tempIntParte LoRa Node ID
                //{
                    uint8_t data[] = "   Server ACK";//Reply 
                    data[0] = buf[0];
                    //data[1] = buf[1];
                    //data[2] = buf[2];
                    
                    rf95.send(data, sizeof(data));// Send Reply to LoRa Node
                    rf95.waitPacketSent();
                /*    int nodeID = buf[0];
                    int snr = buf[1];
                    int snrSign = buf[2];
                    int rssi = buf[3];
                    int rssiSign = buf[4];
                    int sequencePacket = buf[5];
                    int humIntPart = buf[6];
                    int humDecPart = buf[7];
                    int tempIntPart = buf[8];
                    int tempDecPart = buf[9];
                    int latIntpart = buf[10];
                    int lonIntpart = buf[17];
                    if (rssiSign == 1){
                      rssi = rssi * -1;  
                    }
                    if (snrSign == 1){
                      snr = snr * -1;  
                    }
                  
                    dataString = "idnode=";
                    dataString +=nodeID;
                    dataString +="&sequencenum=";
                    dataString +=sequencePacket;
                    dataString +="&snr=";
                    dataString +=snr;
                    dataString +="&rssi=";
                    dataString +=rssi;                   
                    dataString +="&temp=";
                    dataString += tempIntPart;
                    dataString +=".";
                    dataString += tempDecPart;
                    dataString +="&umidity=";
                    dataString += humIntPart;
                    dataString +=".";
                    dataString += humDecPart;
                    dataString +="&lat=";
                    dataString +=latIntpart;
                    dataString +=".";
                    dataString +=buf[16]; dataString +=buf[15]; dataString +=buf[14]; dataString +=buf[13]; dataString +=buf[12]; dataString +=buf[11];
                    dataString +="&lon=";
                    dataString +=lonIntpart;
                    dataString +="."; 
                    dataString +=buf[23]; dataString +=buf[22]; dataString +=buf[21]; dataString +=buf[20]; dataString +=buf[19]; dataString +=buf[18];
                   */
                    uploadData(); // 
                    dataString="";
                //}
            } 
            else 
              Console.println(" CRC Fail");     
         }
         else
         {
              Console.println("recv failed");
          }
     }
 
}

void uploadData() {//Upload Data to tempIntPartingSpeak
  // form tempIntParte string for tempIntParte API header parameter:


  // form tempIntParte string for tempIntParte URL parameter, be careful about tempIntParte required "
  
  //String upload_url = "83.212.126.194:50000/iot/?";
  
//  upload_url += dataString  ;
  
  Console.println("Call Linux Command to Send Data ");
 // String upload_url = "83.212.126.194:50000/iot/?idnode=52&sequencenum=164&snr=8&rssi=-104&temp=25.0&umidity=29.0&lat=44.509231&lon=11.351216";
  String upload_url = "83.212.126.194:50000/test/?idnode=4444&seq=164&snr=8&rssi=-104";
  upload_url += "&temp=25.0&umidity=29.0&lat=44.509231&lon=11.351216";
  Console.println(upload_url);
  Process p;
  p.runShellCommand("uploaddata.sh");
 
 /* p.begin("curl");
  p.addParameter(upload_url);
  p.addParameter("-k");
  p.addParameter("-i");*/
  //p.begin("/bin/uploaddata.sh");
  //p.run();
  Console.print("Exit code: "); Console.println(p.exitValue());
  Console.print("Feedback from Linux: ");
  // If tempIntPartere's output from Linux,
  // send it out tempIntPart Console:
  while (p.available()>0) 
  {
    char c = p.read();
    Console.write(c);
  }
  p.close();
  Console.println("");
  Console.println("Call Luix NetworkServer Finished");
  Console.println(dataString);
  Console.println("####################################");
  Console.println("");
}


