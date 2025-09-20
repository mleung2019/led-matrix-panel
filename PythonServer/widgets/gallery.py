import socket
import time
import struct

from process import parse_gallery

HOST = "0.0.0.0"
PORT = 5002

gallery = None

def load_gallery():
    global gallery
    gallery = parse_gallery()

def run_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((HOST, PORT))
        server_socket.listen(1)
        print(f"Server listening on {HOST}:{PORT}")

        while True:
            conn, addr = server_socket.accept()
            with conn:
                print(f"Client connected from {addr}")
                try:
                    while True:
                        for media in gallery:
                            for frame in media.frames:
                                delay_header = struct.pack(">H", int(media.sleep))
                                conn.sendall(delay_header + frame)
                                time.sleep(0.01)
                except (BrokenPipeError, ConnectionResetError):
                    print("Client disconnected")
