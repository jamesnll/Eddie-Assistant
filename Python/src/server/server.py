# Imports
from flask import Flask, request, jsonify
import os
import sys

# Add src to the path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from model.inference import generate_text # Import inference function

app = Flask(__name__)

@app.route('/')
def home():
    return "Hello from the Flask server!"

# New route to generate text
@app.route('/ask-question', methods=['POST'])
def ask_question():

    # Get the absolute path to the current directory of server.py
    base_dir = os.path.dirname(os.path.abspath(__file__))

    data = request.json
    model_path = os.path.join(base_dir, '..', 'gpt2-finetuned-model')
    max_length = 100
    sequence = data.get('sequence')

    # Call the generate_text function
    answer_text = generate_text(model_path, sequence, max_length)

    # Return the output as JSON
    return jsonify({"answer": answer_text})

if __name__ == '__main__':
    app.run(port=8000)