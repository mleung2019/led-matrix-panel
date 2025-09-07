from flask import Flask, Response
import os
import convert


app = Flask(__name__)

images = os.listdir("./gallery")
index = 0

@app.route("/")
def home():
    return Response("hello")

@app.route("/gallery")
def get_image():
    global index
    response = Response(convert.convert_image(str("./gallery/" + images[index])))
    index = (index + 1) % len(images) 
    return response

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
