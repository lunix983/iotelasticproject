#!/usr/bin/python

import requests
import json


url = 'http://127.0.0.1:31311/'
head = {'Content-type': 'application/json'}
nodeId = 65
payload = {'sensorId':nodeId,'sequenceNumber':'1','snr':'9','rssi':'-110','temperature':'25.3','umidity':'45.4','location':'44.509273,11.351128','time':'2018-05-20 08:33:46'}
#ret = requests.get(url,headers=head)
ret = requests.post(url,data=json.dumps(payload),headers=head)

print ret
