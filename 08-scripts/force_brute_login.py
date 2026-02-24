
# HIDS Log Analysis (Defensive)
# Goal: Parse system authentication logs to identify brute force attempts
# Scenario: Analyze `/var/log/auth.log` for suspicious login patterns

import re
from collections import defaultdict

failed_logins = defaultdict(int)

with open("auth.log") as f:
    for line in f:
        # Match failed password attempts
        match = re.search(r"Failed password for (\w+) from ([\d.]+)", line)
        if match:
            user, ip = match.groups()
            failed_logins[ip] += 1

# Flag IPs with >10 failed attempts
print("Potential brute force attacks:")
for ip, count in failed_logins.items():
    if count > 10:
        print(f"{ip}: {count} failed attempts")

