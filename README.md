# Eddie-Assistant
Eddie is a **semantic TV show recommender system** designed to deliver personalized show suggestions based on user preferences. Using **vector search** and **OpenAI embeddings**, Eddie leverages semantic understanding to provide more accurate and contextually relevant recommendations. The Python portion of the project is hosted on a local Flask server, which handles query processing, interacts with OpenAI embeddings for semantic search, and returns recommendations. The system is interfaced with an **ESP32 client**, where **FreeRTOS** tasks allow the user can speak into a microphone, query the server for recommendations, and receive spoken responses through **TTS** and a speaker.
## Features
- **Kaggle Datasets:** Uses a curated set of TV show-related datasets from Kaggle to improve recommendation accuracy and diversity.
- **Vector Search with OpenAI Embeddings:** Utilizes semantic understanding to search and recommend TV shows based on user queries.
- **Flask Web Server:** Hosts the recommendation engine, processes incoming queries from the ESP32 client, and returns show recommendations.
- **ESP32 Client Integration with FreeRTOS:** Captures user input via a microphone in C, sends queries to the Flask server, handles tasks such as voice input, server queries, TTS output, and Wi-Fi connectivity using FreeRTOS.
- **Real-Time Voice Interaction:** Enables users to interact with the system through speech, creating a hands-free, conversational experience.
## Project Structure
📂 root
- 📂 Python  
  -   📂 archive  → Contains unimplemented code  
      -  question_answering → Model used for question answering tasks  
          - 📂 dataset → Contains Question Answer pairs for various books  
            -  📄 QA_And_Then_There_Were_None.txt  
            -  📄 QA_Forrest_Gump.txt  
            -  📄 QA_Harry_Potter_Philosopher's_Stone.txt  
            -  📄 QA_Heir_to_the_Empire.txt  
            -  📄 QA_Holes.txt  
            -  📄 QA_Invincible.txt  
            -  📄 QA_Moneyball.txt  
            -  📄 QA_The_Girl_with_the_Dragon_Tattoo.txt  
            -  📄 QA_The_Hitchhiker's_Guide_to_the_Galaxy.txt  
            -  📄 QA_The_Lion_the_Witch_and_the_Wardrobe.txt 
            -  📄 QA_The_Princess_Bride.txt  
            -  📄 QA_To_Kill_A_Mockingbird.txt  
            -  📄 QA_Under_the_Dome.txt  
          - 📂 src  
            -  📂 model   
                 - 📄 preprocess.py → Data cleaning and preparation script  
                 - 📄 train.py&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;     → Model training script    
                 - 📄 inference.py&nbsp;&nbsp; → Script for generating inferences  
            - 📂 server  
              - 📄 server.py → Local server to host and query model  
            - 📄 main.py → Main entry point for preprocessing, training, or inferencing  
          - 📄 requirements.txt → Python packages required  
      - 📄 README.md → Archive documentation  

📄 README.md → Project documentation  

## Installation

### Python Setup

### ESP32 Setup

## Usage

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments
- The Kaggle datasets provide valuable data for improving the recommendation system.
- OpenAI for their embeddings that enable semantic search and improve recommendation accuracy.
- ESP32 and FreeRTOS for enabling the real-time, voice-driven interaction in the embedded system.