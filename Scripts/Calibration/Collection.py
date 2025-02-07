import serial
import json
import csv
import os


SAMPLES = 500
PORT = '/dev/ttyACM0'


# Serial Initialization
SerialOBJ = serial.Serial(port=PORT, baudrate=115200, timeout=1)


def SampleDataAcc():

    AX = 0
    AY = 0
    AZ = 0

    for i in range(0, SAMPLES):
        DataString = SerialOBJ.readline().decode("utf-8")

        if DataString.startswith("{"):
            try:
                DataDict = json.loads(DataString)

                AX += int(DataDict["AX"])
                AY += int(DataDict["AY"])
                AZ += int(DataDict["AZ"])
            except:
                pass

    return [AX/SAMPLES, AY/SAMPLES, AZ/SAMPLES]

def SampleDataGyro():

    GX = 0
    GY = 0
    GZ = 0

    for i in range(0, SAMPLES):
        DataString = SerialOBJ.readline().decode("utf-8")

        if DataString.startswith("{"):
            DataDict = json.loads(DataString)

            GX += int(DataDict["GX"])
            GY += int(DataDict["GY"])
            GZ += int(DataDict["GZ"])

    return [GX/SAMPLES, GY/SAMPLES, GZ/SAMPLES]


def WriteData(Row, Acc):

    if Acc:
        with open(f"{os.path.dirname(os.path.abspath(__file__))}/DataAcc.csv", mode= 'a+', newline='') as FileCSV:
            CSVWriter = csv.writer(FileCSV)
            CSVWriter.writerow(Row)
    else:
        with open(f"{os.path.dirname(os.path.abspath(__file__))}/DataGyro.csv", mode= 'a+', newline='') as FileCSV:
            CSVWriter = csv.writer(FileCSV)
            CSVWriter.writerow(Row)
    

while (True):

    UserInput = input("""\nEnter Option:\n1) Collect\t2) Exit\n\n""")

    if (UserInput == "1"):
        DataAcc = SampleDataAcc()
        # DataGyro = SampleDataGyro()

        if (input("\nDiscard?(Y/N):\t").upper() == "Y"):
            pass
        else:
            WriteData(DataAcc, True)
            # WriteData(DataGyro, False)

    elif (UserInput == "2"):
        break