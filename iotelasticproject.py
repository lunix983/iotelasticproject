from flask import Flask, jsonify
from flask import request
import datetime
#from flask import abort
#from flask import make_response

#from flask import url_for
#from flask_httpauth import HTTPBasicAuth
import csv
import os.path


app = Flask(__name__)
#auth = HTTPBasicAuth()

@app.route('/logstashmetricinput', methods=['POST'])
def insert_metric():
    filename = "inputmetrics.csv"
    with open(filename, "aw") as fo:
         idnode = request.json['idnode']
         temperature = request.json['temperature']
         umidity = request.json['umidity']
         timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
         fo.write(timestamp+""+idnode+","+temperature+","+umidity+"\n")
        # linewriter.writerow([idnode,temperature,umidity])
    return jsonify({'insert': "ok"}), 201


if __name__ == '__main__':

    app.run(debug=True)


