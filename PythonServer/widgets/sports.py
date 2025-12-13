import requests
import threading
import time

import process

SPORT_APIS = [
    (   "Basketball",
        "https://site.api.espn.com/apis/site/v2/sports/basketball/nba/scoreboard"
    ),
    (   "Football",
        "https://site.api.espn.com/apis/site/v2/sports/football/nfl/scoreboard"
    ),
    (   "Baseball",
        "https://site.api.espn.com/apis/site/v2/sports/baseball/mlb/scoreboard"
    ),
    (   "Hockey",
        "https://site.api.espn.com/apis/site/v2/sports/hockey/nhl/scoreboard"
    )
]

# 1 = scheduled
# 2 = in progress
# 3 = final
# 23 = halftime (?)
INACTIVE_IDS = ["1", "3"]

# Don't rewrite sports info while sending request at the same time
sports_lock = threading.RLock()

sports_log = []

def fetch_info():
    global sports_log

    with sports_lock:
        sports_log = []
        any_active = False

        print("Fetching sports info...")
        for (sport_name, api) in SPORT_APIS:
            response = requests.get(api, timeout=5)
            data = response.json()

            for event in data["events"]:
                status = event["status"]["type"]
                id = status["id"]
                teams = event["competitions"][0]["competitors"]
                team1 = teams[0]
                team2 = teams[1]
                short_detail = status["shortDetail"]

                if id not in INACTIVE_IDS:
                    any_active = True

                game_data = {
                    "status": id,
                    "sport_name": sport_name,
                    "team1_name": team1["team"]["abbreviation"],
                    "team1_score": team1["score"],
                    "team1_icon": team1["team"]["logo"],
                    "team2_name": team2["team"]["abbreviation"],
                    "team2_score": team2["score"],
                    "team2_icon": team2["team"]["logo"],
                    "short_detail": short_detail.replace(" - ", "-")
                }
                sports_log.append(game_data)
        
        if any_active:
            sports_log = [game for game in sports_log if game["status"] not in INACTIVE_IDS]
        
        print("Number of games to display: " + str(len(sports_log)))

idx = -2
last_fetch = 0

def fetch_game():
    global sports_log, idx, last_fetch

    with sports_lock:
        last_fetch = time.time() - last_fetch

        # Initial load or refresh every 5 minutes
        if idx == -2 or last_fetch >= 300:
            fetch_info()
            idx = -1
            last_fetch = time.time()

        idx += 1

        if idx == len(sports_log) - 1:
            game = sports_log[idx]
            idx = -1
            return game, True

        if not sports_log:
            print("No sports available!")
            return None, False

        return sports_log[idx], False

def fetch_team_icons():
    global sports_log, idx

    with sports_lock:
        icons = sports_log[idx]
    
    return process.parse_url(
        icons["team1_icon"], (24, 24)
    ) + process.parse_url(
        icons["team2_icon"], (24, 24)
    )