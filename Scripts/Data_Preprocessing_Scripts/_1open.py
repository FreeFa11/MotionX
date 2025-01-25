import os
import pandas as pd

def open_csv(filename):
    current_path = os.path.dirname(__file__)
    csv_path = os.path.abspath(os.path.join(current_path, "..", "Data", "Raw", "csvRaw", filename))
    while True:
        try:
            data = pd.read_csv(csv_path)
            return data
        except FileNotFoundError:
            raise FileNotFoundError("File Not Found: Please check the file path and name.")

def main():
    data = open_csv("LeftClick.csv") 
    print(data.head())

if __name__ == "__main__":
    main()