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
    """Get the top k shows by vote_count."""
    # Define the CSV file path for the top_k_shows
    script_directory = os.path.dirname(os.path.realpath(__file__))
    top_k_shows_csv = os.path.join(script_directory, 'top_k_shows.csv')

    # Check if the CSV already exists
    if os.path.exists(top_k_shows_csv):
        print(f"Loading top {k} shows from existing CSV file.")
        df = pd.read_csv(top_k_shows_csv)
    else:
        # Get the dataset using the get_dataset function
        df = get_dataset()
        if df is None:
            return None

        # Sort the DataFrame by 'vote_count' in descending order
        df = df.sort_values(by='vote_count', ascending=False).head(k)

        # Save the top k shows to a CSV file in the same directory as the script
        df.to_csv(top_k_shows_csv, index=False)
        print(f"Top {k} shows saved to '{top_k_shows_csv}'.")

    return df  # Return the top_k_shows DataFrame for further processing

def check_missing_overviews(df):
    """Function to check how many shows have no overview"""
    # Check for NaN values in the 'overview' column
    missing_overview_count = df['overview'].isna().sum()
    
    # Check for empty strings in the 'overview' column
    empty_overview_count = df['overview'].eq('').sum()

    # Total count of shows with no overview (either NaN or empty string)
    total_missing_overview = missing_overview_count + empty_overview_count

    print(f"Total number of shows with no overview: {total_missing_overview}")


def remove_shows_without_overview():
    """Remove shows without an overview from the CSV."""
    # Define the CSV file path for the top_k_shows
    script_directory = os.path.dirname(os.path.realpath(__file__))
    top_k_shows_csv = os.path.join(script_directory, 'top_k_shows.csv')

    if not os.path.exists(top_k_shows_csv):
        print(f"CSV file '{top_k_shows_csv}' does not exist.")
        return None

    # Load the top_k_shows CSV file
    df = pd.read_csv(top_k_shows_csv)

    # Remove rows where 'overview' is NaN or empty
    cleaned_df = df[df['overview'].notna() & (df['overview'] != '')]

    # Save the cleaned DataFrame back to the CSV
    cleaned_df.to_csv(top_k_shows_csv, index=False)

    print(f"Shows without an overview removed. Cleaned data saved to '{top_k_shows_csv}'.")


# Get the top k shows by vote_count
top_k_shows = get_top_k_shows(10000)

if top_k_shows is not None:
    check_missing_overviews(top_k_shows)

    remove_shows_without_overview()
