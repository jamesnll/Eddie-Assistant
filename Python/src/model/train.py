# Imports
from transformers import GPT2Tokenizer, GPT2LMHeadModel


# Load model
def load_model(model_name):
    model = GPT2LMHeadModel.from_pretrained(model_name)
    return model

# Load tokenizer
def load_tokenizer(model_name):
    tokenizer = GPT2Tokenizer.from_pretrained(model_name)
    return tokenizer