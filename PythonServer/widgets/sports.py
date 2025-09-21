import requests

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

sports_log = []
idx = -1

def fetch_info():
    global sports_log

    sports_log = []
    any_active = False

    for (sport_name, api) in SPORT_APIS:
        response = requests.get(api)
        data = response.json()

        for event in data["events"]:
            status = event["status"]["type"]
            id = status["id"]
            teams = event["competitions"][0]["competitors"]
            team1 = teams[0]
            team2 = teams[1]
            short_detail = status["shortDetail"]

            # 1 = scheduled
            # 2 = in progress
            # 3 = final
            if id == "2":
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
        sports_log = [game for game in sports_log if game["status"] == "2"]

def fetch_game():
    global sports_log, idx

    if idx == -1 or idx == len(sports_log)-1:
        print("Fetching new scores")
        fetch_info()
        idx = 0
    else:
        idx = (idx + 1) % len(sports_log)

    return sports_log[idx]

def fetch_team_icons(is_first_team):
    global sports_log, idx

    icon = sports_log[idx]["team1_icon" if is_first_team else "team2_icon"]
    return process.parse_url(
        icon, (24, 24)
    )
    