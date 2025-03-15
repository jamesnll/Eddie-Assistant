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
    
    # Generate text based on the input 'ids' with specific sampling parameters
    final_outputs = model.generate(
        ids,                   # Input token IDs to start the generation process
        do_sample=True,        # Enables sampling, allowing for non-deterministic outputs, useful for creative or open-ended text
        max_length=max_length, # Maximum number of tokens in the generated output; helps control the response length
        pad_token_id=model.config.eos_token_id, # Pads the output with the end-of-sequence token to ensure consistent sequence lengths
        top_k=50,              # Top-K sampling: restricts the sampling pool to the 50 most probable next tokens, adding randomness but limiting unlikely outputs
        top_p=0.95,            # Top-p (nucleus) sampling: considers tokens with a cumulative probability of 0.95, balancing diversity and coherence
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

        return answer_part
