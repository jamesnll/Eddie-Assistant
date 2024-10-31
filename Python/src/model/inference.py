# Imports
from transformers import GPT2Tokenizer, GPT2LMHeadModel

# Load Model
def load_model(model_path):
    model = GPT2LMHeadModel.from_pretrained(model_path)
    return model

# Load Tokenizer
def load_tokenizer(tokenizer_path):
    tokenizer = GPT2Tokenizer.from_pretrained(tokenizer_path)
    return tokenizer
