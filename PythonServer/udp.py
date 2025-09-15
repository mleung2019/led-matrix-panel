import socket
import time
import struct

import process

UDP_IP = "192.168.0.26"
UDP_PORT = 5005

PAYLOAD_SIZE = 1200

gallery = process.parse_gallery()
print("Gallery finished processing")

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Lower-latency tricks
try:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 65536)
except Exception:
    pass
try:
    IPTOS_LOWDELAY = 0x10
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_TOS, IPTOS_LOWDELAY)
except Exception:
    pass

frame_id = 0

def send_frame(frame_bytes, frame_id):
    packet_size = PAYLOAD_SIZE
    total_packets = (len(frame_bytes) + packet_size - 1) // packet_size

    for i in range(total_packets):
        start = i * packet_size
        end = min(start + packet_size, len(frame_bytes))
        payload = frame_bytes[start:end]

        # Big endian
        header = struct.pack(">HBB", frame_id & 0xFFFF, total_packets & 0xFF, i & 0xFF)
        sock.sendto(header + payload, (UDP_IP, UDP_PORT))

while True:
    for media in gallery:
        for frame in media.frames:
            send_frame(frame, frame_id)
            frame_id = (frame_id + 1) % 65536
            time.sleep(media.sleep / 1000.0)