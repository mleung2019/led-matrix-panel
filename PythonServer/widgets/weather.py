import openmeteo_requests

import threading
import json
import time
from datetime import datetime, timedelta

import utils.client_info as client_info

weather_lock = threading.RLock()
weather_info = None
current_icon = None

def hours_ahead(start_time_str, hours=5):
    start = datetime.strptime(start_time_str, "%I:%M%p")
    results = []
    for i in range(1, hours + 1):
        new_time = start + timedelta(hours=i)
        results.append(new_time.strftime("%I%p").lstrip("0"))
    return results

def fetch_info():
    location = client_info.get_location()
    time_str = client_info.get_time(time_str=True)

    # Weather
    openmeteo = openmeteo_requests.Client()
    url = "https://api.open-meteo.com/v1/forecast"
    params = {
        "latitude": location["lat"],
        "longitude": location["lon"],
        "timezone": location["timezone"],
        "daily": ["temperature_2m_max", "temperature_2m_min"],
        "hourly": ["temperature_2m", "weather_code", "is_day"],
    	"current": ["is_day", "weather_code", "temperature_2m"],
        "forecast_days": 1,
        "forecast_hours": 6,
        "wind_speed_unit": "mph",
        "temperature_unit": "fahrenheit",
        "precipitation_unit": "inch",
    }
    responses = openmeteo.weather_api(url, params=params)

    # Process first location. Add a for-loop for multiple locations or weather models
    response = responses[0]

    # Process current data. The order of variables needs to be the same as requested.
    current = response.Current()
    current_is_day = current.Variables(0).Value()
    current_weather_code = current.Variables(1).Value()
    current_temperature_2m = current.Variables(2).Value()

    # Process hourly data. The order of variables needs to be the same as requested.
    hourly = response.Hourly()
    hourly_temperature_2m = hourly.Variables(0).ValuesAsNumpy()
    hourly_weather_code = hourly.Variables(1).ValuesAsNumpy()
    hourly_is_day = hourly.Variables(2).ValuesAsNumpy()

    # Process daily data. The order of variables needs to be the same as requested.
    daily = response.Daily()
    daily_temperature_2m_max = daily.Variables(0).ValuesAsNumpy()
    daily_temperature_2m_min = daily.Variables(1).ValuesAsNumpy()

    # JSON formatting
    time_strs = hours_ahead(time_str, hours=5)
    temps = [round(x) for x in hourly_temperature_2m][1:]
    statuses = [
        get_status(code, is_day) 
        for code, is_day in zip(hourly_weather_code[1:], hourly_is_day[1:])
    ]
    five_hr_log = [
        {"time_str": t, "temp": temp, "status": status} 
        for t, temp, status in zip(time_strs, temps, statuses)
    ]

    icon_tuple = (current_weather_code, current_is_day)

    data = [
        {
            "city": location["city"][:120],
            "curr_temp": round(current_temperature_2m),
            "high_temp": round(daily_temperature_2m_max[0]),
            "low_temp": round(daily_temperature_2m_min[0]),
            "status": get_status(current_weather_code, current_is_day),
            "five_hr_log": five_hr_log,
        },
        icon_tuple
    ]
    return data
    
# Maps weather code and day/night info to a status description
def get_status(weather_code, is_day):
    wmo_code = int(weather_code)
    with open("./weather-icons-24x24/wmo-codes.json", "r") as file:
        data = json.load(file)
        status = data[str(wmo_code)]["day" if is_day else "night"]
        return status["description"]

def update_weather():
    global weather_info

    while True:
        with weather_lock:
            weather_info = fetch_info()
        time.sleep(60)

def fetch_weather():
    global current_icon

    with weather_lock:
        needs_icon = False
        if weather_info[1] != current_icon:
            current_icon = weather_info[1]
            needs_icon = True

        extra_weather_info = {
            "time_str": client_info.get_time(time_str=True)[:-2],
            "needs_icon": needs_icon
        }

        return weather_info[0] | extra_weather_info

def fetch_icon():
    wmo_code, is_day = current_icon
    filename = str(int(wmo_code)) + "-" + ("day" if is_day else "night") + ".bin"
    with open("./weather-icons-24x24/" + filename, "rb") as file:
        return file.read()