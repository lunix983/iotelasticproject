from flask import Flask, jsonify
from flask import request
from datetime import datetime
#from flask import abort
#from flask import make_response

#from flask import url_for
#from flask_httpauth import HTTPBasicAuth
import csv
import os.path


application = Flask(__name__)
#auth = HTTPBasicAuth()

@application.route('/logstashmetricinput', methods=['POST'])
def insert_metric():
    filename = "inputmetrics.csv"
    with open(filename, "aw") as fo:
         idnode = request.json['idnode']
         temperature = request.json['temperature']
         umidity = request.json['umidity']
         timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
         fo.write(timestamp + "," + idnode + "," + temperature + "," + umidity + "\n")
        # linewriter.writerow([idnode,temperature,umidity])
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
        
