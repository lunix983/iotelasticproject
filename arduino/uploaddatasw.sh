#!/bin/sh
#String upload_url = "83.212.126.194:50000/iot/?idnode=52&sequencenum=164&snr=8&rssi=-104&temp=25.0&umidity=29.0&lat=44.509231&lon=11.351216";

UPLOADURL="83.212.126.194:50000/iot/?"
INDEX="1"


for i in `cat /tmp/iotdata.txt`
do

   case "$INDEX" in
	1)
            UPLOADURL=$UPLOADURL"idnode="$i
	    ;;
	2)
            UPLOADURL=$UPLOADURL"&sequencenum="$i
	    ;;
	3) 
	    UPLOADURL=$UPLOADURL"&snr="$i
	    ;;
	4) 
	    UPLOADURL=$UPLOADURL"&rssi="$i
	    ;;
	5) 
	    UPLOADURL=$UPLOADURL"&temp="$i
	    ;;
	6) 
	    UPLOADURL=$UPLOADURL"&umidity="$i
	    ;;
  	7) 
	    UPLOADURL=$UPLOADURL"&lat="$i
	    ;;
	8) 
	    UPLOADURL=$UPLOADURL"&lon="$i
	    ;;
   esac 
   let "INDEX++"
done
echo "URL: " $UPLOADURL
curl -i -k $UPLOADURL
rm -f /tmp/iotdata.txt
