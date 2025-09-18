import threading
import asyncio
from flask import Flask, Response

from widgets import weather, spotify, gallery

# Load gallery before server starts
gallery.load_gallery()
is_gallery_running = False

app = Flask(__name__)

@app.route("/")
def home():
    return Response("hello")

@app.route("/weather")
def get_weather():
    return asyncio.run(weather.fetch_info())

@app.route("/weather/icon")
def get_icon():
    return Response(
        weather.fetch_icon(),
        mimetype="application/octet-stream"
    )

@app.route("/spotify")
def get_track():
    return spotify.fetch_info()

@app.route("/spotify/cover")
def get_cover():
    return Response(
        spotify.fetch_cover(),
        mimetype="application/octet-stream"
    )

@app.route("/gallery")
def toggle_server():
    global is_gallery_running
    if not is_gallery_running:
        gallery.start_server_thread()
        is_gallery_running = True
        return "Gallery server started"
    else:
        gallery.stop_server()
        is_gallery_running = False
        return "Gallery server stopped"

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001)