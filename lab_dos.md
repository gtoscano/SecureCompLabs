
# 🧑‍🏫 Secure Computing Lab Kit

## Controlled DoS Simulation & Defense (Linux + Docker)

**Course:** Secure Computing
**Format:** 1 session (75 min)
**Environment:** Linux lab + Docker
**Focus:** Detection, monitoring, mitigation (not attacking)

---

# 🎯 Learning Outcomes

By the end, students will:

1. Recognize normal vs abnormal traffic behavior
2. Observe DoS effects using system and network telemetry
3. Correlate monitoring signals (CPU, sockets, traffic)
4. Apply defensive mitigation (rate limiting)
5. Explain SOC-style response workflow

Mapped to slide topics:

* NetFlow concept → traffic anomalies
* Packet analysis → connection patterns
* System metrics → CPU/memory
* Defensive tooling → rate limiting, firewall

---

# 🧭 Session Flow (Instructor Timeline)

| Time  | Activity                   |
| ----- | -------------------------- |
| 0–10  | Ethics + intro             |
| 10–20 | Lab setup                  |
| 20–30 | Baseline monitoring        |
| 30–45 | Controlled DoS simulation  |
| 45–60 | Detection analysis         |
| 60–70 | Mitigation (rate limiting) |
| 70–75 | Reflection + wrap          |

---

# ⚖️ Ethics & Legal Framing (OPEN WITH THIS)

Use this script:

> Today we simulate system stress on infrastructure we own.
> Unauthorized DoS attacks are illegal and unethical.
> The same techniques used here are used for:
>
> * resilience testing
> * capacity planning
> * defensive cybersecurity training

Students must agree:

* lab traffic stays inside Docker network
* no tools run outside class environment

---

# 🧪 Lab Architecture

```
[attacker container]  --->  [nginx target]  --->  [monitor container]
                                  |
                             system metrics
                             logs
                             connections
```

Everything local, isolated, safe.

---

# 🛠️ Instructor Setup

## 1) Provide docker-compose file

Students copy:

```yaml
version: "3.9"

services:

  target:
    image: nginx:latest
    container_name: dos_target
    ports:
      - "8080:80"
    networks:
      - dosnet

  attacker:
    image: williamyeh/wrk
    container_name: dos_attacker
    entrypoint: ["sleep", "infinity"]
    networks:
      - dosnet

  monitor:
    image: nicolaka/netshoot
    container_name: dos_monitor
    entrypoint: ["sleep", "infinity"]
    networks:
      - dosnet

networks:
  dosnet:
```

Run:

```bash
docker compose up -d
```

---

# 📊 Student Worksheet

## Phase 1 — Baseline

Open:

```
http://localhost:8080
```

Record:

```bash
docker stats
```

Inside monitor:

```bash
docker exec -it dos_monitor bash
iftop
ss -s
netstat -an | wc -l
```

Questions:

* CPU usage?
* Connections?
* Traffic volume?

This is NORMAL.

---

## Phase 2 — Simulated DoS

Inside attacker:

```bash
docker exec -it dos_attacker sh
wrk -t4 -c400 -d60s http://target
```

Observe in parallel:

```bash
docker stats
```

Inside monitor:

```bash
iftop
ss -s
```

Inside target:

```bash
docker exec -it dos_target bash
top
```

Students record:

* CPU spike?
* latency?
* connection count?
* dropped requests?

---

# 🔎 Detection Prompts (Instructor)

Ask live:

* What changed first?
* Network or CPU?
* Was it bandwidth or connection exhaustion?
* What would NetFlow show?
* What would Zeek flag?

Tie to:

* anomaly detection
* telemetry correlation

---

# 🛡️ Mitigation Phase

Students now defend the server.

Inside target:

```bash
docker exec -it dos_target bash
apt update && apt install -y vim
```

Edit:

```bash
vim /etc/nginx/nginx.conf
```

Inside `http {}`:

```
limit_req_zone $binary_remote_addr zone=mylimit:10m rate=5r/s;
```

Inside `server {}`:

```
limit_req zone=mylimit burst=10 nodelay;
```

Reload:

```bash
nginx -s reload
```

---

## Run attack again

```bash
wrk -t4 -c400 -d60s http://target
```

Students compare:

| Metric      | Before | After |
| ----------- | ------ | ----- |
| CPU         |        |       |
| Connections |        |       |
| Errors      |        |       |
| Latency     |        |       |

Discussion:

* what improved?
* what failed?
* what enterprise systems do this?

---

# 🧠 Reflection Questions (graded)

1. Which metric indicated the issue earliest?
2. Was this bandwidth exhaustion or connection exhaustion?
3. Which monitoring layer detected it first?
4. Why did rate limiting help?
5. Where would mitigation occur in production?

   * CDN
   * WAF
   * edge firewall
   * autoscaling

---

# 🧾 Grading Rubric (quick)

| Component                 | Points |
| ------------------------- | ------ |
| Baseline observations     | 20     |
| Detection analysis        | 25     |
| Mitigation implementation | 25     |
| Reflection answers        | 30     |

---

# 🧑‍🏫 Instructor Talking Points

Use these during lab:

### Key insight #1

DoS is usually resource exhaustion, not “hacking.”

### Key insight #2

Monitoring detects anomalies before failure.

### Key insight #3

Defense happens at multiple layers:

* application
* host
* network
* edge

### Key insight #4

Rate limiting is the first defensive control.

---

# 🚀 Advanced Extension (next class)

You can escalate realism:

### Add Zeek

Students see connection behavior.

### Add Suricata

Students see IDS alerts.

### Add Grafana

Students visualize metrics.

### Simulate:

* SYN flood
* slowloris
* botnet patterns

---

# 🧰 Future Version I Can Build For You

I can generate a **v2 lab** tailored to your Secure Computing course:

* one-command install script
* prebuilt dashboards
* Zeek + Suricata integration
* incident-response scenario
* “red vs blue” team mode
* homework follow-up
