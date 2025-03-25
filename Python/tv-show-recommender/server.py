# Imports
from flask import Flask, request, jsonify
from urllib.parse import unquote
import os

from vector_search import get_recommendations

app = Flask(__name__) # Create Flask app

@app.route('/')
def home():
    return "Hello from the Flask server!"

# Route to retrieve recommendations
@app.route('/recommend-shows', methods=['GET'])
def recommend_shows():

    # Retrieve query from request
    query = request.args.get("query")

    # Check if query is None or empty
    if not query:
        return jsonify({"error": "The 'query' field is required and cannot be empty."}), 400
    
    # Decode the URL-encoded query
    decoded_query = unquote(query)

    print("Query: " + decoded_query)

    # Query the recommendation service
    results = get_recommendations(decoded_query)

    print("Results: " + str(results))

    # Return the output as JSON
    return jsonify({"recommendations": results}), 200

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=8000)