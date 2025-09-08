from PIL import Image
import os
import struct

class Media:
    def __init__(self, frames, sleep):
        self.frames = frames
        self.sleep = sleep

PANEL_LENGTH = 64

def rgb_to_rgb565(r, g, b):
    # 5 bits for red (0-31)
    r5 = r >> 3 
    # 6 bits for green (0-63)
    g6 = g >> 2 
    # 5 bits for blue (0-31)
    b5 = b >> 3 
    
    # Combine into a 16-bit value
    rgb565 = (r5 << 11) | (g6 << 5) | b5
    return struct.pack("<H", rgb565)

def resize_image(img):
    width, height = img.size

    ratio = width / height
    pan = 0
    new_size = (PANEL_LENGTH, PANEL_LENGTH)
    if width > height:
        new_size = (int(ratio * PANEL_LENGTH), PANEL_LENGTH)
        pan = 1
    elif width < height:
        new_size = (PANEL_LENGTH, int(PANEL_LENGTH / ratio))
        pan = 2

    return img.resize(new_size), pan

def convert_frame(img, offset=(0, 0)):
    rgb_values = bytearray()

    for y in range(offset[1], offset[1] + PANEL_LENGTH):
        for x in range(offset[0], offset[0] + PANEL_LENGTH):
            r, g, b = img.getpixel((x, y))
            rgb_values.extend(rgb_to_rgb565(r, g, b))

    return bytes(rgb_values)

def parse_media():
    filenames = os.listdir("./gallery")
    gallery = []

    for filename in filenames:
        filename = "./gallery/" + filename

        # Determine if static, panning image, or video
        # TODO: still haven't done video yet
        img = Image.open(filename).convert("RGB")
        img, pan = resize_image(img)
        width, height = img.size

        # pan = 0 (static)
        # pan = 1 (horizontal pan)
        # pan = 2 (vertical pan)
        if pan == 0:
            # Stays on static image for 5 seconds
            gallery.append(Media([convert_frame(img)], 5000))
            continue

        frames = []
        if pan == 1:
            for offset in range(0, width - PANEL_LENGTH + 1, 2):
                frames.append(convert_frame(img, offset=(offset, 0)))
        else:
            for offset in range(0, height - PANEL_LENGTH + 1, 2):
                frames.append(convert_frame(img, offset=(0, offset)))
        
        # Pans left/right or top/bottom in 5 seconds
        gallery.append(Media(frames, int(5000 / len(frames))))

    return gallery


            