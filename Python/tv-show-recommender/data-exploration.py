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

def get_top_k_shows(k):
    # Get the dataset using the get_dataset function
    df = get_dataset()

    if df is None:
        return None

    # Sort the DataFrame by 'vote_count' in descending order
    top_k_shows = df.sort_values(by='vote_count', ascending=False).head(k)

    # Get the directory of the executing script
    script_directory = os.path.dirname(os.path.realpath(__file__))

    # Save the top k shows to a CSV file in the same directory as the script
    top_k_shows.to_csv(os.path.join(script_directory, 'top_k_shows.csv'), index=False)

    print(f"Top {k} shows saved to '{script_directory}\\top_k_shows.csv'.")

# Get the top 5 shows by vote_count
get_top_k_shows(5)