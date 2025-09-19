import socket
import time

from process import parse_gallery

HOST = "0.0.0.0"
PORT = 5002

gallery = None

def load_gallery():
    global gallery
    gallery = parse_gallery()
    print("Finished parsing gallery")

def run_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((HOST, PORT))
        server_socket.listen(1)
        print(f"Server listening on {HOST}:{PORT}")

        conn, addr = server_socket.accept()
        with conn:
            print(f"Client connected from {addr}")
            try:
                while True:
                    for media in gallery:
                        for frame in media.frames:
                            t0 = time.time()
                            conn.sendall(frame)
                            dt = (time.time() - t0) * 1000
                            if dt > 5:
                                print(f"sendall blocked {dt:.1f} ms")
                            time.sleep(media.sleep / 1000.0)
            except (BrokenPipeError, ConnectionResetError):
                print("Client disconnected")
