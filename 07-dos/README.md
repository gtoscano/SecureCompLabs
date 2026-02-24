# ✅ Lab DoS Symptoms + Monitoring + Mitigation (Docker)

* Establish a **baseline**
* Generate **controlled stress** (HTTP load test)
* Use monitoring tools to answer:

  * *What is happening?*
  * *Which port is being hit?*
  * *Is it CPU, connections, or bandwidth?*
* Apply **defensive mitigations** (rate limit + timeouts + connection limits)
* Understand **SYN & Slowloris defenses** (configured, not executed)

---

# 0) Files (copy/paste)

## `docker-compose.yaml`

```yaml
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
    entrypoint: ["sh", "-c", "while true; do sleep 3600; done"]
    networks:
      - dosnet

  monitor:
    image: nicolaka/netshoot
    container_name: dos_monitor
    entrypoint: ["sh", "-c", "while true; do sleep 3600; done"]
    networks:
      - dosnet

networks:
  dosnet: {}
```

Start:

```bash
docker compose up -d
docker ps
```

Sanity check:

```bash
curl -I http://localhost:8080
```

---

# 1) Baseline (10 minutes)

## A. Host view

```bash
docker stats
```

## B. Monitoring view (inside `monitor`)

```bash
docker exec -it dos_monitor sh
```

### “What port is being attacked?” (baseline)

```sh
ss -tuln
```

### Connection snapshot

```sh
ss -tan | head
ss -s
```

### Who is hitting which port? (counts by destination port)

```sh
ss -tan | awk '{print $4}' | cut -d: -f2 | sort | uniq -c | sort -nr | head
```

### Traffic view

```sh
iftop
```

Have students write down baseline:

* CPU/mem of `dos_target`
* total TCP connections
* destination-port distribution

---

# 2) Controlled “DoS-like” Stress (10 minutes)

This is a **legitimate load test** to create observable symptoms.

From attacker:

```bash
docker exec -it dos_attacker sh
wrk -t4 -c300 -d45s http://target/
```

While it runs, students watch:

## A. On host

```bash
docker stats
```

## B. In monitor

```sh
ss -s
ss -tan | awk '{print $4}' | cut -d: -f2 | sort | uniq -c | sort -nr | head
ss -tan state established | wc -l
iftop
```

## C. Optional packet peek (SOC vibe)

```sh
tcpdump -ni any tcp port 80
```

Discussion prompts:

* Which metric changed first?
* Does this look like bandwidth exhaustion or connection exhaustion?
* What port is receiving most connections?

---

# 3) Add Defensive Controls (20 minutes)

## 3A) Nginx rate limiting (HTTP flood mitigation)

Inside target:

```bash
docker exec -it dos_target sh
```

Edit `/etc/nginx/nginx.conf` (add zone inside `http {}`):

```sh
vi /etc/nginx/nginx.conf
```

Add inside `http {}`:

```nginx
limit_req_zone $binary_remote_addr zone=req_limit:10m rate=10r/s;
```

Now apply it in `server {}` (it’s in `/etc/nginx/conf.d/default.conf`):

```sh
vi /etc/nginx/conf.d/default.conf
```

Inside `location / {}` add:

```nginx
limit_req zone=req_limit burst=20 nodelay;
```

Reload:

```sh
nginx -t && nginx -s reload
```

---

## 3B) Slowloris countermeasures (timeouts + connection caps)

⚠️ Add this note to the lab:

> We do NOT demonstrate Slowloris in class because it is easily weaponized.
> We only configure and verify defensive protections.

Edit nginx.conf again (`http {}`):

```sh
vi /etc/nginx/nginx.conf
```

Add inside `http {}`:

```nginx
limit_conn_zone $binary_remote_addr zone=perip:10m;

client_header_timeout 10s;
client_body_timeout   10s;
send_timeout          10s;
keepalive_timeout     15s;
```

Apply in `server {}` (default.conf):

```sh
vi /etc/nginx/conf.d/default.conf
```

Add inside `server {}` (or inside `location /`):

```nginx
limit_conn perip 20;
```

Reload:

```sh
nginx -t && nginx -s reload
```

---

## 3C) SYN flood countermeasures (host-only kernel hardening)

⚠️ Add this note too:

> We do NOT demonstrate SYN floods in class.
> We configure defensive kernel settings that mitigate handshake exhaustion.

On the Docker **host**:

```bash
sudo sysctl -w net.ipv4.tcp_syncookies=1
sudo sysctl -w net.ipv4.tcp_max_syn_backlog=4096
sudo sysctl -w net.core.somaxconn=4096
```

(Optionally) show current values:

```bash
sysctl net.ipv4.tcp_syncookies net.ipv4.tcp_max_syn_backlog net.core.somaxconn
```

---

# 4) Verify defenses (10 minutes)

Run the same load test again:

```bash
docker exec -it dos_attacker sh
wrk -t4 -c300 -d45s http://target/
```

Students compare before/after:

## Metrics to record

* Requests/sec and latency (from `wrk` output)
* `docker stats` CPU for `dos_target`
* `ss -s` TCP summary
* Destination port counts:

```sh
ss -tan | awk '{print $4}' | cut -d: -f2 | sort | uniq -c | sort -nr | head
```

Expected outcome:

* The server remains responsive longer
* Some requests get throttled (rate limit)
* Connection behavior is more controlled
* Timeouts prevent long-held sockets

---

# 5) Wrap-up (5 minutes)

Ask:

1. What port was “under attack” and how did you prove it? (`ss` port counts + tcpdump)
2. Which mitigation was most effective for HTTP load?
3. Why do SYN & Slowloris defenses live at different layers?
4. If this were production, where would you add protection next? (CDN/WAF/LB)

---

