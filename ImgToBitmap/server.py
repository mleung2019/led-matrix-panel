from flask import Flask, Response
import process

app = Flask(__name__)

gallery = process.parse_media()
index = 0

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

@app.route("/gallery/<int:mediaIndex>")
def get_media(mediaIndex):
    # Frames go into a byte stream
    def stream():
        output = bytearray()
        media = gallery[mediaIndex]
        for frame in media.frames:
            output.extend(frame)
        return bytes(output)

    data = stream()
    return Response(
        data,
        mimetype="application/octet-stream",
        headers={"Content-Length": str(len(data))}
    )

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
