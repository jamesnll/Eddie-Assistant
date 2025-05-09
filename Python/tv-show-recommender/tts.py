from gtts import gTTS

def text_to_speech(input: list):
    # TODOs
    # Any error handling as required
    output_path = "test.mp3"

    # Define TTS language and accent
    language = "en"
    accent = "co.uk"

    # Convert input list to one string, add final period to the end.
    input_str = '. '.join(input) + '.'

    # Convert the text into speech
    tts = gTTS(text=input_str, lang=language, slow=False, tld=accent)

    # Save the speech to disk and save the path to output_path
    tts.save(output_path)
    # Return the output_path
    return output_path


if __name__ == '__main__':
    input = ["My", "name", "is", "Edie"]
    text_to_speech(input)