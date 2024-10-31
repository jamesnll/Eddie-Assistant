# Eddie-Book-Assistant

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