from _1open_csv import open_csv as open
from save_file import SaveFile
from pathlib import Path
import logging as log
import pandas as pd
from sklearn.model_selection import train_test_split


def setup_logging():
    logs_dir = Path(__file__).parent / ".." / "Logs" / "Split_Logs"
    logs_dir.mkdir(parents=True,exist_ok=True)

    #Logging Configuration
    log.basicConfig(
        level = log.INFO,
        format="%(asctime)s - %(levelname)s - %(message)s",
        handlers = [
            log.FileHandler(logs_dir / "splitted_data.log"),
            log.StreamHandler()
        ]
    )
    log.info(f"Logging successfully configured on {logs_dir}")


def split_data(filename, test_split = 0.2, random_state = 40):
    try:
        data = open(filename)
        log.info(f"Successfully opened {filename} for splitting into training and testing set.")

        if "label" not in data.columns:
            log.error(f"Label not found in the file {filename}.")
            return
         
        
        train_data, test_data = train_test_split(
            data,
            test_size = test_split,
            stratify = data["label"],
            random_state = random_state
        )

        timestamps_train = (train_data["timestamp"]/1000).tolist()
        Y_Train = pd.DataFrame(train_data["label"])

        Y_Train.insert(0,"timestamp",timestamps_train)
        
        X_Train = train_data.drop(columns = "label")

        timestamps_test = (test_data["timestamp"]/1000).tolist()

        Y_Test = pd.DataFrame(test_data["label"])

        Y_Test.insert(0,"timestamp",timestamps_test)

        print(Y_Test.head())
        X_Test = test_data.drop(columns = "label")
        

    
        log.info(f"Data from {filename} successfully split into {X_Train.shape} and {Y_Train.shape} training set into {X_Test.shape} and {Y_Test.shape} testing set.")

        X_train_filename = filename.replace("_combined.csv", "_Xtrain.csv")
        X_test_filename = filename.replace("_combined.csv", "_Xtest.csv")

        Y_train_filename = filename.replace("_combined.csv", "_Ytrain.csv")
        Y_test_filename = filename.replace("_combined.csv", "_Ytest.csv")
        
        X_train_content = X_Train.to_csv(index = False, lineterminator = "\n")
        X_test_content = X_Test.to_csv(index = False, lineterminator = "\n")

        Y_train_content = Y_Train.to_csv(index = False, lineterminator = "\n")
        Y_test_content = Y_Test.to_csv(index = False, lineterminator = "\n")
        
        log.info("Saving the X_train data")
        XtrainSaver = SaveFile(
            filename = X_train_filename,
            content = X_train_content,
            file_type = "train"
        )

        XtrainSaver.save()

        log.info("Saving the X_test data")


        XtestSaver = SaveFile(
            filename = X_test_filename,
            content = X_test_content,
            file_type = "test"
        )

        XtestSaver.save()

        log.info("Saving the Y_train data")
        YtrainSaver = SaveFile(
            filename = Y_train_filename,
            content = Y_train_content,
            file_type = "train"
        )

        YtrainSaver.save()

        log.info("Saving the Y_test data")
        YtestSaver = SaveFile(
            filename = Y_test_filename,
            content = Y_test_content,
            file_type = "test"
        )

        YtestSaver.save()

        log.info(f"Features Training Set saved as {X_train_filename} and Testing Set saved as {X_test_filename}.")
        log.info(f"Target Training Set saved as {Y_train_filename} and Testing Set saved as {Y_test_filename}.")
    except Exception as e:
        log.error(f"Error Splitting the data from {filename}: {e}")

def split():
    setup_logging()
    try:
        combined_dir = Path(__file__).parent / ".."/ "Data" / "Combined_Data"
        combined_dir = combined_dir.resolve()
        if not combined_dir.exists():
            log.error(f"{combined_dir} doesn't exist")
            return
        
        combined_files = list(combined_dir.glob("*_combined.csv"))
        
        if not combined_files:
            log.error(f"No combined files in {combined_dir}")
            return
        
        for file in combined_files:
            split_data(str(file))
            
    except Exception as e:
        log.error(f"Script Execution Failed: {e}.")
        raise

def main():
    split()

if __name__ == "__main__":
    main()        