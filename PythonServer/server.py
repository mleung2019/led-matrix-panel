import os

from flask import Flask, Response, request, session, redirect
from werkzeug.middleware.proxy_fix import ProxyFix
import asyncio
from threading import Thread

from widgets import weather, spotify, gallery, sports

app = Flask(__name__)
app.secret_key = os.environ.get("FLASK_SECRET_KEY")
app.config.update(
    SESSION_COOKIE_SECURE=True,
    SESSION_COOKIE_SAMESITE="None",
    SESSION_COOKIE_HTTPONLY=True,
)
app.wsgi_app = ProxyFix(app.wsgi_app, x_proto=1, x_host=1)

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

@app.route("/spotify/login")
def spotify_login():
    auth_manager = spotify.get_auth_manager()
    auth_url = auth_manager.get_authorize_url()
    return redirect(auth_url)

@app.route("/spotify/callback")
def spotify_callback():
    auth_manager = spotify.get_auth_manager()
    code = request.args.get("code")
    if not code:
        return "Missing code", 400
    
    token_info = auth_manager.get_access_token(code)
    spotify.save_token(token_info)

    return redirect("/spotify")

@app.route("/spotify")
def get_track():
    data = spotify.fetch_info()

    if data is None:
        return redirect("/spotify/login")
    
    return data

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
