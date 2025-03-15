import os
from kaggle.api.kaggle_api_extended import KaggleApi

def get_dataset():

    # Initialize the Kaggle API
    api = KaggleApi()
    api.authenticate()

    # Define the dataset and the new destination directory
    dataset = 'asaniczka/full-tmdb-tv-shows-dataset-2023-150k-shows'
    destination_directory = os.path.join(os.getcwd(), 'Python', 'tv-show-recommender')

    # Make sure the destination directory exists, create it if it doesn't
    os.makedirs(destination_directory, exist_ok=True)

    # Download the dataset
    api.dataset_download_files(dataset, path=destination_directory, unzip=True)

    # Check the extracted files and rename the correct one
    # List all files in the destination directory
    extracted_files = os.listdir(destination_directory)

    # Print extracted files to see the actual file names
    print(f"Extracted files: {extracted_files}")

    # Find the CSV file and rename it
    for file in extracted_files:
        if file.endswith('.csv'):
            # Rename the first CSV file found to 'tv_shows.csv'
            os.rename(os.path.join(destination_directory, file), 
                    os.path.join(destination_directory, 'tv_shows.csv'))
            print(f"Renamed {file} to tv_shows.csv")
            break
    else:
        print("No CSV file found to rename.")
