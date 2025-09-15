import process
import json

with open("./widgets/wmo-codes.json", "r") as file:
    data = json.load(file)
    for code in data:
        with open("./weather-icons-16x16/" + code + "-day.bin", "wb") as f:
            f.write(process.parse_url(data[code]["day"]["image"], (16, 16)))
        
        with open("./weather-icons-16x16/" + code + "-night.bin", "wb") as f:
            f.write(process.parse_url(data[code]["night"]["image"], (16, 16)))