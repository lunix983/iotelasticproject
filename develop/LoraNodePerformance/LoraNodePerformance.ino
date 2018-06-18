#include <Adafruit_GPS.h>

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <RH_RF95.h>


RH_RF95 rf95;
TinyGPS gps;

int nodeID = 52;
float frequency = 868.0;
int count = 0;
float flat, flon;
unsigned long age;
unsigned long timesend, timereplay, tdelay;
SoftwareSerial ss(3, 4); 
float gpsdefcoord = 1000.00;


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
  Serial.begin(9600);  // Serial to print out GPS info in Arduino IDE
  ss.begin(9600); // SoftSerial port to get GPS data. 
  while (!Serial) {
     ;
  };

  Serial.println("Progetto Tesi Lunix. Lora Node performance Signal measurement.");
  if (!rf95.init())
  {
        Serial.println("RF95 init failed");
  }
  else 
  {
       Serial.println("RF95 init done.");
  }
  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);
  Serial.print("LoRa End Node ID: "); Serial.println(nodeID);
  
} // end setup

// SMART DELAY
static void smartdelay(unsigned long ms)
{
  Serial.println("... smartdelay ...");
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


void printData(){
  Serial.print("MILLIS: ");  Serial.println(data.time );
  Serial.print("LAT: ");  Serial.println(data.lat);
  Serial.print("LON: ");  Serial.println(data.lon);
  Serial.print("RSSI: ");  Serial.println(data.rssi);
  Serial.print("SNR: ");  Serial.println(data.snr);
  //Serial.print();  Serial.println();
 
}

void loop()
{
  
  gps.f_get_position(&flat, &flon, &age);
  
  
  smartdelay(500);
 
  data.idnode = nodeID;
  data.sequencenum = count;
  data.lat = flat;
  data.lon = flon;
  data.time = millis();
  data.rssi = rf95.lastRssi();
  data.snr = rf95.lastSNR();
  printData();
  memcpy(tx_buf, &data, sizeof(data) );
  rf95.send((uint8_t *)tx_buf, sizeof(data));

  rf95.waitPacketSent();
    // Now wait for a reply
  uint8_t rx_buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(rx_buf);


  if (rf95.waitAvailableTimeout(3000))
  {
      if (rf95.recv(rx_buf, &len))
      {
        memcpy(&data, rx_buf, sizeof(data)); 
        data.delay = (float)(millis() - data.time) /2 ;
        Serial.print("Message from gateway: ");   Serial.println(data.message);
      }
      else
      {
         Serial.println("recv failed");
      }
           
  } // end if waitAvailableTimeout
  else
  {
    Serial.println("No reply, is LoraGateway running?");
  }
  count++;
} // end loop

