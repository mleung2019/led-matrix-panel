from flask import Flask, request, Response
import asyncio

from widgets import weather, spotify

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

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001)