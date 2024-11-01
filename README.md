# Eddie-Book-Assistant
Eddie is a virtual assistant designed to provide a seamless question-answering experience focused on books. With a setup inspired by Alexa, Eddie combines an ESP32 embedded system for real-time voice interaction and a fine-tuned GPT-2 model curated specifically for book-related questions. Written in Python and powered by the ESP32, Eddie takes voice input from users, queries the model, and responds with spoken output.
## Features
* Fine-Tuned GPT-2 Model: Curated dataset enhances accuracy and relevance for book-focused queries.  
## Project Structure
📂 root  
│  
├── 📂 Python  
│    ├── 📂 dataset                     # Contains Question Answer pairs for various books  
│    │    ├── 📄 QA_And_Then_There_Were_None.txt  
│    │    ├── 📄 QA_Forrest_Gump.txt  
│    │    ├── 📄 QA_Harry_Potter_Philosopher's_Stone.txt  
│    │    ├── 📄 QA_Heir_to_the_Empire.txt  
│    │    ├── 📄 QA_Holes.txt  
│    │    ├── 📄 QA_Invincible.txt  
│    │    ├── 📄 QA_Moneyball.txt  
│    │    ├── 📄 QA_The_Girl_with_the_Dragon_Tattoo.txt  
│    │    ├── 📄 QA_The_Hitchhiker's_Guide_to_the_Galaxy.txt  
│    │    ├── 📄 QA_The_Lion_the_Witch_and_the_Wardrobe.txt  
│    │    ├── 📄 QA_The_Princess_Bride.txt         
│    │    ├── 📄 QA_To_Kill_A_Mockingbird.txt  
│    │    └── 📄 QA_Under_the_Dome.txt  
│    │  
│    ├── 📂 src  
│    │    ├── 📂 model  
│    │    │    ├── 📄 preprocess.py    # Data cleaning and preparation script  
│    │    │    ├── 📄 train.py         # Model training script  
│    │    │    └── 📄 inference.py     # Script for generating inferences  
│    │    │  
│    │    └── 📄 main.py               # Main entry point for preprocessing, training, or inferencing  
│    │  
│    └── 📄 README.md                  # Project documentation  