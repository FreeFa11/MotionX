from _1open_csv import open_csv
import pandas as pd
from pathlib import Path
from save_file import SaveFile
import logging as log

def setup_logging():
    logs_dir = Path(__file__).parent / ".." / "Logs" / "Standard_Logs"
    logs_dir.mkdir(parents=True,exist_ok=True)

    #Logging Configuration
    log.basicConfig(
        level = log.INFO,
        format="%(asctime)s - %(levelname)s - %(message)s",
        handlers = [
            log.FileHandler(logs_dir / "standard_data.log"),
            log.StreamHandler()
        ]
    )
    log.info(f"Logging successfully configured on {logs_dir}")

def add_columns(data):
    accelero_columns = ["AcceleroX", "AcceleroY", "AcceleroZ"]

    for i, column in enumerate(accelero_columns):
        if column not in data.columns:
            log.warning(f"Missing column {column}. Adding {column} at index {4 + i}.")
            data.insert(4 + i, column, 0)
    
    return data

def standardize_data(filename):
    try:
        data = open_csv(filename)
        log.info(f"Successfully opened {filename} for standardization")

        standardize_data = add_columns(data)

        standardized_filename = filename.replace("_cleaned.csv", "_standard.csv")

        standardized_content = standardize_data.to_csv(index = False, lineterminator = "\n")

        filesaver = SaveFile(
            filename = standardized_filename,
            content = standardized_content,
            file_type = "standard"
        )

        filesaver.save()


        log.info(f"Standardized data successfully saved to {filename}")

    except Exception as e:
       log.error(f"Unexpected error occured: {e}") 
       raise

def standardize():
    setup_logging()
    try:
        clean_dir = Path(__file__).parent / ".."/ "Data" / "Cleaned_Data"
        clean_dir = clean_dir.resolve()
        if not clean_dir.exists():
            log.error(f"{clean_dir} doesn't exist")
            return
        
        clean_files = list(clean_dir.glob("*_cleaned.csv"))
        
        if not clean_files:
            log.error(f"No cleand files in {clean_dir}")
            return
        
        for file in clean_files:
            standardize_data(str(file.name))
            log.info(f"Starting to standardize {file}.")
    except Exception as e:
        log.error(f"Script Execution Failed: {e}.")


def main():
    standardize()

if __name__ == "__main__":
    main()