from flask import Flask, Response
import asyncio

from widgets import weather

app = Flask(__name__)

@app.route("/")
def home():
    return Response("hello")

@app.route("/weather")
def get_weather():
    return asyncio.run(weather.fetch_info())

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001)