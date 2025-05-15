from _1open_csv import open_csv as open
from save_file import SaveFile
import pandas as pd
from pathlib import Path
import logging as log
from sklearn.preprocessing import MinMaxScaler

def setup_logging():
    logs_dir = Path(__file__).parent / ".." / "Logs" / "Scaled_Logs"
    logs_dir.mkdir(parents=True, exist_ok=True)

    log.basicConfig(
        level=log.INFO,
        format="%(asctime)s - %(levelname)s - %(message)s",
        handlers=[
            log.FileHandler(logs_dir / "scaled_data.log"),
            log.StreamHandler()
        ]
    )
    log.info(f"Logging successfully configured on {logs_dir}")

def scale_Xtrain_data(filename, scaler):
    try:
        data = open(filename)
        log.info("Using MinMaxScaler for X_train data.")
        # Exclude the timestamp column from scaling
        timestamps = (data["timestamp"] / 1000).tolist()
        data = data.drop(columns="timestamp")
        
        scaled_df = pd.DataFrame(scaler.fit_transform(data), columns=data.columns)
        scaled_df.insert(0, "timestamp", timestamps)

        scaled_filename = filename.replace("_Xtrain.csv", "_Xtrain_final.csv")
        scaled_content = scaled_df.to_csv(index=False, lineterminator="\n")

        scaled_Saver = SaveFile(
            filename=scaled_filename,
            content=scaled_content,
            file_type="final_train"
        )
        scaled_Saver.save()
        
        log.info(f"File scaled and successfully saved as {scaled_filename}")

    except Exception as e:
        log.error(f"Unable to scale {filename}: {e}")

def scale_Ytrain_data(filename):
    try:
        data = open(filename)
        log.info(f"Opened Y_train file {filename} for final saving.")

        Y_filename = filename.replace("_Ytrain.csv", "_Ytrain_final.csv")
        Y_content = data.to_csv(index=False, lineterminator="\n")

        scaled_Saver = SaveFile(
            filename=Y_filename,
            content=Y_content,
            file_type="final_train"
        )
        scaled_Saver.save()
    
    except Exception as e:
        log.error(f"Unable to process {filename}: {e}")

def scale_Xtest_data(filename, scaler):
    try:
        data = open(filename)
        log.info(f"Opened X_test file {filename} for scaling and final saving.")
        
        timestamps = (data["timestamp"] / 1000).tolist()
        data = data.drop(columns="timestamp")
        scaled_data = pd.DataFrame(scaler.transform(data), columns=data.columns)

        scaled_data.insert(0, "timestamp", timestamps)
        X_filename = filename.replace("_Xtest.csv", "_Xtest_final.csv")
        scaled_content = scaled_data.to_csv(index=False, lineterminator="\n")

        scaled_Saver = SaveFile(
            filename=X_filename,
            content=scaled_content,
            file_type="final_test"
        )
        scaled_Saver.save()

    except Exception as e:
        log.error(f"Unable to process {filename}: {e}")

def scale_Ytest_data(filename):
    try:
        data = open(filename)
        log.info(f"Opened Y_test file {filename} for final saving.")

        Y_filename = filename.replace("_Ytest.csv", "_Ytest_final.csv")
        Y_content = data.to_csv(index=False, lineterminator="\n")

        scaled_Saver = SaveFile(
            filename=Y_filename,
            content=Y_content,
            file_type="final_test"
        )
        scaled_Saver.save()

    except Exception as e:
        log.error(f"Unable to process {filename}: {e}")

def scale():
    setup_logging()
    try:
        Train_dir = Path(__file__).parent / ".." / "Data" / "Split_Data" / "Training_Data"
        Test_dir = Path(__file__).parent / ".." / "Data" / "Split_Data" / "Testing_Data"

        Train_dir = Train_dir.resolve()
        Test_dir = Test_dir.resolve()

        if not Train_dir.exists():
            log.error(f"{Train_dir} doesn't exist.")
            return

        if not Test_dir.exists():
            log.error(f"{Test_dir} doesn't exist.")
            return

        X_train_files = list(Train_dir.glob("*_Xtrain.csv"))
        Y_train_files = list(Train_dir.glob("*_Ytrain.csv"))
        X_test_files = list(Test_dir.glob("*_Xtest.csv"))
        Y_test_files = list(Test_dir.glob("*_Ytest.csv"))

        # Instantiate MinMaxScaler with the desired feature range (-1, 1)
        scaler = MinMaxScaler(feature_range=(-1, 1))

        for file in X_train_files:
            scale_Xtrain_data(str(file), scaler)
        for file in Y_train_files:
            scale_Ytrain_data(str(file))
        for file in X_test_files:
            scale_Xtest_data(str(file), scaler)
        for file in Y_test_files:
            scale_Ytest_data(str(file))

    except Exception as e:
        log.error(f"Error running the scaling script: {e}")

def main():
    scale()

if __name__ == "__main__":
    main()
