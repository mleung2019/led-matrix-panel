from PIL import Image
import struct
import process

def rgb565_to_rgb888(pixel):
    """Convert 16-bit RGB565 to 24-bit RGB888."""
    r = (pixel >> 11) & 0x1F
    g = (pixel >> 5) & 0x3F
    b = pixel & 0x1F

    # Scale up to 8-bit
    r = (r * 255) // 31
    g = (g * 255) // 63
    b = (b * 255) // 31
    return (r, g, b)

def rgb565_le_to_png(input_file, output_file, width, height):
    with open(input_file, "rb") as f:
        data = f.read()

    expected_size = width * height * 2
    if len(data) != expected_size:
        raise ValueError(f"File size {len(data)} != expected {expected_size} (w*h*2)")

    img = Image.new("RGB", (width, height))
    pixels = img.load()

    for y in range(height):
        for x in range(width):
            idx = (y * width + x) * 2
            # Little endian → first byte is LSB
            pixel_val = data[idx] | (data[idx + 1] << 8)
            pixels[x, y] = rgb565_to_rgb888(pixel_val)

    img.save(output_file, "PNG")
    print(f"Saved {output_file}")


# Example usage
if __name__ == "__main__":
    rgb565_le_to_png("./weather-icons-24x24/0-day.bin", "aef.png", 24, 24)