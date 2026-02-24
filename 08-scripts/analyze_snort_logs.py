# Goal: Aggregate Snort alerts to identify persistent attackers
# Scenario: Analyze Snort alert log for repeated attack patterns

import pandas as pd

# Load Snort alerts (CSV format)
df = pd.read_csv("snort_alerts.csv")

# Group by source IP and alert type
grouped = df.groupby(['src_ip', 'alert_msg']).size().reset_index(name='count')

# Find sources with >10 alerts of same type
repeat_offenders = grouped[grouped['count'] > 10]

print("Persistent attack sources:")
print(repeat_offenders.sort_values('count', ascending=False))

# Additional analysis: time-based clustering
df['timestamp'] = pd.to_datetime(df['timestamp'])
df['hour'] = df['timestamp'].dt.hour
hourly = df.groupby('hour').size()
print("\nAlerts by hour (identify attack time patterns):")
print(hourly)
