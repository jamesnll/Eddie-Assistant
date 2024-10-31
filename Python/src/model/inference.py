# Imports
from transformers import GPT2Tokenizer, GPT2LMHeadModel
import os

# Load Model
def load_model(model_path):
    model = GPT2LMHeadModel.from_pretrained(model_path)
    return model

# Load Tokenizer
def load_tokenizer(tokenizer_path):
    tokenizer = GPT2Tokenizer.from_pretrained(tokenizer_path)
    return tokenizer

# Generate text
def generate_text(model_path, sequence, max_length):
    model = load_model(model_path)
    tokenizer = load_tokenizer(model_path)
    ids = tokenizer.encode(f'{sequence}', return_tensors='pt')
    
    final_outputs = model.generate(
        ids,
        do_sample=True,
        max_length=max_length,
        pad_token_id=model.config.eos_token_id,
        top_k=50,
        top_p=0.95,
    )
    
    # Decode the generated text
    output_text = tokenizer.decode(final_outputs[0], skip_special_tokens=True)
    
    # Extract the first question and answer
    if "Q:" in output_text:
        # Split the output text by "Question:" to isolate the first question
        question_answer = output_text.split("Q:")[1].strip()
        
        # Now split this part by "Answer:" to isolate the answer
        if "A:" in question_answer:
            question_part = question_answer.split("A:")[0].strip()
            answer_part = question_answer.split("A:")[1].strip()
        else:
            question_part = question_answer.strip()  # No answer provided
            answer_part = ""  # Default to an empty answer if no answer is found

        # Print the formatted output
        print("Q: " + question_part)
        print("A: " + answer_part)
