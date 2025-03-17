from langchain_community.document_loaders import TextLoader
from langchain_text_splitters import CharacterTextSplitter
from langchain_openai import OpenAIEmbeddings
from langchain_chroma import Chroma

import pandas as pd
import os
from dotenv import load_dotenv


def load_environment_variables():
    """Load environment variables from the .env file."""
    load_dotenv()


def load_shows_data(file_path: str) -> pd.DataFrame:
    """Load shows data from a CSV file."""
    return pd.read_csv(file_path)


def save_tagged_overviews(shows: pd.DataFrame, output_file: str):
    """Save the tagged overviews from the shows dataframe to a .txt file."""
    shows["tagged_overview"].to_csv(output_file, sep="\n", index=False, header=False, encoding="utf-8")


def load_documents(file_path: str) -> list:
    """Load and split documents from a .txt file."""
    raw_documents = TextLoader(file_path, encoding="utf-8").load()
    text_splitter = CharacterTextSplitter(chunk_size=0, chunk_overlap=0, separator="\n")
    return text_splitter.split_documents(raw_documents)


def create_or_load_chroma_db(db_path: str, documents: list = None) -> Chroma:
    """Create or load a Chroma vector database."""
    if os.path.exists(db_path) and os.listdir(db_path):
        print("Loading existing vector database...")
        return Chroma(persist_directory=db_path, embedding_function=OpenAIEmbeddings())
    else:
        print("Creating new vector database...")
        db_shows = Chroma.from_documents(
            documents=documents,
            embedding=OpenAIEmbeddings(),
            persist_directory=db_path
        )
        return db_shows


def retrieve_semantic_recommendations(db_shows: Chroma, query: str, top_k: int = 10, shows: pd.DataFrame = None) -> list:
    """Retrieve the top_k semantic recommendations from the vector database."""
    # Query the vector database
    recommendations = db_shows.similarity_search(query, 50)

    # Blank list to store show ids
    shows_list = []

    for i in range(0, len(recommendations)):
        # Extract the content and split it
        page_content = recommendations[i].page_content.strip('"')

        # Try to extract the first part as the show ID (assuming the first word is always the ID)
        try:
            # Attempt to extract the first word (which is the ID)
            show_id = int(page_content.split()[0])  
            shows_list.append(show_id)
        except ValueError:
            # In case the first part is not a number (e.g., 'The' or title), skip it
            continue

    # Get the top_k shows based on the IDs found
    top_shows = shows[shows["id"].isin(shows_list)].head(top_k)

    # Return the list of show names
    return top_shows["name"].tolist()


def main():
    """Main function to run the process."""
    # Load environment variables
    load_environment_variables()

    # Load the shows data
    shows = load_shows_data("top_k_shows_cleaned.csv")

    # Define the path for the Chroma database
    db_path = "./chroma_db"

    # Check if the Chroma database exists
    if os.path.exists(db_path) and os.listdir(db_path):
        # Load the existing Chroma database
        db_shows = create_or_load_chroma_db(db_path)
    else:
        # Save the tagged overviews
        save_tagged_overviews(shows, "tagged_overview.txt")

        # Load and split the documents
        documents = load_documents("tagged_overview.txt")

        # Create the Chroma database
        db_shows = create_or_load_chroma_db(db_path, documents)

    # Retrieve recommendations
    results = retrieve_semantic_recommendations(db_shows, "A TV show that is similar to Better Call Saul", 10, shows)

    # Print results
    print(results)


if __name__ == "__main__":
    main()
