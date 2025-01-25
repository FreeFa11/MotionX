import json
import os
import pandas as pd

def define_directories():
    try: 
        base_dir = os.path.dirname(__file__)
        raw_json = os.path.abspath(os.path.join(base_dir, "..", "Data", "Raw", "jsonRaw"))
        raw_csv = os.path.abspath(os.path.join(base_dir, "..", "Data", "Raw", "csvRaw"))

        os.makedirs(raw_csv, exist_ok=True)

        return raw_json, raw_csv
    
    except Exception as e:
        raise Exception(f"Unexpected error occured as {e}")

def file_path(json_dir):
    json_files = []

    for filename in os.listdir(json_dir):
        if filename.endswith(".json"):
            json_path = os.path.join(json_dir, filename)
            json_files.append(json_path)
    
    return json_files


def convert_json_to_csv(json_dir,csv_dir):

    try: 
        json_files = file_path(json_dir)
        parameters = []
        timestamps = []

        for j in json_files:
            try: 
                with open(j, "r") as file:
                    data = json.load(file)
                
                interval_ms = data["payload"]["interval_ms"]
                values = data["payload"]["values"]

                for sensor in data["payload"]["sensors"]:
                    parameters.append(sensor["name"])

                for i in range(len(values)):
                    timestamps.append(i*interval_ms)

                df = pd.DataFrame(values, columns=parameters)
                df.insert(0, "timestamps(ms)", timestamps)

                json_filename = os.path.basename(j)
                csv_filename = os.path.splitext(json_filename)[0] + ".csv"
                

                output_path = os.path.join(csv_dir, csv_filename)

                df.to_csv(output_path, index=False)

                parameters = []
                values = []
                timestamps = []
            except KeyError:
                raise KeyError(f"Invalid key in file {j}")
            except ValueError:
                raise ValueError(f"Invalid value in the file {j}")
            except Exception as e:
                raise Exception(f"Unexpected error occured as {e}")

    except FileNotFoundError:
        raise FileNotFoundError("The file is not there in the specified directory.")
    except pd.errors.EmptyDataError:
        raise pd.errors.EmptyDataError("The file is empty.")
    except json.JSONDecodeError:
        raise json.JSONDecodeError("The json file is not in the correct format.")
    except Exception as e:
        raise Exception(f"Unexpected error occured as {e}")

def main():
    json_dir, csv_dir = define_directories()
    convert_json_to_csv(json_dir, csv_dir)

if __name__ == "__main__":
    main()