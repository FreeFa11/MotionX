from _1open_csv import open_csv
import pandas as pd
import logging as log
from sklearn.impute import SimpleImputer
from pathlib import Path
import numpy as np
from scipy.signal import butter, filtfilt
from save_file import SaveFile

def setup_logging():
    logs_dir = Path(__file__).parent / ".." / "Logs" / "Clean_Logs"
    logs_dir.mkdir(parents=True,exist_ok=True)

    #Logging Configuration
    log.basicConfig(
        level = log.INFO,
        format="%(asctime)s - %(levelname)s - %(message)s",
        handlers = [
            log.FileHandler(logs_dir / "clean_data.log"),
            log.StreamHandler()
        ]
    )
    log.info(f"Logging successfully configured on {logs_dir}")

def butterworth_low_pass_filter(data, cutoff, fs, order):
    #Nyquist Frequency is always half the sampling frequency
    nyquist_frequency = 0.5 * fs

    #Normalized cutoff frequency
    normal_cutoff = cutoff/nyquist_frequency

    #Butterworth Filter
    b, a = butter(order, normal_cutoff, btype = "low", analog=False)

    #Using filtfilt to remove any phase distortion faced during the application of filter. Works by filtering twice once from front and one from back
    return filtfilt(b, a, data) 

def clean_data(filename, cutoff = 10, fs = 100, order = 5):
    try:
        data = open_csv(filename)
        log.info(f"Successfully opened {filename} for cleaning.")


        imputer = SimpleImputer(missing_values=np.nan, strategy="median")
        data_imputed = pd.DataFrame(imputer.fit_transform(data), columns=data.columns)

        unfiltered_columns = ["Index", "Middle", "Ring"]

        for column in unfiltered_columns:
            if column in data_imputed.columns:
                try:
                    filtered_values = butterworth_low_pass_filter(data_imputed[column], cutoff, fs, order)
                    data_imputed[column] = filtered_values
                    log.info(f"Successfully filtered the {column} using Butterworth Low-Pass Filter.")
                except Exception as e:
                    log.error(f"Error filtering {column} as {e}.")
                    raise
        
        cleaned_filename = filename.replace("_raw.csv", "_cleaned.csv")

        cleaned_content = data_imputed.to_csv(index=False, lineterminator="\n")

        fileSaver = SaveFile(
            filename=cleaned_filename,
            content = cleaned_content,
            file_type="cleaned"
        )

        fileSaver.save()

        log.info(f"Successfully cleaned {filename} and saved as {cleaned_filename}")

    except Exception as e:
        log.error(f"Error Cleaning Data: {e}")
        raise

def clean():
    setup_logging()
    try:
        raw_dir = Path(__file__).parent / ".."/ "Data" / "Raw_Data" / "csvRaw"
        raw_dir = raw_dir.resolve()
        if not raw_dir.exists():
            log.error(f"{raw_dir} doesn't exist")
            return
        
        raw_files = list(raw_dir.glob("*_raw.csv"))
        
        if not raw_files:
            log.error(f"No raw files in {raw_dir}")
            return
        
        for file in raw_files:
            clean_data(str(file.name))
            log.info(f"Starting to clean {file}.")
    except Exception as e:
        log.error(f"Script Execution Failed: {e}.")

def main():
    clean()

if __name__ == "__main__":
    main()