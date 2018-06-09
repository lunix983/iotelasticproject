#!/bin/sh

UPLOADURL="http://83.212.126.194:50000/logstashmetricinput"
CONTENTTYPE="\"Content-Type: application/json\""
INDEX="1"
while read line
do
	case "$INDEX" in 
		1)
			IDNODE=$line
			;;
		2)
		    SEQUENCENUM=$line
		    ;;
		3)
			SNR=$line
			;;
		4)
			RSSI=$line
			;;
		5)
			TEMPERATURE=$line
			;;
		6)
			UMIDITY=$line
			;;
		7)
			LATITUDE=$line
			;;
		8)
			LONGITUDE=$line
			;;	
	esac		
	let "INDEX++"

done

PAYLOAD=\'{\"idnode\":\"$IDNODE\",\"sequencenum\":\"$SEQUENCENUM\",\"snr\":\"$SNR\",\"rssi\":\"$RSSI\",\"temperature\":\"$TEMPERATURE\",\"umidity\":$UMIDITY,\"latitude\":\"$LATITUDE\",\"longitude\":\"$LONGITUDE\"}\'

eval curl -i -H $CONTENTTYPE -X POST -d $PAYLOAD $UPLOADURL