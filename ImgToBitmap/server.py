from flask import Flask, Response
import process
import struct
import time

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

# @app.route("/gallery/stream")
# def get_stream():
#     def generate():
#         while True:
#             for media in gallery:
#                 for frame in media.frames:
#                     yield frame
#                     pixels = struct.unpack("<20H", frame[:40])
#                     print("First 20 pixels (hex RGB565):")
#                     print(" ".join(f"{p:04X}" for p in pixels))
#                     time.sleep(media.sleep / 1000.0)

#     return Response(
#         generate(), 
#         mimetype="application/octet-stream",
#         headers={
#             "Content-Type": "application/octet-stream"
#         },
#         direct_passthrough=True
#     )

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
