#include <Console.h>
#include <Process.h>
#define BAUDRATE 115200

void setup (){
  Bridge.begin(BAUDRATE);
  Console.begin();
}

void loop(){
  //Console.println("Hello world");
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

