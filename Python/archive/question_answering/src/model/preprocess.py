import os
import re

def preprocess_data(input_folder='dataset', output_file='preprocessed_data.txt'):
    """
    Preprocesses text files in the input folder by extracting questions and answers,
    formats them, and saves the output to a specified file.

    Args:
        input_folder (str): Folder containing the input text files.
        output_file (str): File to save the preprocessed data.
    """

    # Check if the output file exists; if so, delete it
    if os.path.exists(output_file):
        os.remove(output_file)
        print(f"Existing file '{output_file}' deleted.")

    # Initialize an empty list to hold formatted data
    formatted_data = []

    # Loop through each file in the input folder
    for filename in os.listdir(input_folder):
        if filename.endswith('.txt'):
            file_path = os.path.join(input_folder, filename)
            
            with open(file_path, 'r', encoding='utf-8') as file:
                # Read the content of the file
                content = file.read()
                
                # Use regex to find questions and answers
                question_pattern = r'Question: (.*?)\n'
                answer_pattern = r'Answer: (.*?)(?=\nQuestion:|\Z)'
                
                questions = re.findall(question_pattern, content)
                answers = re.findall(answer_pattern, content, re.DOTALL)
                
                # Loop through the found questions and answers
                for question, answer in zip(questions, answers):
                    
                    # Append formatted data to the list
                    formatted_data.append(f"Q: {question}\nA: {answer}\n")

    # Write the formatted data to the output file
    with open(output_file, 'w', encoding='utf-8') as output:
        output.writelines(formatted_data)

    print(f"Preprocessing complete! Data saved to {output_file}")