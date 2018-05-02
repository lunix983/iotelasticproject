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

String myWriteAPIString = "0QKMFZUSWTUHEXYQ";
uint16_t crcdata = 0;
uint16_t recCRCData = 0;
float frequency = 868.0;
String dataString = "iotdata=";
int sf = 12;
int bw = 250000;
int cr = 8;


void uploadData(); // Upload Data to ThingSpeak.

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
                    uint8_t i;
                    /*
                    for (i=0; i<24; i++)
                    {
                      Console.print(buf[i]);
                      dataString += buf[i];
                      
                         /* if (i==0)
                          {
                            dataString = "idnode=";
                            dataString += buf[i];
                          }
                          if (i==1)
                          {
                            dataString += "&sequencenum=";
                            dataString += buf[i];
                          }
                          if (i==2)
                          {
                            dataString += "&snr=";
                            dataString += buf[i];
                          }
                          if (i==3)
                          {
                            dataString += "&rssi=";
                            dataString += buf[i];
                          }*/
                          
                          /*
                          switch (i)
                          {
                            case 0:
                                dataString = "idnode=";
                                dataString += buf[i];
                                break;
                            case 1:
                                dataString += "&sequencenum=";
                                dataString += buf[i];
                                break;
                            case 2: // gestione del segno neg/pos di snr
                                if (buf[i] == 1)
                                {
                                  dataString += "&snr=-";
                                }
                                else
                                {
                                  dataString += "&snr=";
                                }
                                break;
                            case 3:
                                dataString += buf[i];
                                break;
                            case 4: // gestione del segno neg/pos di rssi
                                if (buf[i] == 1)
                                {
                                  dataString += "&rssi=-";
                                }
                                else
                                {
                                  dataString += "&rssi=";
                                }
                                break;
                            case 5:
                                dataString += buf[i];
                                break;
                            case 6:
                                dataString += "&temp=";
                                dataString += buf[i];
                                break;
                            case 7:
                                dataString += ".";
                                dataString += buf[i];
                                break;
                            case 8:
                                dataString += "&umidity=";
                                dataString += buf[i];
                                break;
                            case 9:
                                dataString += buf[i];
                                break;
                            case 10:
                                dataString += "&lat=";
                                dataString += buf[i];
                                dataString += ".";
                                break;
                            case 11:
                                dataString += buf[i];
                                break;
                            case 12:
                                dataString += buf[i];
                                break;
                            case 13:
                                dataString += buf[i];
                                break;
                            case 14:
                                dataString += buf[i];
                                break;
                            case 15:
                                dataString += buf[i];
                                break;
                            case 16:
                                dataString += buf[i];
                                break;
                            case 17:
                                dataString += "&lon=";
                                dataString += buf[i];
                                dataString += ".";
                                break;
                            case 18:
                                dataString += buf[i];
                                break;
                            case 19:
                                dataString += buf[i];
                                break;
                            case 20:
                                dataString += buf[i];
                                break;
                            case 21:
                                dataString += buf[i];
                                break;
                            case 22:
                                dataString += buf[i];
                                break;
                            case 23:
                                dataString += buf[i];
                                break;
                          }*/                                                      
                    //}
                    Console.println();
               /*
                    dataString = "idnode=";
                    dataString += buf[0];
                    dataString +="&sequencenum=";
                    dataString += buf[1];
                    dataString +="&snr=";
                    dataString += buf[2];
             /*       if (buf[3] == 1)
                    {
                      dataString +="&snr=-";
                    }
                    else
                    {
                      dataString +="&snr=";
                    }
                    dataString += buf[4];
                    if (buf[5] == 1)
                    {
                      dataString +="&rssi=-";
                    }
                    else
                    {
                      dataString +="&rssi=";
                    }
                    dataString += buf[6];
                    dataString +="&temp=";
                    dataString +=buf[7];
                    dataString +=".";
                    dataString +=buf[8];
                    dataString +="&umidity=";
                    dataString +=buf[9];
                    dataString +=".";
                    dataString +=buf[9];*/
                    dataString = "idnode=";
                    dataString.concat(buf[0]);
                    dataString.concat("&sequencenum=");
                    dataString.concat(buf[1]);
                    /*dataString.concat("&snr=");
                    dataString.concat(buf[2]);*/

                    
                    Console.print("DataString  ");
                    Console.println(dataString);
                    
                    uploadData();
                    dataString="";
                                    
            } 
            else 
              Console.println(" CRC Fail");     
         }
         else
         {
              //Console.println("recv failed");
              ;
          }
     }

}
void uploadData() {//Upload Data to ThingSpeak
  // form the string for the API header parameter:


  // form the string for the URL parameter, be careful about the required "
/*  String upload_url = "https://api.thingspeak.com/update?api_key=";
  upload_url += myWriteAPIString;
  upload_url += "&";
  upload_url += dataString;*/
String upload_url = "83.212.126.194:50000/iot/?idnode=52&sequencenum=164&snr=8&rssi=-104&temp=25.0&umidity=29.0&lat=44.509231&lon=11.351216";

  Console.println("Call Linux Command to Send Data");
  Process p;    // Create a process and call it "p", this process will execute a Linux curl command
  p.begin("curl");
  p.addParameter("-k");
  p.addParameter(upload_url);
  p.run();    // Run the process and wait for its termination

  Console.print("Feedback from Linux: ");
  // If there's output from Linux,
  // send it out the Console:
  while (p.available()>0) 
  {
    char c = p.read();
    Console.write(c);
  }
  Console.println("");
  Console.println("Call Finished");
  Console.println("####################################");
  Console.println("");
}


