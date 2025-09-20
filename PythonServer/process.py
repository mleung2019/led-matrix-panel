from PIL import Image
import requests
from io import BytesIO
import cv2
import math
import struct
import mimetypes
import os
import pickle

PANEL_LENGTH = 64

FPS_CONST = 20
DELAY_CONST = 1000 / FPS_CONST

class Media:
    def __init__(self, frames, sleep):
        self.frames = frames
        self.sleep = sleep


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

def crop_center(img, target_width, target_height):
    width, height = img.size
    left = (width - target_width) // 2
    top = (height - target_height) // 2
    right = left + target_width
    bottom = top + target_height
    return img.crop((left, top, right, bottom))

def pad_to_size(img, target_size):
    target_w, target_h = target_size
    img = img.convert("RGBA")  # Ensure transparency is kept
    canvas = Image.new("RGBA", (target_w, target_h), (0, 0, 0, 0))
    offset_x = (target_w - img.width) // 2
    offset_y = (target_h - img.height) // 2
    canvas.paste(img, (offset_x, offset_y), img)
    return canvas

# Used by PIL for images
def convert_frame(img, offset=(0, 0)):
    rgb_values = bytearray()
    for y in range(offset[1], offset[1] + PANEL_LENGTH):
        for x in range(offset[0], offset[0] + PANEL_LENGTH):
            r, g, b = img.getpixel((x, y))
            rgb_values.extend(rgb_to_rgb565(r, g, b))
    return bytes(rgb_values)

# Used by OpenCV for videos
def convert_frame_cv2(bgr):
    rgb_values = bytearray()
    for x in range(PANEL_LENGTH):
        for y in range(PANEL_LENGTH):
            low, high = bgr[x, y]
            pixel = (int(high) << 8) | int(low) 
            rgb_values.extend(struct.pack("<H", pixel))
    return bytes(rgb_values)

# URL to PIL img to bytes object
def parse_url(url, size=(PANEL_LENGTH, PANEL_LENGTH), preserve_ratio=False):
    if url == None:
        return None
    
    response = requests.get(url)
    if response.status_code == 200:
        img_data = BytesIO(response.content)
        img = Image.open(img_data).convert("RGBA")

        if preserve_ratio:
            longest_len = max(img.size)
            print(longest_len)
            img = pad_to_size(img, (longest_len, longest_len))

        background = Image.new("RGBA", img.size, (0, 0, 0, 255))  # black bg
        composite = Image.alpha_composite(background, img).resize(size)

        # Convert back to RGB (drops alpha but keeps background filled)
        pil_image = composite.convert("RGB")

        return convert_frame(pil_image)
    else:
        return None

def parse_gallery():
    filenames = os.listdir("./gallery")

    # If cache file exists, use that instead
    if ".cache.pkl" in filenames:
        print("Using gallery from cache file")
        with open("./gallery/.cache.pkl", "rb") as file:
            return pickle.load(file)

    gallery = []

    for filename in filenames:
        mime_type = mimetypes.guess_type(filename)[0]
        
        primary, sub = mime_type.split("/")
        
        filename = "./gallery/" + filename

        # Determine if static, panning image, or video
        if primary == "image" and sub != "gif":
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
                for offset in range(0, width - PANEL_LENGTH + 1, 1):
                    frames.append(convert_frame(img, offset=(offset, 0)))
            else:
                for offset in range(0, height - PANEL_LENGTH + 1, 1):
                    frames.append(convert_frame(img, offset=(0, offset)))
            
            # Pans left/right or top/bottom in 5 seconds
            gallery.append(Media(frames, int(5000 / len(frames))))

        else:
            if primary == "video":
                video = cv2.VideoCapture(filename)
                fps = video.get(cv2.CAP_PROP_FPS)
                frames = []

                while True:
                    ret, frame = video.read()
                    if not ret:
                        break

                    frame = cv2.resize(
                        frame, 
                        (PANEL_LENGTH, PANEL_LENGTH), 
                        interpolation=cv2.INTER_AREA
                    )
                    bgr = cv2.cvtColor(frame, cv2.COLOR_BGR2BGR565)
                    
                    frames.append(convert_frame_cv2(bgr))

                gallery.append(Media(frames, int(1000 / fps)))
            
            if sub == "gif":
                frames = []
                with Image.open(filename) as img:
                    try:
                        while True:
                            frame = img.convert("RGB").resize(
                               (PANEL_LENGTH, PANEL_LENGTH)
                            )
                            frames.append(convert_frame(frame))
                            img.seek(img.tell() + 1)
                    except EOFError:
                        pass

                gallery.append(Media(frames, 100))

    # Throttle gallery such that everything runs at 20 FPS or lower
    for media in gallery:
        if media.sleep < DELAY_CONST:
            original_frame_count = len(media.frames)
            print("Original frames:", original_frame_count)

            # Calculate how many frames to remove
            target_frame_count = math.ceil(original_frame_count * media.sleep / DELAY_CONST)
            frames_to_keep = target_frame_count

            if frames_to_keep < original_frame_count:
                step = original_frame_count / frames_to_keep
                new_frames = [
                    media.frames[math.floor(i * step)] for i in range(frames_to_keep)
                ]
                media.frames = new_frames
                media.sleep = DELAY_CONST
            else:
                media.sleep = DELAY_CONST

            print("Throttled frames:", len(media.frames))


    # Cache the gallery object for future use
    with open("./gallery/.cache.pkl", "wb") as file:
        print("Cached gallery")
        pickle.dump(gallery, file)

    return gallery
