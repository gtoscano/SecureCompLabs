# DoS Defense Lab (Docker)

## Ethics and scope (Defense Only)
This lab is for **defensive monitoring and hardening**.
It does **not** include attack execution steps for SYN flood, Slowloris, or any other DoS attack procedures.
Use this environment only for lawful classroom activities.

## Files
- `docker-compose.yml`
- `prometheus.yml`
- `nginx/default.conf`
- `grafana/provisioning/datasources/datasource.yml`
- `grafana/provisioning/dashboards/dashboard.yml`
- `grafana/dashboards/dos-lab-dashboard.json`

## Start and stop
```bash
docker compose up -d
docker compose down
```

## URLs
- Nginx target: http://localhost:8080
- Grafana: http://localhost:3000 (admin/admin)
- Prometheus targets: http://localhost:9090/targets
- cAdvisor UI: http://localhost:8081

## Verify monitoring stack
1. Open Prometheus targets page and confirm `UP` for:
   - `prometheus`
   - `cadvisor`
   - `nginx-exporter`
2. Open Grafana and confirm:
   - Prometheus datasource exists automatically
   - Dashboard `DoS Lab - Defense Monitoring` is present in folder `DoS Lab`

## Baseline defensive observation commands
Run from the monitor container:
```bash
docker compose exec monitor sh
```

Inside the container:
```bash
# Socket summary (TCP states)
ss -s

# List established TCP connections
ss -tan state established

# Interface bandwidth view (interactive)
iftop -n

# Packet capture for HTTP port 80 only
tcpdump -ni any tcp port 80
```

## Safe stress test (normal HTTP load only)
Run from attacker container:
```bash
docker compose exec attacker sh
wrk -t2 -c50 -d30s http://target/
```

This generates normal HTTP load so you can observe metric changes in:
- Grafana (CPU, memory, network, nginx connections, request rate)
- Prometheus target and query pages
- cAdvisor container resource panels

## Countermeasures (Defense Only)

### SYN flood countermeasures (defensive sysctl examples)
Apply on Linux host (defensive tuning examples only):
```bash
sudo sysctl -w net.ipv4.tcp_syncookies=1
sudo sysctl -w net.ipv4.tcp_max_syn_backlog=4096
sudo sysctl -w net.core.somaxconn=4096
```
Persist via `/etc/sysctl.conf` or `/etc/sysctl.d/*.conf` in production change control.

### Slowloris countermeasures (Nginx hardening)
Configured in `nginx/default.conf`:
- `client_header_timeout 10s;`
- `client_body_timeout 10s;`
- `keepalive_timeout 15s;`
- `send_timeout 10s;`
- `limit_conn_zone` + `limit_conn` per client IP

## Identify the attacked port
Use these defensive commands to see which destination port is receiving the most connections.

```bash
# Aggregate by destination port from established TCP sockets
ss -tan state established | awk '{print $4}' | awk -F: '{print $NF}' | sort | uniq -c | sort -nr
```

```bash
# Live interface-level traffic view
iftop -n
```

```bash
# Focus packet capture on TCP/80
tcpdump -ni any tcp port 80
```
