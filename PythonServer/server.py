import os

from flask import Flask, Response, request, session, redirect, abort
from werkzeug.middleware.proxy_fix import ProxyFix
import asyncio
from threading import Thread

from widgets import weather, spotify, sports

app = Flask(__name__)
app.secret_key = os.environ.get("FLASK_SECRET_KEY")
app.config.update(
    SESSION_COOKIE_SECURE=True,
    SESSION_COOKIE_SAMESITE="None",
    SESSION_COOKIE_HTTPONLY=True,
)
app.wsgi_app = ProxyFix(app.wsgi_app, x_proto=1, x_host=1)

@app.before_request
def check_key():
    key = request.headers.get("X-Device-Key")
    if key != os.environ["X_DEVICE_KEY"]:
        abort(403)

@app.route("/")
def home():
    return Response("hello")

@app.route("/location", methods=["POST"])
def update_location():
    return weather.update_location()

@app.route("/weather")
def get_weather():
    return asyncio.run(weather.fetch_info())

@app.route("/weather/icon")
def get_icon():
    return image_response(weather.fetch_icon())
     
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
    return image_response(spotify.fetch_cover())

@app.route("/sports")
def get_sports():
    game, trigger_refresh = sports.fetch_game()

    if trigger_refresh:
        Thread(target=sports.fetch_info, daemon=True).start()

    return game

@app.route("/sports/icons")
def get_icons():
    return image_response(sports.fetch_team_icons())

# Helper to return image or error
def image_response(data):
    if data is None:
        abort(503)
    else:
        return Response(
            data,
            mimetype="application/octet-stream"
        )

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001)
