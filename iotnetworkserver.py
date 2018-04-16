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
         fo.write(timestamp+","+idnode+","+temperature+","+umidity+"\n")
        # linewriter.writerow([idnode,temperature,umidity])
    return jsonify({'insert': "ok"}), 201

@application.route('/iot/', methods=['GET'])
def foo():
    filename = "inputmetrics_2.csv"
    with open(filename, "aw") as fo:
        idnode = request.args.get('idnode')
        snr = request.args.get('snr')
        rssi = request.args.get('rssi')
        temperature = request.args.get('temp')
        umidity = request.args.get('umidity')
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        fo.write(timestamp + "," + idnode + "," + snr + "," + rssi + ","  + temperature + "," + umidity + "\n")
    return 'success', 200


if __name__ == '__main__':

    application.run(debug=True,host='0.0.0.0')
    #application.run(host='127.0.0.1')
        
