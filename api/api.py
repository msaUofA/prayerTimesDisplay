from fastapi import FastAPI
import pandas as pd

app = FastAPI()

df = pd.read_csv('Edmonton Prayer Times - 2024 IMS.csv')

months = {
    1: "January",
    2: "February",
    3: "March",
    4: "April",
    5: "May",
    6: "June",
    7: "July",
    8: "August",
    9: "September",
    10: "October",
    11: "November",
    12: "December"
}

@app.get("/prayer-times")
def get_prayer_times(month: int, day: int):
    if month not in months:
        return {"error": "invalid month"}
    
    month_name = months[month]

    filtered_df = df[(df["Unnamed: 0"] == day) & (df[month_name] == month)]

    if not filtered_df.empty:
        return filtered_df.to_dict(orient="records")
    else:
        return {"error": "error"}
