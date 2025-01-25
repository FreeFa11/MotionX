from _1open import open_csv

def data_review(filename):
    data = open_csv(filename)

    #Prints the first n rows of the data DataFrame, n = 5 by default
    print(data.head())

    #Prints the basis statistical information about the data
    print(data.describe())

    #Prints the number of null values in the data
    print(data.isnull().sum())

def main():
    data_review("RightClick.csv")

if __name__ == "__main__":
    main()