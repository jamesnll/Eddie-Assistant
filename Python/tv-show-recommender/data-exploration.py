import os
import zipfile
import pandas as pd
from kaggle.api.kaggle_api_extended import KaggleApi

def get_dataset():
    # Initialize the Kaggle API
    api = KaggleApi()
    api.authenticate()

    # Define the dataset
    dataset = 'asaniczka/full-tmdb-tv-shows-dataset-2023-150k-shows'

    # Define the destination directory
    destination_directory = os.path.join(os.getcwd(), 'temp')

    # Make sure the destination directory exists
    os.makedirs(destination_directory, exist_ok=True)

    # Download the dataset as a zip file
    print(f"Downloading dataset: {dataset}")
    api.dataset_download_files(dataset, path=destination_directory, unzip=False)

    # Locate the zip file in the destination directory
    zip_file_path = os.path.join(destination_directory, 'full-tmdb-tv-shows-dataset-2023-150k-shows.zip')

    # Extract the CSV file from the zip
    with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
        # List all files in the zip to check what was extracted
        extracted_files = zip_ref.namelist()
        print(f"Extracted files: {extracted_files}")
        
        # Extract the CSV file
        for file in extracted_files:
            if file.endswith('.csv'):
                with zip_ref.open(file) as csv_file:
                    # Load the CSV into a pandas DataFrame
                    df = pd.read_csv(csv_file)
                    print(f"Found and extracted {file}")
                    break
        else:
            print("No CSV file found in the archive.")
            return None

    # After extracting, delete the zip file and temp directory
    os.remove(zip_file_path)
    os.rmdir(destination_directory)

    # Return the dataset as a pandas DataFrame
    return df
