#/bin/sh

REMOTEURL="83.212.126.194:50000/test/?"

curl -i -k $REMOTEURL"idnode="$1"&seq="$2"&snr="$3"&rssi="$4"&temp="$5"&umidity="$6"&lat="$7"&lon="$8
