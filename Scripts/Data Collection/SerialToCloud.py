import serial
import json
import hmac, hashlib
import requests
import time


LABEL = "RightClick"
# LABEL = "LeftClick"
# LABEL = "RingClick"



# **********************************************Collection************************************************#
# Serial Initialization
SerialOBJ = serial.Serial(port='COM12', baudrate=115200, timeout=1)

DataFull = []
for i in range(1, 1500):
    DataString = SerialOBJ.readline().decode("utf-8")

    if DataString.startswith("{"):
        DataDict = json.loads(DataString)
        DataFull.append(
            [int(DataDict["IF"]),
             int(DataDict["MF"]),
             int(DataDict["RF"]),
             float(DataDict["GX"]/65.0),
             float(DataDict["GY"]/65.0),
             float(DataDict["GZ"]/65.0),
             float(DataDict["RO"]),
             float(DataDict["PI"]),
             float(DataDict["YA"])]
            )
    


# ***********************************************Uploading************************************************#

# Keys for digital Signature
API_KEY = "Key"
HMAC_KEY = "Key"


DataOBJ = {
    "protected": {
        "ver": "v1",
        "alg": "HS256",
        "iat": time.time()
    },
    "signature": "0000000000000000000000000000000000000000000000000000000000000000",
    "payload": {
        "device_name": "",
        "device_type": "Acer",
        "interval_ms": 10,
        "sensors": [
            { "name": "Index", "units": "°" },
            { "name": "Middle", "units": "°" },
            { "name": "Ring", "units": "°" },
            { "name": "GyroX", "units": "°/s" },
            { "name": "GyroY", "units": "°/s" },
            { "name": "GyroZ", "units": "°/s" },
            { "name": "Roll", "units": "°" },
            { "name": "Pitch", "units": "°" },
            { "name": "Yaw", "units": "°" }
        ],
        "values": DataFull
    }
}

DataJSON = json.dumps(DataOBJ)



# Signing the Data
signature = hmac.new(bytes(HMAC_KEY, 'utf-8'), msg = DataJSON.encode('utf-8'), digestmod = hashlib.sha256).hexdigest()

DataOBJ["signature"] = signature
DataJSON = json.dumps(DataOBJ)



# Uploading
res = requests.post(url     =   'https://ingestion.edgeimpulse.com/api/training/data',
                    data    =   DataJSON,
                    headers =   {
                        'Content-Type': 'application/json',
                        'x-file-name': LABEL,
                        'x-api-key': API_KEY
                    })
if (res.status_code == 200):
    print('Uploaded file to Edge Impulse', res.status_code, res.content)
else:
    print('Failed to upload file to Edge Impulse', res.status_code, res.content)

