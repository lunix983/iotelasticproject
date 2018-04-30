#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <DHT.h>



RH_RF95 rf95;

#define DHTPIN 7
#define DHTTYPE DHT11
//#define dht_dpin A0 // Use A0 pin as Data pin for DHT11. 
int dht_dpin = 7;
byte bGlobalErr;
char dht_dat[5]; // Store Sensor Data DA VERIFICARE
int nodeID = 52;
float frequency = 868.0;
int sf = 12;
long bw = 250000;
int cr = 8;
bool hand = true;
unsigned int count = 1;

float flat, flon;
unsigned long age;

DHT dht(DHTPIN, DHTTYPE);
TinyGPS gps;

 /*The circuit:ss(rx, tx)ss(3, 4)
 * RX is digital pin 10 (connect to TX of GPS or other device)
 * TX is digital pin 11 (connect to RX of GPS or other device)
*/
SoftwareSerial ss(3, 4); // Arduino RX, TX ,

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);


void setup()
{
  // initialize both serial ports:
  Serial.begin(9600);  // Serial to print out GPS info in Arduino IDE
  ss.begin(9600); // SoftSerial port to get GPS data. 
  while (!Serial) {
     ;
  };
  Serial.println("Monitor Dragino LoRa GPS Shield Status");
  Serial.print("Testing TinyGPS library. "); Serial.println(TinyGPS::library_version());
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
  Serial.println("    DHT11 Temperature and Humidity Sensor and GPS\n");
  Serial.print("LoRa End Node ID: "); Serial.println(nodeID);
  Serial.print("Spreading Factor: "); Serial.println(sf);
  Serial.print("Bandwidth:" );  Serial.println(bw);
  Serial.print("Code Rate:" );  Serial.println(cr);
  Serial.println();
  //setRssiSnr();
  
}

void getGpsData()
{
  gps.f_get_position(&flat, &flon, &age);
  //print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  //print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  Serial.println();
  Serial.print("FLAT: ");
  Serial.println(flat, 6);
  Serial.print("FLON: ");
  Serial.println(flon, 6);
  Serial.println();
  smartdelay(1000);
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



static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
    {
      //ss.print(Serial.read());
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

void setRssiSnr()
{
  int rssi = rf95.lastRssi();
  int snr = rf95.lastSNR();

  if (rssi == 0 || snr ==0){
    setRssiSnr();
    delay(3000);
    Serial.println("SNR o RSSI zero");
  }
}




void loop()
{
  getGpsData();
  InitDHT();
<<<<<<< HEAD
  
  
  Serial.print("****** "); Serial.print("COUNT="); Serial.println(count); Serial.println("****** ");
  count++;
  char data[50] = {0} ;
  //uint8_t data[50];
  int dataLength = 50; // Payload Length
=======
  char gps_lon[20]={"\0"};  
  char gps_lat[20]={"\0"}; 
  String datastring="";
  String datastring1="";
  /*if (hand)
  {
    handshake();
    hand = false;  
  }*/
  
  Serial.print("****** "); Serial.print("COUNT="); Serial.println(count); Serial.println("****** ");
  count++;
  //char data[50] = {0} ;
  uint32_t data[100];
  int dataLength = 100; // Payload Length
>>>>>>> 3c3f4bd639f89374e829cf1aacfd8a0891f26b5a
  float h = dht.readHumidity(); // Read temperature Humidity
  float t = dht.readTemperature(); // Read temperature as Celsius (the default)
 /* Serial.print("temperature: "); 
  Serial.println(t);
  Serial.print("umidity: "); 
  Serial.println(h);*/
  
  int rssi = rf95.lastRssi();
  int snr = rf95.lastSNR();
  int snrSign=0;
  int rssiSign=0;
  Serial.print("SNR: "); 
  Serial.println(rf95.lastSNR());
  if (rssi < 0) {
      Serial.print("RSSI negativo: ");  // print RSSI
      rssi = rssi * -1;
      rssiSign = 1; //imposto a uno data[7] quando rssi è negativo altrimenti il default è 0
  }
   if (snr < 0) {
      Serial.print("SNR negativo: ");  // print RSSI
      snr = snr * -1;
      snrSign = 1; //imposto a uno data[7] quando rssi è negativo altrimenti il default è 0
  }
  Serial.print("RSSI: ");  
  Serial.println(rf95.lastRssi()); // print RSSI
  int tempIntPart = t;
  int tempDecPart = (t - tempIntPart) * 100;
  int umIntPart = h;
  int unDecPart = (h - umIntPart) * 100;
  int flatIntPart = flat;
  long flatDecPart = (flat - flatIntPart) * 1000000;
  int flonIntPart = flon;
<<<<<<< HEAD
  long flonDecPart = ((flon - flonIntPart) * 1000000) +1 ;
/*
  Serial.print("** flatIntPart: ");  Serial.println(flatIntPart);
  Serial.print("** flatDecPart: ");  Serial.println(flatDecPart);
  Serial.print("** flonIntPart: ");  Serial.println(flonIntPart);
  Serial.print("** flonDecPart: ");  Serial.println(flonDecPart);*/

 
 
 
  
  
  data[0] = nodeID;
  data[1] = snr;
  data[2] = snrSign;
  data[3] = rssi;
  data[4] = rssiSign;
  data[5] = count;
  data[6] = umIntPart;
  data[7] = unDecPart;
  data[8] = tempIntPart;
  data[9] = tempDecPart;
  data[10] = flatIntPart ; 
  
  for (int i=0; i<6; i++){ 
    data[11 + i] = flatDecPart % 10;
    flatDecPart /= 10;
  }
  data[17] = flonIntPart;
  for (int i=0; i<6; i++){ 
    data[18 + i] = flonDecPart % 10;
    flonDecPart /= 10;
  }

 

      
=======
  long flonDecPart = (flon - flonIntPart) * 1000000;

  Serial.print("** flatIntPart: ");  Serial.println(flat);
  Serial.print("** flatDecPart: ");  Serial.println(flatDecPart);
  Serial.print("** flonIntPart: ");  Serial.println(flon);
  Serial.print("** flonDecPart: ");  Serial.println(flonDecPart);
  //datastring +=dtostrf(flat, 0, 6, gps_lat); 
  //datastring1 +=dtostrf(flon, 0, 6, gps_lon);
/*  char cp[10];
  datastring.toCharArray(cp, 10);*/
  data[0] = nodeID;
  data[1] = snr;
  data[2] = rssi;
  data[3] = umIntPart;
  data[4] = unDecPart;
  data[5] = tempIntPart;
  data[6] = tempDecPart;
  data[8] = flatIntPart; 
  data[9] = flatDecPart;
  data[10] = flonIntPart; 
  data[11] = flonDecPart;

  Serial.print("data[8] gps_lat int part: ");
  Serial.println(data[8]);
  Serial.print("data[9] gps_lat dec part: ");
  Serial.println(data[9]);

   Serial.print("data[10] gps_lon int part: ");
  Serial.println(data[10]);
  Serial.print("data[11] gps_lon dec part: ");
  Serial.println(data[11]);
    
>>>>>>> 3c3f4bd639f89374e829cf1aacfd8a0891f26b5a
  uint16_t crcData = CRC16((unsigned char*)data,dataLength);//get CRC DATA

  Serial.print("Data to be sent(without CRC): ");
    
  int i;
  for(i = 0;i < dataLength; i++)
  {      
      Serial.print(data[i],HEX);
      Serial.print(" ");
  }
  Serial.println();      
<<<<<<< HEAD
  unsigned char sendBuf[50]={0};
  //uint8_t sendBuf[100]={0};
  for(i = 0;i < dataLength;i++)
  {
        sendBuf[i] = data[i] ;
       /* Serial.print("sendBuf ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(sendBuf[i]);*/
  }  
  sendBuf[dataLength] = (unsigned char)crcData; // Add CRC to LoRa Data
  sendBuf[dataLength+1] = (unsigned char)(crcData>>8); // Add CRC to LoRa Data
=======
 // unsigned char sendBuf[50]={0};
  uint32_t sendBuf;
 /* for(i = 0;i < dataLength;i++)
  {
        sendBuf[i] = data[i] ;
  }  */
 // sendBuf[dataLength] = (unsigned char)crcData; // Add CRC to LoRa Data
 // sendBuf[dataLength+1] = (unsigned char)(crcData>>8); // Add CRC to LoRa Data
  //sendBuf[dataLength] = crcData;
>>>>>>> 3c3f4bd639f89374e829cf1aacfd8a0891f26b5a
  Serial.print("Data to be sent(with CRC):    ");
 /* for(i = 0;i < (dataLength +2); i++)
  {
      Serial.print(sendBuf[i],HEX);
      Serial.print(" ");
  }*/
  Serial.println();
<<<<<<< HEAD
  rf95.send(sendBuf, dataLength+2);//Send LoRa Data

  ///// Replay from gateway
  
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];//Reply data array
  uint8_t len = sizeof(buf);//reply data length

  if (rf95.waitAvailableTimeout(30000))// Check If there is reply in 30 seconds.
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
    //delay(3000); // Send sensor data every 30 seconds
  Serial.println("");
=======
  rf95.send(data, dataLength+2);//Send LoRa Data
>>>>>>> 3c3f4bd639f89374e829cf1aacfd8a0891f26b5a
  
  
}

