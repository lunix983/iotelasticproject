#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <DHT.h>


RH_RF95 rf95;
TinyGPS gps;

int nodeID = 52;
float frequency = 868.0;
int count = 0;
float flat, flon;
unsigned long age;
SoftwareSerial ss(3, 4); 


#define DHTPIN 7
#define DHTTYPE DHT11
//#define dht_dpin A0 // Use A0 pin as Data pin for DHT11. 
int dht_dpin = 7;
DHT dht(DHTPIN, DHTTYPE);

struct message{
  String idnode;
  String sequencenum;
  String time;
  String snr;
  String rssi;
  String lat;
  String lon;
  String delay;
  String status;
  String umidity;
  String temp;
  String lorasetup;
}data;

String* dataPtr = (String*)&data;


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
  
  Serial.print("SEQUENCE: ");  Serial.println(data.sequencenum );
  Serial.print("MILLIS: ");  Serial.println(data.time );
  Serial.print("LAT: ");  Serial.println(data.lat);
  Serial.print("LON: ");  Serial.println(data.lon);
  Serial.print("RSSI: ");  Serial.println(data.rssi);
  Serial.print("SNR: ");  Serial.println(data.snr);
  Serial.print("DELAY: ");  Serial.println(data.delay );
  Serial.print("TEMP: ");  Serial.println(data.temp );
  Serial.print("UMIDITY: ");  Serial.println(data.umidity );
 
}

void loop()
{
  
  gps.f_get_position(&flat, &flon, &age);
  pinMode(dht_dpin,OUTPUT);//Set A0 to output
  digitalWrite(dht_dpin,HIGH);//Pull high A0
  
  smartdelay(500);
  
  data.idnode = nodeID;
  data.umidity = String(dht.readHumidity());
  data.temp = String(dht.readTemperature());
  data.sequencenum = count;
  data.lat = String(flat,6);
  data.lon = String(flon,6);
  data.time = String(millis());
  data.rssi = rf95.lastRssi();
  data.snr = rf95.lastSNR();

  unsigned long timestamp = millis();
  
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
       
        data.delay = String((millis() - timestamp) /2);
        
        if (data.status == 1){
          Serial.print("Message from gateway: ");  Serial.println(data.status);
          Serial.print("SEQUENCE REPLAY: ");  Serial.println(data.sequencenum );
           Serial.print("TIME REPLAY: ");  Serial.println(data.time );
          Serial.print("DELAY REPLAY: ");  Serial.println(data.delay );
          
        }
         
        
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
  delay(3000);
  Serial.println("-------------------------------------");
} // end loop

