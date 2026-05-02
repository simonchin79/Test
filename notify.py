import requests
from datetime import datetime
import sys

# Paste your details here
TOKEN = "8734082474:AAGn2DD0Wv_Uv8oPb2W5ZkX73Tz1ZIEtZtc"
CHAT_ID = "8581226007"

def send_telegram(message):
    url = f"https://api.telegram.org/bot{TOKEN}/sendMessage"
    payload = {
        "chat_id": CHAT_ID,
        "text": message,
        "parse_mode": "Markdown"
    }
    
    # Get current time for the terminal output
    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    try:
        print(f"[{now}] 📡 Sending message to Telegram...")
        response = requests.post(url, data=payload)
        res_json = response.json()
        
        if res_json.get("ok"):
            print(f"[{now}] ✅ Success! Message delivered.")
        else:
            print(f"[{now}] ❌ Telegram Error: {res_json.get('description')}")
            
        return res_json
    except Exception as e:
        print(f"[{now}] ❌ Connection Error: {e}")

if __name__ == "__main__":
    # This allows you to pass a custom message from the terminal
    # Example: python3 notify.py "Build complete"
    custom_msg = sys.argv[1] if len(sys.argv) > 1 else "Task completed successfully!"
    
    full_message = f"🚀 **CodeAgent Alert:** {custom_msg}"
    send_telegram(full_message)