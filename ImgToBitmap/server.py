from flask import Flask, Response, stream_with_context
import process
import struct
import time

app = Flask(__name__)

gallery = process.parse_media()
index = 0

def generate_stream():
    while True:
        for media in gallery:
            for frame in media.frames:
                # Prefix frame with 4-byte little-endian length
                yield struct.pack("<I", len(frame)) + frame
                time.sleep(media.sleep)

@app.route("/")
def home():
    return Response("hello")

@app.route("/gallery")
def get_info():
    global index
    media = gallery[index]
    # Number of frames, sleep duration per frame (ms)
    response = Response(str(index) + "," + str(len(media.frames)) + "," + str(media.sleep))
    index = (index + 1) % len(gallery) 
    return response

@app.route("/gallery/<int:mediaIndex>/<int:frame>")
def get_media(mediaIndex, frame):
    return Response(
        gallery[mediaIndex].frames[frame],
        mimetype="application/octet-stream"
    )

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
