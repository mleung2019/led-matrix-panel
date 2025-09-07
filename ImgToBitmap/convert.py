from PIL import Image

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
    return f"0x{rgb565:04x}"

def resize_image(img, width, height):
    new_size = (1, 1)
    ratio = width / height
    if (width < height):
        new_size = (PANEL_LENGTH, int(PANEL_LENGTH / ratio))
    else:
        new_size = (int(ratio * PANEL_LENGTH), PANEL_LENGTH)
        
    return img.resize(new_size)

def convert_image(filename):
    img = Image.open(filename)
    img = img.convert("RGB")

    width, height = img.size
    if (width != PANEL_LENGTH and height != PANEL_LENGTH):
        img = resize_image(img, width, height)
        width = height = PANEL_LENGTH

    rgb_values = []

    for y in range(height):
        for x in range(width):
            r, g, b = img.getpixel((x, y))
            rgb_values.append(rgb_to_rgb565(r, g, b))

    return ",".join(rgb_values)
