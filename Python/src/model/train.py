# Imports
from transformers import GPT2Tokenizer, GPT2LMHeadModel, TextDataset, DataCollatorForLanguageModeling, TrainingArguments, Trainer
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

# Load model and tokenizer
model = load_model(model_name)
tokenizer = load_tokenizer(model_name)

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

# Load dataset
train_dataset = load_dataset(get_preprocessed_data_path(), tokenizer)

# Data collator
data_collator = DataCollatorForLanguageModeling(
    tokenizer=tokenizer, mlm=False
)

# Train the model
def train_model():

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

    # Trainer
    trainer = Trainer(
        model=model,
        args=training_args,
        data_collator=data_collator,
        train_dataset=train_dataset,
    )

    # Fine-tune the model
    trainer.train()

    # Save the fine-tuned model
    model.save_pretrained("./gpt2-finetuned-model")
    tokenizer.save_pretrained("./gpt2-finetuned-model")
