import pandas as pd
from flask import Flask, jsonify, request

file_path = 'Edmonton Prayer Times - 2024 IMS.csv'
data = pd.read_csv(file_path)

months = ['', 'January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December']

month_starts = data[data.iloc[:, 1].isin(months)].index

months_data = {}

for i, start in enumerate(month_starts):
    month_name = data.iloc[start, 1]
    end = month_starts[i + 1] if i + 1 < len(month_starts) else len(data)
    month_df = data.iloc[start + 1:end].copy()
    month_df.columns = ['Day', 'Date', 'Fajr', 'Sunrise', 'Dhuhr', 'Asr', 'Maghrib', 'Isha', 'Extra']
    month_df = month_df.drop(columns=['Extra']).dropna(subset=['Date'])
    month_df['Date'] = pd.to_numeric(month_df['Date'], errors='coerce').dropna().astype(int)
    months_data[month_name] = month_df

app = Flask(__name__)

@app.route('/prayer-times', methods=['GET'])
def get_prayer_times():
    month_i = request.args.get('month', type=int)
    day = request.args.get('day', type=int)

    if month_i not in range(1,12+1):
        return jsonify({'error': 'invalid month'}), 400
    month = months[month_i]

    month_data = months_data.get(month)
    prayer_times = month_data[month_data['Date'] == day]

    if prayer_times.empty:
        return jsonify({'error': 'No data available for the provided day'}), 404

    result = prayer_times[['Day', 'Fajr', 'Sunrise', 'Dhuhr', 'Asr', 'Maghrib', 'Isha']].to_dict(orient='records')[0]

    return jsonify(result)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=80)
