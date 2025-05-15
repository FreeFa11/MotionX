from _1open_csv import open_csv as open
import logging as log
from save_file import SaveFile
from pathlib import Path
import pandas as pd

def setup_logging():
    logs_dir = Path(__file__).parent / ".." / "Logs" / "Combine_Logs"
    logs_dir.mkdir(parents=True,exist_ok=True)

    #Logging Configuration
    log.basicConfig(
        level = log.INFO,
        format="%(asctime)s - %(levelname)s - %(message)s",
        handlers = [
            log.FileHandler(logs_dir / "combined_data.log"),
            log.StreamHandler()
        ]
    )
    log.info(f"Logging successfully configured on {logs_dir}")

def combine_labeled_csv(files):
    try:
        combined_list = []
        for file in files:
            print(file)
            data = open(file)
            combined_list.append(data)

        #Initializing the combined dataframe and combining the datasets 

        combined_df = pd.concat(combined_list, ignore_index=True).sort_values("label", ascending=True)
        log.info(f"{len(combined_list)} files have successfully been combined.")

        combined_filename = "Gestures_combined.csv"
        combined_content = combined_df.to_csv(index = False, lineterminator = "\n")

        fileSaver = SaveFile(
            filename = combined_filename,
            content = combined_content,
            file_type = "combined"
        )

        fileSaver.save()

        log.info(f"Successfully combined the files and saved as {combined_filename}")

    except Exception as e:
        log.error(f"Unable to combine CSV files: {e}")
        raise

def combine():
    setup_logging()
    try:
        combined_list = []
        labeled_dir = Path(__file__).parent / ".."/ "Data" / "Labeled_Data"
        labeled_dir = labeled_dir.resolve()
        if not labeled_dir.exists():
            log.error(f"{labeled_dir} doesn't exist")
            return
        
        labeled_files = list(labeled_dir.glob("*_labeled.csv"))
        
        if not labeled_files:
            log.error(f"No labeled files in {labeled_dir}")
            return
        
        for file in labeled_files:
            combined_list.append(str(file))
            log.info(f"Starting to combine {file}.")

        combine_labeled_csv(combined_list)

    except Exception as e:
        log.error(f"Script Execution Failed: {e}.")
        raise

    combine_labeled_csv(combined_list)

def main():
    combine()

if __name__ == "__main__":
    main()