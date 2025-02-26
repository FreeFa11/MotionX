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

    Iteration = 300
    print("Sampling\t", sep="")

    while (Iteration > 0):
        DataString = SerialOBJ.readline().decode("utf-8")

        if DataString.startswith("{"):
            try:
                DataDict = json.loads(DataString)
                with open(f"{os.path.dirname(os.path.abspath(__file__))}/DataGyro.csv", mode= 'a+', newline='') as FileCSV:
                    CSVWriter = csv.writer(FileCSV)
                    CSVWriter.writerow([int(DataDict["GX"]), int(DataDict["GY"]), int(DataDict["GZ"])])
                Iteration -= 1
            except:
                pass
        
        if (Iteration % 100 == 0):
            print(".", end="")


def WriteData(Row):

    with open(f"{os.path.dirname(os.path.abspath(__file__))}/DataAcc.csv", mode= 'a+', newline='') as FileCSV:
        CSVWriter = csv.writer(FileCSV)
        CSVWriter.writerow(Row)
    

while (True):

    UserInput = input("""\nEnter Option:\n1) Collect\t2) Exit\n\n""")

    if (UserInput == "1"):
        # DataAcc = SampleDataAcc()
        DataGyro = SampleDataGyro()

        # if (input("\nDiscard?(Y/N):\t").upper() == "Y"):
        #     pass
        # else:
        #     WriteData(DataAcc)
        #     pass

    elif (UserInput == "2"):
        break