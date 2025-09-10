import socket
import time
import process

UDP_IP = "192.168.0.26"
UDP_PORT = 5005

gallery = process.parse_media()
print("Gallery finished processing")

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
frame_id = 0

def send_frame(frame_bytes, frame_id):
    # Recommended packet size
    packet_size = 1400
    total_packets = (len(frame_bytes) + packet_size - 1) // packet_size

    for i in range(total_packets):
        start = i * packet_size
        end = min(start + packet_size, len(frame_bytes))
        payload = frame_bytes[start:end]

        # Header: frame_id (2 bytes) 
        # ----- packet index (1 byte) 
        # ----- number of total packets (1 byte)
        header = frame_id.to_bytes(2, "big") + i.to_bytes(1, "big") + total_packets.to_bytes(1, "big")
        sock.sendto(header + payload, (UDP_IP, UDP_PORT))

while True:
    for media in gallery:
        for frame in media.frames:
            send_frame(frame, frame_id)
            frame_id = (frame_id + 1) % 65536
            time.sleep(media.sleep / 1000.0)