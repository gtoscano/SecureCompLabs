# Snort — Detailed Overview and Practical Examples

Snort is an open-source **Network Intrusion Detection System (NIDS)** and **Intrusion Prevention System (IPS)** that analyzes network traffic in real time to detect malicious activity, policy violations, and suspicious patterns.

Originally developed by Martin Roesch and now maintained by Cisco Talos, Snort is one of the most widely used IDS tools in academia and industry.

---

# 1. What Snort Is Used For

Snort helps security teams:

- Monitor network traffic
- Detect attacks and exploits
- Generate alerts for suspicious activity
- Support forensic investigations
- Enforce security policies

It can operate in two main modes:

| Mode | Purpose |
|------|---------|
| IDS | Detect and alert |
| IPS | Detect and block |

---

# 2. How Snort Works

Snort analyzes packets as they traverse a network.

### Core workflow

```

Network Traffic
↓
Packet Capture
↓
Preprocessors (normalize data)
↓
Detection Engine (rules)
↓
Alerts / Logs / Actions

```

---

# 3. Snort Architecture

## Components

### 1) Packet Decoder
- Captures packets from the network
- Identifies protocols (Ethernet, IP, TCP, HTTP, etc.)

### 2) Preprocessors
- Clean and normalize traffic
- Reassemble fragmented packets
- Decode protocols

Examples:
- HTTP normalization
- TCP stream reassembly
- Port scan detection

### 3) Detection Engine
- Compares traffic against rules
- Matches signatures and patterns

### 4) Output Modules
- Alerts
- Logs
- Databases
- SIEM integration

---

# 4. Snort Detection Approaches

Snort mainly uses **signature-based detection**, but also supports behavioral and protocol analysis.

| Method | Description |
|--------|-------------|
| Signature | Matches known attack patterns |
| Protocol analysis | Detects malformed or suspicious packets |
| Behavioral | Detects abnormal traffic patterns |

---

# 5. Snort Rule Structure

Snort rules define what traffic is suspicious.

## General format

```

action protocol source_ip source_port -> destination_ip destination_port (options)

```

### Example

```

alert tcp any any -> 10.0.0.25 80 (msg:"Exploit detected"; content:"cmd.exe"; sid:1000001;)

```

---

# 6. Rule Components Explained

## Rule Header

| Element | Meaning |
|--------|---------|
| alert | Action |
| tcp | Protocol |
| any any | Source IP/port |
| -> | Direction |
| 10.0.0.25 80 | Destination |

## Rule Options

Inside parentheses:

| Option | Purpose |
|--------|---------|
| msg | Alert message |
| content | Payload match |
| sid | Signature ID |
| rev | Rule revision |
| classtype | Attack classification |
| nocase | Case-insensitive |

---

# 7. Practical Examples

## Example 1 — Detect Port Scanning

```

alert tcp any any -> any 22 (msg:"SSH Scan Detected"; flags:S; sid:1000002;)

```

Detects repeated SYN packets targeting SSH.

---

## Example 2 — Detect SQL Injection

```

alert tcp any any -> any 80 (msg:"SQL Injection Attempt"; content:"' OR 1=1"; sid:1000003;)

```

Matches common SQL injection string.

---

## Example 3 — Detect Malware Command & Control

```

alert tcp any any -> any 443 (msg:"Suspicious C2 Traffic"; content:"malicious-domain.com"; sid:1000004;)

```

Detects connections to known malicious domains.

---

## Example 4 — Detect Unauthorized File Access

```

alert tcp any any -> any 80 (msg:"Access to /etc/passwd attempt"; content:"/etc/passwd"; sid:1000005;)

```

Triggers when attacker attempts directory traversal.

---

# 8. Running Snort (Basic Commands)

## Start Snort in IDS mode

```

snort -c /etc/snort/snort.conf -i eth0

```

## Test configuration

```

snort -T -c /etc/snort/snort.conf

```

## Log traffic

```

snort -dev -i eth0

```

---

# 9. When Snort Generates Alerts

Snort creates alerts when traffic matches:

- Known exploit signatures
- Suspicious payload patterns
- Protocol anomalies
- Abnormal connection behavior

Example alert:

```

[**] [1:1000003:1] SQL Injection Attempt [**]
[Classification: Web Application Attack]
[Priority: 1]
192.168.1.12:53422 -> 10.0.0.25:80

```

---

# 10. Strengths of Snort

- Open source
- Large community rule sets
- Highly customizable
- Real-time detection
- Lightweight compared to enterprise tools

---

# 11. Limitations

- Signature dependent
- Needs frequent rule updates
- Cannot inspect encrypted payloads easily
- Requires tuning to reduce false positives

---

# 12. Snort vs Other Tools

| Tool | Focus |
|------|------|
| Snort | Signature-based IDS/IPS |
| Suricata | High-performance IDS/IPS |
| Zeek | Network behavior analysis |
| Wireshark | Packet analysis (manual) |

---

# 13. Real-World Use Cases

- Enterprise network monitoring
- SOC environments
- Academic cybersecurity labs
- Incident response investigations
- Malware detection

---

# 14. Best Practices

- Keep rules updated
- Tune alerts to environment
- Combine with SIEM tools
- Use alongside HIDS
- Monitor logs regularly

---

# 15. Key Takeaway

Snort is a foundational tool for understanding network intrusion detection.

It demonstrates how:
- Network traffic can be monitored
- Attacks leave observable patterns
- Detection relies on rules, context, and analysis

Snort is not just a tool — it’s a **framework for thinking about network security visibility**.
