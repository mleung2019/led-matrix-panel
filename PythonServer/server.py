from threading import Thread
import asyncio
from flask import Flask, Response

from widgets import weather, spotify, gallery, sports

app = Flask(__name__)

@app.route("/")
def home():
    return Response("hello")

@app.route("/weather", methods=["POST"])
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

@app.route("/sports")
def get_sports():
    game, trigger_refresh = sports.fetch_game()

    if trigger_refresh:
        Thread(target=sports.fetch_info, daemon=True).start()

    return game

@app.route("/sports/icons")
def get_icons():
    return Response(
        sports.fetch_team_icons(),
        mimetype="application/octet-stream"
    )

if __name__ == "__main__":
    app.run(host="0.0.0.0")