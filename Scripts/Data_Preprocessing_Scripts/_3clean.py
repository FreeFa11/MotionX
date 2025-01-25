from _1open import open_csv
import numpy as np
from sklearn.impute import SimpleImputer
import pandas as pd
import os

def clean_data(filename):
    try:
        data = open_csv(filename)
        imputer = SimpleImputer(missing_values=np.nan, strategy="mean")
        data_imputed = pd.DataFrame(imputer.fit_transform(data), columns=data.columns)
        filename = filename.replace(".csv", "")
        save_csv(data_imputed, f"{filename}cleaned.csv")
        return data_imputed
    except FileNotFoundError: 
        raise FileNotFoundError("The file is not there.")
    except pd.errors.EmptyDataError:
        raise pd.errors.EmptyDataError("The file is empty.")
    except pd.errors.ParserError:
        raise pd.errors.ParserError("The file is not in .csv format")
    except KeyError as e:
        raise KeyError(f"Missing expected columns as {e}")
    except ValueError as e:
        raise ValueError(f"Invalid data format or invalid imputation as {e}")
    except Exception as e:
        raise Exception(f"Unexpected error occured as {e}")

def save_csv(cleaned_data, file):
    try:
        current_path = os.path.dirname(__file__)
        csv_path = os.path.abspath(os.path.join(current_path,"..", "Data", "Raw", "csvRaw"))
        if not os.path.exists(csv_path):
            os.makedirs(csv_path)
        file_path = os.path.join(csv_path.replace(".csv", ""), file)
        cleaned_data.to_csv(file_path, index=False)

    except FileNotFoundError:
        raise FileNotFoundError("File doesn't exits")
    except PermissionError:
        raise PermissionError("Permission Denied.")
    except AttributeError:
        raise AttributeError("Not a Valid Pandas DataFrame.")
    except ValueError as e:
        raise(f"Invalid data format as {e}.")
    except Exception as e:
        raise(f"Unexpected error occured as {e}")
    
clean_data("RightClick.csv")