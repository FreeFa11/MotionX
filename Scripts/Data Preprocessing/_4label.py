from _1open_csv import open_csv as open
import logging as log
from pathlib import Path
from save_file import SaveFile
import pandas as pd

def setup_logging():
    logs_dir = Path(__file__).parent / ".." / "Logs" / "Label_Logs"
    logs_dir.mkdir(parents=True,exist_ok=True)

    #Logging Configuration
    log.basicConfig(
        level = log.INFO,
        format="%(asctime)s - %(levelname)s - %(message)s",
        handlers = [
            log.FileHandler(logs_dir / "labeled_data.log"),
            log.StreamHandler()
        ]
    )
    log.info(f"Logging successfully configured on {logs_dir}")

def assign_labels():
    return {
        "leftclick" : 0,
        "rightclick" : 1,
        "middleclick" : 2
    }

def extract_gesture(filename):
    gesture_label = assign_labels()
    for gesture in gesture_label:
        if gesture.lower() in filename.lower():
            return gesture
    
    return "unknown"

def label_data(data,gesture):
    gesture_key = gesture
    gesture_labels = assign_labels()

    if gesture_key in gesture_labels:
        label = gesture_labels[gesture_key]
        log.info(f"Assigned {label} for {gesture}.")
    else:
        log.warning(f"No matching label for {gesture}")

    data["label"] = label

    return data

def label_dataset(filename):
    try:
        data = open(filename)
        log.info(f"Opening {filename} and starting to label it.")

        gesture = extract_gesture(filename)
        labeled_data = label_data(data, gesture)

        labeled_filename = filename.replace("_standard.csv", "_labeled.csv")

        labeled_content = labeled_data.to_csv(index = False, lineterminator="\n")

        fileSaver = SaveFile(
            filename = labeled_filename,
            content = labeled_content,
            file_type = "label"
        )

        fileSaver.save()

        log.info(f"Successfully labeled and then saved as {labeled_filename}.")

    except Exception as e:
        log.error(f"Error labeling {filename}: {e}")

def label():
    setup_logging()
    try:
        standard_dir = Path(__file__).parent / ".."/ "Data" / "Standard_Data"
        standard_dir = standard_dir.resolve()
        if not standard_dir.exists():
            log.error(f"{standard_dir} doesn't exist")
            return
        
        standard_files = list(standard_dir.glob("*_standard.csv"))
        
        if not standard_files:
            log.error(f"No standardized files in {standard_dir}")
            return
        
        for file in standard_files:
            label_dataset(str(file.name))
            log.info(f"Starting to Label {file}.")
    except Exception as e:
        log.error(f"Script Execution Failed: {e}.")


def main():
    label()

if __name__ == "__main__":
    main()