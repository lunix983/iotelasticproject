#include <Console.h>
#include <Process.h>
#include <RH_RF95.h>
#define BAUDRATE 115200


RH_RF95 rf95;
float frequency = 868.0;
int sf = 12;
long bw = 250000;
int cr = 8;
uint16_t crcdata = 0;
uint16_t recCRCData = 0;

void setup (){
  Bridge.begin(BAUDRATE);
  Console.begin();
   if (!rf95.init())
        Console.println("init failed");
   // Setup ISM frequency
   rf95.setFrequency(frequency);
   // Setup Power,dBm
   rf95.setTxPower(13);    
   rf95.setSpreadingFactor(sf);
   rf95.setSignalBandwidth(bw);
   rf95.setCodingRate4(cr);
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

void loop(){
  Console.println("Hello world");

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
              uint8_t data[] = "Server ACK";//Reply 
               rf95.send(data, sizeof(data));// Send Reply to LoRa Node
               rf95.waitPacketSent();
            }
            else{
              Serial.println("CRC fail ...");
            }
      }
   }

  
  Process p;
  String uploadurl = "83.212.126.194:50000/iot/?idnode=52&sequencenum=164&snr=8&rssi=-104&temp=25.0&umidity=29.0&lat=44.509231&lon=11.351216";
  Console.println(uploadurl);
  p.begin("curl");
  p.addParameter("-k");
  p.addParameter("-i");
  p.addParameter(uploadurl);
  p.run();
  delay(3000);
  Console.print("Exit code: "); Console.println(p.exitValue());
  while (p.available()>0){
    char c = p.read();
    Console.print(c);
  }
}

