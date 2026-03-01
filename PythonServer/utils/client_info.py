from flask import request
import pytz
from datetime import datetime

location = {
    "lat": "37.7749",
    "lon": "122.4194",
    "city": "Location not set",
    "timezone": "America/Los_Angeles"
}

def get_location():
    return location

def update_location():
    data = request.json
    location["lat"], location["lon"] = data["loc"].split(",")
    location["city"] = data["city"]
    location["timezone"] = data["timezone"]
    return "Location updated", 200

def get_time(time_str=False):
    global location

    # Time
    tz = pytz.timezone("America/Los_Angeles")
    now = datetime.now(tz)

    if time_str:
        return now.strftime("%-I:%M%p")
    else:
        return {
            "year": now.year,
            "month": now.month,
            "day": now.day,
            "hour": now.hour,
            "minute": now.minute,
            "second": now.second,
        }