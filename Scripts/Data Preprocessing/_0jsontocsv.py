import json
from pathlib import Path
import os
import pandas as pd
import logging as log

def setup_logging():
    # Defining the Path for the logs directory
    logs_dir = Path(__file__).parent / ".." / "Logs" / "Json_to_Csv_logs"
    # Making sure the parents for the log directory exist
    logs_dir.mkdir(parents=True, exist_ok=True)
    
    # Logging Configuration
    log.basicConfig(
        level=log.INFO,
        format="%(asctime)s - %(levelname)s - %(message)s",
        handlers=[
            log.FileHandler(logs_dir / "json_to_csv.log"),
            log.StreamHandler()
        ]
    )
    log.info("Logging Configuration Successful.")

def define_directories():
    try:
        base_dir = Path(__file__).parent
        raw_json = base_dir / ".." / "Data" / "Raw_Data" / "jsonRaw"
        raw_csv = base_dir / ".." / "Data" / "Raw_Data" / "csvRaw"

        # Resolve paths and create csv directory if it doesn't exist
        raw_json.resolve()
        raw_csv.resolve()
        raw_csv.mkdir(parents=True, exist_ok=True)

        log.info(f"JSON directory: {raw_json}")
        log.info(f"CSV directory: {raw_csv}")
        return raw_json, raw_csv
    
    except Exception as e:
        log.error(f"Error defining directories: {e}")
        raise

def file_path(json_dir):
    try:
        json_files = list(json_dir.glob("*.json"))
        log.info(f"{len(json_files)} json files found in {json_dir}")
        return json_files
    except Exception as e:
        log.error(f"Error! No .json files found in the {json_dir} directory: {e}")
        raise

def convert_json_to_csv(json_dir, csv_dir):
    try:
        json_files = file_path(json_dir)
        count = len(json_files)
        
        for j in json_files:
            try:
                log.info(f"Processing {j.name} currently")
                with open(j, "r") as file:
                    data = json.load(file)
                
                interval_ms = data["payload"]["interval_ms"]
                values = data["payload"]["values"]
                # Reinitialize parameters and timestamps for each file
                parameters = [sensor["name"] for sensor in data["payload"]["sensors"]]
                timestamps = [i * interval_ms for i in range(len(values))]
                
                df = pd.DataFrame(values, columns=parameters)
                df.insert(0, "timestamp", timestamps)
                
                name_parts = j.stem.split('.')
                if len(name_parts) >= 2:
                    base_name = f"{name_parts[0]}_{name_parts[1]}_{name_parts[-1]}"
                else:
                    base_name = j.stem
                
                csv_filename = f"{base_name}_raw.csv"
                output_path = csv_dir / csv_filename
                df.to_csv(output_path, index=False)
                log.info(f"Resulting csv file {csv_filename} successfully saved to {output_path}")
            
            except KeyError as e:
                log.error(f"Invalid key in file {j}: {e}")
                raise
            except ValueError as e:
                log.error(f"Invalid value in the file {j}: {e}")
                raise
            except Exception as e:
                log.error(f"Unexpected error occurred: {e}")
                raise
        
        return count

    except FileNotFoundError:
        log.error("The directory doesn't exist.")
        raise
    except pd.errors.EmptyDataError:
        log.error("The file is empty.")
        raise
    except json.JSONDecodeError:
        log.error("The json file is not in the correct format.")
        raise
    except Exception as e:
        log.error(f"Unexpected error occurred: {e}")
        raise

def json_to_csv():
    try:
        setup_logging()
        log.info("Starting JSON to CSV conversion.")
        json_dir, csv_dir = define_directories()
        # Correctly call the conversion function and get the count
        count = convert_json_to_csv(json_dir, csv_dir)
        log.info(f"Successfully converted {count} JSON files.")
    except Exception as e:
        log.error(f"Script running failed: {e}")

def main():
    json_to_csv()

if __name__ == "__main__":
    main()
