from pathlib import Path
import os
import pandas as pd
import logging as log

def setup_logging():

    #Defining the Path for the logs directory
    logs_dir = Path(__file__).parent / ".." / "Logs" / "Open_File"

    #Making sure the parents for the log directory and the Log directory exists
    logs_dir.mkdir(parents=True, exist_ok=True)

    #Logging Configuration
    log.basicConfig(
        level = log.INFO,
        format = "%(asctime)s - %(levelname)s - %(message)s",
        handlers =[
            log.FileHandler(logs_dir / "open_csv.log"),
            log.StreamHandler()
        ]
    )
    log.info("Logging Configuration Successful.")


def open_csv(filename):
    try:    
        base_path = Path(__file__).parent
        
        dir_mapping = {
            "_cleaned.csv" :base_path/ ".." / "Data" / "Cleaned_Data",
            "_combined.csv" :base_path/ ".." / "Data" / "Combined_Data",
            "train_final.csv": base_path / ".." / "Data" / "Final_Data" / "Training_Data",
            "test_final.csv": base_path / ".." / "Data" / "Final_Data" / "Testing_Data",
            "_labeled.csv": base_path / ".." / "Data" / "Labeled_Data",
            "_raw.csv": base_path / ".." / "Data" / "Raw_Data" / "csvRaw",
            "_normalized.csv": base_path / ".." / "Data" / "Final_Data" / "Normalized_Data"/"csv", 
            "_scaled.csv": base_path / ".." / "Data" / "Scaled_Data",
            "_standard.csv": base_path / ".." / "Data" / "Standard_Data",
            "_Xtest.csv": base_path / ".." / "Data" / "Split_Data" / "Testing_Data",
            "_Ytest.csv": base_path / ".." / "Data" / "Split_Data" / "Testing_Data",
            "_Xtrain.csv": base_path / ".." / "Data" / "Split_Data" / "Training_Data",
            "_Ytrain.csv": base_path / ".." / "Data" / "Split_Data" / "Training_Data",
        }

        csv_path = None
        for ext, dir_name in dir_mapping.items():
            if filename.endswith(ext):
                csv_path = dir_name / filename
                break

        if csv_path is None:
            raise ValueError(f"Invalid File! File {filename} does not match any known pattern.")

        data = pd.read_csv(csv_path)
        log.info(f"Successfully Opened {csv_path}")
        return data
        
    except FileNotFoundError as e:
        log.error(f"Couldn't find the file {csv_path}: {e}")
        raise
    except pd.errors.ParserError as e:
        log.error(f"Unable to parse the file {filename}: {e}")
        raise
    except pd.errors.EmptyDataError:
        log.error(f"The .csv file {filename} is empty.")
        raise
    except PermissionError:
        log.error(f"Permission denied for the file {filename}")
        raise
    except Exception as e:
        log.error(f"Unexpected error occurred while trying to open the csv file {filename}: {e}")
        raise

def open():
    try:
        setup_logging()
        log.info("Starting CSV file opening process.")
        data = open_csv("Clap_5k8pfvjm_s1_normalized.csv")  
        log.info("CSV file opened successfully.")
        print(data.describe())
    except Exception as e:
        log.error(f"Script running failed: {e}")

def main():
    open()


if __name__ == "__main__":
    main()