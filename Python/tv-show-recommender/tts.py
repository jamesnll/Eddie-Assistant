from gtts import gTTS

def text_to_speech(input: str):
    # TODOs
    # Any error handling as required

    # Define TTS language and accent
    language = "en"
    accent = "co.uk"

    # Convert the text into speech
    tts = gTTS(text=input, lang=language, slow=False, tld=accent)

    # Save the speech to disk and save the path to output_path
    tts.save(output_path)
    # Return the output_path
    return output_path


if __name__ == '__main__':
    input = "My name is Eddie"
    output_path = "test.mp3"
    text_to_speech(input, output_path)