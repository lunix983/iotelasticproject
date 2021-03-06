#include <RHTcpProtocol.h>
#include <RH_ASK.h>
#include <RH_NRF905.h>
#include <RHGenericDriver.h>
#include <RHHardwareSPI.h>
#include <RH_NRF24.h>
#include <RH_Serial.h>
#include <RHRouter.h>
#include <RH_RF69.h>
#include <RadioHead.h>
#include <RH_MRF89.h>
#include <RH_RF22.h>
#include <RH_RF24.h>
#include <RHSoftwareSPI.h>
#include <RHCRC.h>
#include <RHSPIDriver.h>
#include <RH_CC110.h>
#include <radio_config_Si4460.h>
#include <RHDatagram.h>
#include <RHReliableDatagram.h>
#include <RHMesh.h>
#include <RHGenericSPI.h>
#include <RH_TCP.h>
#include <RH_NRF51.h>
#include <RHNRFSPIDriver.h>
//#include <RH_RF95.h>



#include <DHT.h>
#include <SdFat.h>
#include <FreeStack.h>
#include <SdFatConfig.h>
#include <BlockDriver.h>
#include <SysCall.h>
#include <MinimumSerial.h>
#include <SPI.h>
#include <RH_RF95.h>


/*
  Upload Data to IoT Server ThingSpeak (https://thingspeak.com/):
  Support Devices: LoRa Shield + Arduino 
  
  Example sketch showing how to read Temperature and Humidity from DHT11 sensor,  
  Then send the value to LoRa Gateway, the LoRa Gateway will send the value to the 
  IoT server

  It is designed to work with the other sketch dht11_server. 

  modified 24 11 2016
  by Edwin Chen <support@dragino.com>
  Dragino Technology Co., Limited
*/


/*#include <String.h>*/

RH_RF95 rf95;

#define DHTPIN 7
#define DHTTYPE DHT11
//#define dht_dpin A0 // Use A0 pin as Data pin for DHT11. 
int dht_dpin = 7;
byte bGlobalErr;
char dht_dat[5]; // Store Sensor Data
//char node_id[3] = {1,1,1}; //LoRa End Node ID
int nodeID = 51;
float frequency = 868.0;
unsigned int count = 1;
DHT dht(DHTPIN, DHTTYPE);
int sf = 12;
long bw = 250000;
int cr = 8;

void setup()
{
   // InitDHT();
    Serial.begin(9600);
    if (!rf95.init())
        Serial.println("init failed");
    // Setup ISM frequency
    rf95.setFrequency(frequency);
    // Setup Power,dBm
    rf95.setTxPower(13);
   // Setup Spreading Factor (6 ~ 12)
   rf95.setSpreadingFactor(sf);
  
    // Setup BandWidth, option: 7800,10400,15600,20800,31250,41700,62500,125000,250000,500000
    //Lower BandWidth for longer distance.
    rf95.setSignalBandwidth(bw);
  
    // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
    rf95.setCodingRate4(cr);
  
    
    dht.begin();
    Serial.println("LoRa End Node Example --"); 
    Serial.println("    DHT11 Temperature and Humidity Sensor by LUIX\n");
    Serial.print("LoRa End Node ID: ");
    Serial.println(nodeID);
    Serial.print("Spreading Factor: ");
    Serial.println(sf);
     Serial.print("Bandwidth:" );
    Serial.println(bw);
     Serial.print("Code Rate:" );
    Serial.println(cr);
/*
    for(int i = 0;i < 3; i++)
    {
        Serial.print(node_id[i],HEX);
    }*/
    Serial.println();
}

void InitDHT()
{
    pinMode(dht_dpin,OUTPUT);//Set A0 to output
    digitalWrite(dht_dpin,HIGH);//Pull high A0
    
}

//Get Sensor Data
void ReadDHT()
{
    bGlobalErr=0;
    byte dht_in;
    byte i;
        
    //pinMode(dht_dpin,OUTPUT);
    digitalWrite(dht_dpin,LOW);//Pull Low A0 and send signal
    delay(30);//Delay > 18ms so DHT11 can get the start signal
        
    digitalWrite(dht_dpin,HIGH);
    delayMicroseconds(40);//Check the high level time to see if the data is 0 or 1
    pinMode(dht_dpin,INPUT);
    // delayMicroseconds(40);
    dht_in=digitalRead(dht_dpin);//Get A0 Status
    //   Serial.println(dht_in,DEC);
    if(dht_in){
        bGlobalErr=1;
        return;
    }
    delayMicroseconds(80);//DHT11 send response, pull low A0 80us
    dht_in=digitalRead(dht_dpin);
    
    if(!dht_in){
        bGlobalErr=2;
        return;
    }
    delayMicroseconds(80);//DHT11 send response, pull low A0 80us
    for (i=0; i<5; i++)//Get sensor data
    dht_dat[i] = read_dht_dat();
    pinMode(dht_dpin,OUTPUT);
    digitalWrite(dht_dpin,HIGH);//release signal and wait for next signal
    byte dht_check_sum = dht_dat[0]+dht_dat[1]+dht_dat[2]+dht_dat[3];//calculate check sum
    if(dht_dat[4]!= dht_check_sum)//check sum mismatch
        {bGlobalErr=3;}
};

byte read_dht_dat(){
    byte i = 0;
    byte result=0;
    for(i=0; i< 8; i++)
    {
        while(digitalRead(dht_dpin)==LOW);//wait 50us
        delayMicroseconds(30);//Check the high level time to see if the data is 0 or 1
        if (digitalRead(dht_dpin)==HIGH)
        result |=(1<<(7-i));//
        while (digitalRead(dht_dpin)==HIGH);//Get High, Wait for next data sampleing. 
    }
    return result;
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

uint16_t CRC16(uint8_t *pBuffer,uint32_t length)
{
    uint16_t wCRC16=0;
    uint32_t i;
    if (( pBuffer==0 )||( length==0 ))
    {
      return 0;
    }
    for ( i = 0; i < length; i++)
    { 
      wCRC16 = calcByte(wCRC16, pBuffer[i]);
    }
    return wCRC16;
}




void loop()
{
    Serial.print("###########    ");
    Serial.print("COUNT=");
    Serial.println(count);
   
    count++;
    
   // ReadDHT();
    char data[50] = {0} ;
   // int dataLength = 7; // Payload Length
    int dataLength = 9; // Payload Length
    // Use data[0], data[1],data[2] as Node ID
   float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    int rssi = rf95.lastRssi();
    int snr = rf95.lastSNR();
    Serial.print("SNR: ");
    Serial.println(snr);
    if (rssi < 0) {
      Serial.print("RSSI negativo: ");  // print RSSI
      rssi = rssi * -1;
      data[8] = 1;
    }
    
    Serial.print("RSSI: ");  // print RSSI
    Serial.println(rssi);
    int tempIntPart = t;
    int tempDecPart = (t - tempIntPart) * 100;
    int umIntPart = h;
    int unDecPart = (h - umIntPart) * 100;
    data[0] = nodeID;
    data[3] = umIntPart;
    data[4] = unDecPart;
    data[5] = tempIntPart;
    data[6] = tempDecPart;
    data[7] = rssi;
   
    switch (bGlobalErr)
    {
      case 0:
          Serial.print("Current humidity = ");
          Serial.print(data[3], DEC);//Show humidity
          Serial.print(".");
          Serial.print(data[4], DEC);//Show humidity
          Serial.print("%  ");
          Serial.print("temperature = ");
          Serial.print(data[5], DEC);//Show temperature
          Serial.print(".");
          Serial.print(data[6], DEC);//Show temperature
          Serial.println("C  ");
          break;
       case 1:
          Serial.println("Error 1: DHT start condition 1 not met.");
          break;
       case 2:
          Serial.println("Error 2: DHT start condition 2 not met.");
          break;
       case 3:
          Serial.println("Error 3: DHT checksum error.");
          break;
       default:
          Serial.println("Error: Unrecognized code encountered.");
          break;
    }
    
    uint16_t crcData = CRC16((unsigned char*)data,dataLength);//get CRC DATA
    //Serial.println(crcData,HEX);
    
    Serial.print("Data to be sent(without CRC): ");
    
    int i;
    for(i = 0;i < dataLength; i++)
    {
        Serial.print(data[i],HEX);
        Serial.print(" ");
    }
    Serial.println();
        
    unsigned char sendBuf[50]={0};

    for(i = 0;i < dataLength;i++)
    {
        sendBuf[i] = data[i] ;
    }
    
    sendBuf[dataLength] = (unsigned char)crcData; // Add CRC to LoRa Data
    sendBuf[dataLength+1] = (unsigned char)(crcData>>8); // Add CRC to LoRa Data

    Serial.print("Data to be sent(with CRC):    ");
    for(i = 0;i < (dataLength +2); i++)
    {
        Serial.print(sendBuf[i],HEX);
        Serial.print(" ");
    }
    Serial.println();

    rf95.send(sendBuf, dataLength+2);//Send LoRa Data
    if( rf95.isChannelActive() ) {
        Serial.println("Channel Is Active " );
    }
    else {
        Serial.println("Channel Is NOT Active " );
    }
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];//Reply data array
    uint8_t len = sizeof(buf);//reply data length

    if (rf95.waitAvailableTimeout(3000))// Check If there is reply in 3 seconds.
    {
        // Should be a reply message for us now   
        if (rf95.recv(buf, &len))//check if reply message is correct
       {
           if(buf[0] == nodeID ) // Check if reply message has the our node ID
           {
               pinMode(4, OUTPUT);
               digitalWrite(4, HIGH);
               Serial.print("Got Reply from Gateway: ");//print reply
               Serial.println((char*)buf);             
               delay(400);
               digitalWrite(4, LOW); 
           }    
        }
        else
        {
           Serial.println("recv failed");//
           rf95.send(sendBuf, strlen((char*)sendBuf));//resend if no reply
        }
    }
    else
    {
        Serial.println("No reply, is LoRa gateway running?");//No signal reply
        rf95.send(sendBuf, strlen((char*)sendBuf));//resend data
    }
    delay(30000); // Send sensor data every 30 seconds
    Serial.println("");
}



