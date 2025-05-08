from gtts import gTTS

def text_to_speech(input: str, output_path: str):
    # TODOs

    # Define TTS language and accent
    language = "en"
    accent = "co.uk"

    # Convert the text into speech
    tts = gTTS(text=input, lang=language, slow=False, tld=accent)
    # Save the speech to disk and save the path to output_path
    # Return the output_path
    return output_path
    # Any error handling as required