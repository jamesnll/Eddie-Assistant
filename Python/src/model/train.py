# Imports
from transformers import GPT2Tokenizer, GPT2LMHeadModel, TextDataset, DataCollatorForLanguageModeling, TrainingArguments
import os

model_name = "gpt2"

# Load model
def load_model(model_name):
    model = GPT2LMHeadModel.from_pretrained(model_name)
    return model

# Load tokenizer
def load_tokenizer(model_name):
    tokenizer = GPT2Tokenizer.from_pretrained(model_name)
    return tokenizer

# Get preprocessed dataset
def get_preprocessed_data_path():
    # Get the absolute path to the current directory of main.py
    base_dir = os.path.dirname(os.path.abspath(__file__))
    preprocessed_data_path = os.path.join(base_dir, '..', '..', 'dataset', 'preprocessed_data.txt')
    return preprocessed_data_path

# Prepare dataset
def load_dataset(file_path, tokenizer, block_size = 128):
    return TextDataset(
        tokenizer=tokenizer,
        file_path=file_path,
        block_size=block_size
    )

tokenizer = load_tokenizer(model_name)

# Load dataset
train_dataset = load_dataset(get_preprocessed_data_path(), tokenizer)

# Data collator
data_collator = DataCollatorForLanguageModeling(
    tokenizer=tokenizer, mlm=False
)

# Training arguments
training_args = TrainingArguments(
    output_dir="./gpt2-finetuned-model",
    overwrite_output_dir=True,
    num_train_epochs=50.0,
    per_device_train_batch_size=8,
    save_steps=50000,
    save_total_limit=2,
    prediction_loss_only=True,
)