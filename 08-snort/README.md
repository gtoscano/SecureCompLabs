# Snort IDS Lab (Docker Compose)

This lab includes:
- `snort`: IDS engine using custom local rules.
- `snort-viewer`: simple web UI for alert logs.

The default mode is **pcap** so you can safely analyze captured traffic in class.

## Snort modes and differences

### General Snort modes

Snort is commonly described with these core operating modes:

- **Sniffer mode**: reads packets and prints them to the console for inspection.
- **Packet logger mode**: captures packets and writes them to files for later analysis.
- **NIDS/IDS mode**: inspects traffic against rules and generates alerts on matches.
- **Inline/IPS mode** (advanced deployments): can actively drop/block traffic, not just alert.

### Lab runtime modes

This lab exposes two practical runtime modes through `.env`:

- **`SNORT_MODE=pcap`**: analyzes an existing capture file (`./pcaps/sample.pcap`) and then exits scan mode.
- **`SNORT_MODE=live`**: listens on an interface (for example `eth0`) and analyzes traffic as it arrives.

Use `pcap` for repeatable classroom exercises and `live` for real-time traffic observation.

## Quick start

```bash
cd 08-snort
cp .env.example .env
docker compose up -d --build
```

Open alert viewer:
- `http://localhost:8082/?token=<LOG_VIEW_TOKEN>`

## Create `sample.pcap` (required in pcap mode)

If you see:
- `missing pcap file: /lab/pcaps/sample.pcap`

run one of the scripts below. Each script:
- creates its own pcap in `./pcaps`
- copies that capture to `./pcaps/sample.pcap` (the default file Snort reads)

```bash
cd 08-snort
chmod +x scripts/*.sh
./scripts/01-trigger-admin-probe.sh
```

Then restart Snort so it analyzes the new `sample.pcap`:

```bash
docker compose restart snort
docker compose logs -f snort
```

## Add traffic for analysis

1. Put a capture file in `./pcaps` named in `.env` (default: `sample.pcap`).
2. Restart Snort to re-run analysis:

```bash
docker compose restart snort
```

3. Watch alerts:

```bash
docker compose logs -f snort
docker compose logs -f snort-viewer
```

## Example trigger scripts

Three scripts are included in `./scripts` to generate host-accessible pcaps and update `./pcaps/sample.pcap`:

- `scripts/01-trigger-admin-probe.sh` -> triggers `sid:1000002` (`/admin`)
- `scripts/02-trigger-sqli-keyword.sh` -> triggers `sid:1000001` (`' OR 1=1`)
- `scripts/03-trigger-cmd-injection.sh` -> triggers `sid:1000003` (`;` / `||` / `&&`)

Run one script to create `sample.pcap`, then restart Snort:

```bash
chmod +x scripts/*.sh
./scripts/01-trigger-admin-probe.sh
docker compose restart snort
```

Notes:
- Scripts use `sudo tcpdump` to capture traffic to local port `80`.
- Generated files are on the host under `./pcaps`.

## Switch to live sniff mode

Edit `.env`:

```bash
SNORT_MODE=live
SNORT_INTERFACE=eth0
```

Then restart:

```bash
docker compose up -d
```

## Files you can customize

- `rules/local.rules`: detection logic for your class experiments.
- `snort.conf`: minimal Snort config used by this lab.
- `Dockerfile`: install extra tools or pin package versions.
- `viewer/app.py`: alert dashboard behavior.
- `scripts/*.sh`: repeatable trigger traffic examples.

## Safety notes

- Use this lab only in isolated classroom/lab environments.
- Do not deploy to production networks.
- Keep `LOG_VIEW_TOKEN` private.

## Check active rules

Use these commands to verify what Snort is using:

1. Enabled (loaded) rules:

```bash
docker compose exec -T snort snort -T -c /etc/snort/snort.conf
```

Look for summary lines such as:
- `3 Snort rules read`
- `3 detection rules`

2. Rules that actually fired:

```bash
tail -n 100 logs/alert
```

Each alert line includes `[gid:sid:rev]`:
- `sid` = rule ID
- `rev` = rule revision

3. Quick list of local enabled rule definitions:

```bash
rg -n '^alert ' rules/local.rules
```
