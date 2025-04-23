import os
from flask import Flask, request, send_file, render_template_string
import azure.cognitiveservices.speech as speechsdk
 
app = Flask(__name__)
subscription_key = "6BV46EI5SaDVOC9YrTps3mHQ9IDADR1SblZCGlLBgeZrEuoBbCMyJQQJ99BDACPV0roXJ3w3AAAYACOGpqv8"
region = "germanywestcentral"
speech_config = speechsdk.SpeechConfig(subscription=subscription_key, region=region)
 
@app.route("/", methods=["GET", "POST"])
def index():
    if request.method == "POST":
 
        text = request.form["text"]
 
        if text: 
            audio_config = speechsdk.audio.AudioOutputConfig(use_default_speaker=True)
            synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config, audio_config=audio_config)
            result = synthesizer.speak_text_async(text).get()
 
            if result.reason == speechsdk.ResultReason.SynthesizingAudioCompleted:
                return "Text-to-Speech synthesis succeeded."
            else:
                return f"Error: {result.cancellation_details.error_details}"
 
    html_content = """<!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Text to Speech</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
                margin: 0;
                background-color: #f4f4f4;
            }
            .container {
                text-align: center;
                padding: 20px;
                border: 1px solid #ddd;
                border-radius: 10px;
                background-color: white;
            }
            textarea {
                width: 80%;
                max-width: 400px;
                height: 100px;
                padding: 10px;
                margin: 10px 0;
                border: 1px solid #ddd;
                border-radius: 5px;
                font-size: 16px;
            }
            button {
                padding: 10px 20px;
                font-size: 16px;
                background-color: #007BFF;
                color: white;
                border: none;
                border-radius: 5px;
                cursor: pointer;
            }
            button:hover {
                background-color: #0056b3;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>Convert Text to Speech</h1>
            <form action="/download_audio" method="post">
                <textarea name="text" rows="4" placeholder="Enter your text here..."></textarea><br><br>
                <button type="submit">Download Audio</button>
            </form>
        </div>
    </body>
    </html>"""
    return render_template_string(html_content)
 
@app.route("/download_audio", methods=["POST"])
def download_audio():
    text = request.form.get("text")
 
    if text:
 
        audio_config = speechsdk.audio.AudioOutputConfig(filename="output_audio.wav")
        synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config, audio_config=audio_config)
        result = synthesizer.speak_text_async(text).get()
 
        if result.reason == speechsdk.ResultReason.SynthesizingAudioCompleted:
            return send_file("output_audio.wav", as_attachment=True)
        else:
            return f"Error: {result.cancellation_details.error_details}"
    else:
        return "No text provided"
 
if __name__ == "__main__":
    app.run(debug=True)