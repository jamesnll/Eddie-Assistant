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


def filter_overview_length(min_words=25):
    """Filter out shows that have less than `min_words` words in their overview."""
    # Define the CSV file path for the top_k_shows
    script_directory = os.path.dirname(os.path.realpath(__file__))
    top_k_shows_csv = os.path.join(script_directory, 'top_k_shows.csv')

    if not os.path.exists(top_k_shows_csv):
        print(f"CSV file '{top_k_shows_csv}' does not exist.")
        return None

    # Load the top_k_shows CSV file
    df = pd.read_csv(top_k_shows_csv)

    # Function to count the number of words in an overview
    def count_words(overview):
        return len(str(overview).split())

    # Filter the DataFrame by the number of words in the 'overview' column
    df_filtered = df[df['overview'].apply(count_words) >= min_words]

    # Save the filtered DataFrame back to the CSV file
    df_filtered.to_csv(top_k_shows_csv, index=False)

    # Print a message with the count of rows removed
    removed_count = len(df) - len(df_filtered)
    print(f"Filtered out {removed_count} shows with less than {min_words} words in the overview.")
    
    return df_filtered


def add_tagged_overview(filtered_df):
    """Add a 'tagged_overview' field to the DataFrame and save as 'top_k_shows_cleaned.csv'."""
    # Ensure required columns exist
    if 'id' not in filtered_df.columns or 'overview' not in filtered_df.columns:
        print("Required columns ('id', 'overview') not found in the dataset.")
        return None

    # Create the new 'tagged_overview' column
    filtered_df['tagged_overview'] = filtered_df['id'].astype(str) + ' ' + filtered_df['overview'].astype(str)

    # Define the output file path
    script_directory = os.path.dirname(os.path.realpath(__file__))
    cleaned_csv_path = os.path.join(script_directory, 'top_k_shows_cleaned.csv')

    # Save the modified DataFrame to a new CSV file
    filtered_df.to_csv(cleaned_csv_path, index=False)

    print(f"Updated dataset saved as '{cleaned_csv_path}'.")
    return cleaned_csv_path


# Get the top k shows by vote_count
top_k_shows = get_top_k_shows(10000)

if top_k_shows is not None:
    check_missing_overviews(top_k_shows)

    remove_shows_without_overview()

    filtered_df = filter_overview_length(min_words=25)

    cleaned_csv_path = add_tagged_overview(filtered_df)
