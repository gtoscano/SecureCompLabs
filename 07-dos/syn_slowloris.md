# 🔐 Advanced DoS Defenses (SYN & Slowloris)

## ⚠️ Ethical & safety note (add this to your lab intro)

> We will NOT simulate SYN flood or Slowloris attacks in this lab.
> These techniques are extremely powerful, easily weaponized, and can disrupt real systems even unintentionally.
>
> Instead, we focus on **defensive hardening, detection, and response**, which is the responsibility of security professionals.

This keeps the lab aligned with blue-team training.

---

# 🛡️ Countermeasures — SYN Flood

SYN floods exhaust the TCP handshake queue, preventing legitimate connections.

## OS-level defenses (primary protection)

Run on the **host machine** (not inside containers):

```bash
# Enable SYN cookies
sudo sysctl -w net.ipv4.tcp_syncookies=1

# Increase backlog queue
sudo sysctl -w net.ipv4.tcp_max_syn_backlog=4096

# Increase socket queue
sudo sysctl -w net.core.somaxconn=4096
```

Make persistent:

```bash
sudo nano /etc/sysctl.conf
```

Add:

```
net.ipv4.tcp_syncookies = 1
net.ipv4.tcp_max_syn_backlog = 4096
net.core.somaxconn = 4096
```

Apply:

```bash
sudo sysctl -p
```

---

## Network-edge protections (enterprise reality)

Explain conceptually to students:

* firewall SYN rate limits
* load balancers absorb bursts
* CDNs terminate TCP handshakes
* IPS detects abnormal SYN ratios

Key teaching message:

> SYN defense belongs at the **kernel and edge**, not only the application.

---

# 🛡️ Countermeasures — Slowloris

Slowloris holds connections open slowly until the web server runs out of workers.

## Nginx protections

Edit inside container:

```bash
docker exec -it dos_target sh
```

Open config:

```bash
vi /etc/nginx/nginx.conf
```

Inside `http {}` add:

```nginx
limit_conn_zone $binary_remote_addr zone=perip:10m;

client_header_timeout 10s;
client_body_timeout   10s;
send_timeout          10s;
keepalive_timeout     15s;
```

Then open:

```bash
vi /etc/nginx/conf.d/default.conf
```

Inside `server {}` add:

```nginx
limit_conn perip 20;
```

Reload nginx:

```bash
nginx -t
nginx -s reload
```

---

## What these controls do (explain to students)

| Defense               | Effect                        |
| --------------------- | ----------------------------- |
| client_header_timeout | kills slow header senders     |
| keepalive_timeout     | closes idle sockets faster    |
| limit_conn            | caps connections per attacker |
| send_timeout          | stops stalled responses       |

Teaching point:

> Slowloris is defeated by **timeouts + connection limits**, not bandwidth.

---

# 🔎 How to detect which port is being attacked

You asked for a tool that shows **what port is under attack** — these are the best classroom options.

## 1) `ss` (most accurate, modern)

Inside monitor container or host:

```bash
ss -tulnp
```

Shows:

* open ports
* connection counts per port
* processes using them

Find busiest port:

```bash
ss -tan | awk '{print $4}' | cut -d: -f2 | sort | uniq -c | sort -nr
```

Students will see:

```
 400 80
  10 22
   3 443
```

Port 80 is being targeted.

---

## 2) `netstat` (classic)

```bash
netstat -ant | grep ':80' | wc -l
```

Shows number of connections hitting web server.

---

## 3) `iftop` (visual, great for class)

```bash
iftop
```

Students see:

* top traffic flows
* IPs hitting specific ports

Very intuitive.

---

## 4) `tcpdump` (SOC-style visibility)

```bash
tcpdump -ni any tcp port 80
```

Shows real-time packets targeting the port.

Explain:

> This is what analysts use during incident response.

---

# 🧪 Lab task addition (ready to paste)

## New exercise: Defensive hardening

Students must:

### Step A — Identify target service

```bash
ss -tulnp
```

Which port is receiving most traffic?

---

### Step B — Apply SYN protections (host)

Enable SYN cookies and backlog tuning.

---

### Step C — Apply Slowloris protections (nginx)

Add:

* connection limits
* timeouts

---

### Step D — Verify improvement

Students compare:

| Metric             | Before | After |
| ------------------ | ------ | ----- |
| open connections   |        |       |
| CPU                |        |       |
| latency            |        |       |
| port concentration |        |       |

---

# 🎓 Teaching narrative (important)

This creates a powerful realization:

1. Attacks are often simple.
2. Detection is observable via metrics.
3. Defense is configuration + architecture.
4. Prevention happens at:

   * kernel
   * app server
   * firewall
   * CDN

You’re training **defenders**, not attackers.
