import socket
import time
import threading

from process import parse_gallery

gallery = None
server_thread = None
stop_flag = threading.Event()

HOST = "0.0.0.0"
PORT = 5002

def load_gallery():
    global gallery
    gallery = parse_gallery()
    print("Finished parsing gallery")

def run_server():
    global stop_flag
    stop_flag.clear()

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
                            conn.sendall(frame)
                            time.sleep(media.sleep / 1000.0)
                            if stop_flag.is_set():
                                return

            except (BrokenPipeError, ConnectionResetError):
                print("Client disconnected")

def start_server_thread():
    global server_thread
    server_thread = threading.Thread(target=run_server, daemon=True)
    server_thread.start()

def stop_server():
    stop_flag.set()
    with socket.create_connection((HOST, PORT), timeout=1):
        pass
    print("Server stopped")
