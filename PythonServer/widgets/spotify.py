from dotenv import load_dotenv
import os
import json

import spotipy
from spotipy.oauth2 import SpotifyOAuth

import process

# Load environment variables from .env file
load_dotenv()

CLIENT_ID = os.getenv("SPOTIFY_CLIENT_ID")
CLIENT_SECRET = os.getenv("SPOTIFY_CLIENT_SECRET")
REDIRECT_URI = os.getenv("SPOTIFY_REDIRECT_URI")
SCOPE = "user-read-currently-playing"
TOKEN_FILE = "./widgets/spotify_token.json"

DUMMY_COVER_URL = "https://www.pikpng.com/pngl/b/569-5691531_circular-question-mark-button-number-3-png-white.png"

def load_token():
    try:
        with open(TOKEN_FILE, "r") as f:
            return json.load(f)
    except:
        return None

def save_token(token_info):
    with open(TOKEN_FILE, "w") as f:
        json.dump(token_info, f)

def get_auth_manager():
    return SpotifyOAuth(
        client_id=CLIENT_ID,
        client_secret=CLIENT_SECRET,
        redirect_uri=REDIRECT_URI,
        scope=SCOPE,
        cache_path=None,
        open_browser=False,
    )

def get_spotify_client():
    token_info = load_token()
    
    if not token_info:
        return None

    auth_manager = get_auth_manager()

    if auth_manager.is_token_expired(token_info):
        token_info = auth_manager.refresh_access_token(token_info["refresh_token"])
        save_token(token_info)

    return spotipy.Spotify(auth=token_info["access_token"])

# We don't want to process the Spotify cover for a song over and over again.
# Detect when the cover changes by keeping track of the old cover url.
current_cover = None

def fetch_info():
    global current_cover

    # Authenticate if necessary
    sp = get_spotify_client()
    if not sp:
        return None

    # Get currently playing track
    try:
        current = sp.current_user_playing_track()
    except:
        return None
    
    if current == None or current["item"] == None: 
        current_cover = DUMMY_COVER_URL
        return {"is_active": False}

    track_name = current["item"]["name"]
    artist_name = ", ".join(artist["name"] for artist in current["item"]["artists"])
    
    images = current["item"]["album"].get("images", [])
    cover_url = images[0]["url"] if images else DUMMY_COVER_URL
    
    progress_ms = current["progress_ms"]
    duration_ms = current["item"]["duration_ms"]

    needs_cover = False

    if cover_url != current_cover:
        current_cover = cover_url
        needs_cover = True

    # Send relevant info
    data = {
        "is_active": True,
        "track_name": track_name[:80],
        "artist_name": artist_name[:40],
        "needs_cover": needs_cover,
        "progress_ms": progress_ms,
        "duration_ms": duration_ms
    }

    return data

def fetch_cover():
    return process.parse_url(current_cover)