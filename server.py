from fastapi import FastAPI
import uvicorn
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from datetime import datetime
import os
from dotenv import load_dotenv

import google.auth
from google.oauth2 import service_account
from googleapiclient.discovery import build

load_dotenv()

app = FastAPI()
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=False,
    allow_methods=["*"],
    allow_headers=["*"],
)

class Data(BaseModel):
    counter: int
    timeElapsed: str


# Load Google Sheets credentials
SERVICE_ACCOUNT_FILE = 'C:/Users/asus/Downloads/intern-426210-26177e683dce.json'  # Update this with your service account key file
SCOPES = ['https://www.googleapis.com/auth/spreadsheets']
credentials = service_account.Credentials.from_service_account_file(
    SERVICE_ACCOUNT_FILE, scopes=SCOPES)
spreadsheet_id = '1tYVRh_vxBIccUhM7u_q2guu2NwcNVomTC7Mvs7jjgUw'  # Update this with your Google Sheet ID
sheet_name = 'Sheet1'  # Update this with your sheet name if different

# Initialize Google Sheets API client
service = build('sheets', 'v4', credentials=credentials)
sheet = service.spreadsheets()



@app.post("/get_data")
def get_data(data: Data):
    timestamp = datetime.now().isoformat()
    print(f"Timestamp: {timestamp}, Counter: {data.counter}, timeElapsed:{data.timeElapsed}")

    # Append the data to the Google Spreadsheet
    values = [[timestamp, data.counter, data.timeElapsed]]
    body = {
        'values': values
    }
    result = sheet.values().append(
        spreadsheetId=spreadsheet_id,
        range=f"{sheet_name}!A:C",
        valueInputOption="USER_ENTERED",
        body=body
    ).execute()

    return {"status": 200, "timestamp": timestamp, "counter": data.counter, "timeElapsed": data.timeElapsed}

if __name__ == "__main__":
    uvicorn.run(
        app,
        host="0.0.0.0",  # this host is open at all IP addresses
        port=3000,       # make sure you include this exact same port in ESP32 as well
    )
