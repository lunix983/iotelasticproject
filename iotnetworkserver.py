from flask import Flask, jsonify
from flask import request
from datetime import datetime
import csv
import os.path
import json
import requests


application = Flask(__name__)
#auth = HTTPBasicAuth()

@application.route('/logstashmetricinput', methods=['POST'])
def insert_metric():
    url = 'http://127.0.0.1:31311/'
    head = {'Content-type': 'application/json'}
    filename = "/home/iotadmin/git/iotelasticproject/inputmetric.cvs"
    idnode = str(request.json['idnode'])
    sequencenum = str(request.json['sequencenum'])
    snr = str(request.json['snr'])
    rssi = str(request.json['rssi'])
    temperature = str(request.json['temperature'])
    umidity = str(request.json['umidity'])
    latitude = str(request.json['latitude'])
    longitude = str(request.json['longitude'])
    nodelocation = str(latitude)+","+str(longitude)
    delay = str(request.json['delay'])
    lorasetup = str(request.json['lorasetup'])
    packetsize = str(request.json['packetsize'])
    expid = str(request.json['experimentid'])
    gwlocation = str(request.json['gw-location'])
    timestamp = str(datetime.now().strftime('%Y-%m-%d %H:%M:%S'))
    payload = {'sensorId':idnode,'sequenceNumber':sequencenum,'snr':snr,'rssi':rssi,'temperature':temperature,'umidity':umidity,'node-location':nodelocation,'time':timestamp,'delay':delay,'lorasetup':lorasetup,'packetsize':packetsize,'experimentid':expid,'gw-location':gwlocation}
    print payload
    res = requests.post(url,data=json.dumps(payload),headers=head)
    print res
    with open(filename, "aw") as fo:
         fo.write(timestamp+","+idnode+","+sequencenum +","+snr+","+rssi+","+temperature+","+umidity+","+nodelocation+","+delay+","+lorasetup+","+packetsize+","+expid+","+gwlocation+"\n")
    return jsonify({'insert': "ok"}), 201


@application.route('/iot/', methods=['GET'])
def foo():
    filename = "inputmetrics_2.csv"
    with open(filename, "aw") as fo:
        idnode = request.args.get('idnode')
        snr = request.args.get('snr')
        rssi = request.args.get('rssi')
        sequence = request.args.get('sequencenum')
        temperature = request.args.get('temp')
        umidity = request.args.get('umidity')
        lat = request.args.get('lat')
        lon = request.args.get('lon')
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        fo.write(timestamp + "," + sequence + "," +  idnode + "," + snr + "," + rssi  +"," + temperature + "," + umidity + "," + lat + "," + lon +  "\n")

    return 'success', 200

@application.route('/test/', methods=['GET'])
def test():
    filename = "inputmetrics_2.csv"
    with open(filename, "aw") as fo:
        idnode = request.args.get('idnode')
        sequence = request.args.get('seq')
        snr = request.args.get('snr')
        rssi = request.args.get('rssi')
	temperature = request.args.get('temp')
        umidity = request.args.get('umidity')
	lat = request.args.get('lat')
        lon = request.args.get('lon')
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        fo.write(timestamp + "," + sequence + "," +  idnode + "," + snr + "," + rssi  +"," + temperature + "," + umidity + "," + lat + "," + lon +  "\n")
        #fo.write(timestamp + "," + sequence + "," +  idnode + "," + snr + "," + rssi  +"," + temperature + "," + umidity + "," + "\n")
        #fo.write(timestamp + "," + sequence + "," +  idnode + "," + snr + ","  +"," + temperature + "," + umidity + "," + "\n")
        #fo.write(timestamp + "," + sequence + "," +  idnode + "," + snr + " " + rssi + "," + temperature + "\n")
        #fo.write(timestamp + "," + sequence + "," +  idnode + "," + snr + " " + rssi + "," + "\n")

    return 'success', 200


if __name__ == '__main__':

    application.run(debug=True,host='0.0.0.0')
    #application.run(host='127.0.0.1')
        
