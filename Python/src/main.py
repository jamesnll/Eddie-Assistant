import os
import argparse
from model.preprocess import preprocess_data  # Import preprocessing function
from model.train import train_model  # Import training function

def main(preprocess, train):
    """
    Main script for preprocessing dataset files.

    This script determines the absolute path to the dataset folder, then calls the 
    `preprocess_data` function from the `model.preprocess` module to preprocess 
    the data contained within that folder. The preprocessed data is saved to 
    a text file named 'preprocessed_data.txt' in the same dataset folder.

    Ensure that the dataset is structured correctly before running this script.
    """

    if preprocess:

        # Get the absolute path to the current directory of main.py
        base_dir = os.path.dirname(os.path.abspath(__file__))
        
        # Construct the absolute path to the dataset folder (up one level)
        input_folder = os.path.join(base_dir, '..', 'dataset')
        output_file = os.path.join(base_dir, '..', 'dataset', 'preprocessed_data.txt')
        print("Preprocessing data...")
        preprocess_data(input_folder=input_folder, output_file=output_file)

    if train:
        print("Training the model...")
        train_model()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run preprocessing and/or training.')
    parser.add_argument('--preprocess', action='store_true', help='Run data preprocessing')
    parser.add_argument('--train', action='store_true', help='Run model training')

    args = parser.parse_args()

    main(args.preprocess, args.train)
