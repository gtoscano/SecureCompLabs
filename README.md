# Secure Computing Labs

## Honeypot labs

- `lab-honeypot-cowrie`: SSH/Telnet honeypot with Cowrie.
- `lab-web-honeypot`: Web honeypot with fake login UI and protected log dashboard.
- `lab-snort`: Snort IDS lab with custom rules and a web alert viewer.
- `08-honey-cowrie`: Cowrie honeypot variant used in module 08.
- `08-web-honeypot`: Web honeypot variant used in module 08.
- `08-snort`: Snort IDS lab variant used in module 08.

## Quick start

### Cowrie honeypot

```bash
cd lab-honeypot-cowrie
cp .env.example .env
docker compose up -d --build
```

### Web honeypot

```bash
cd lab-web-honeypot
cp .env.example .env
docker compose up -d --build
```

### Snort IDS lab

```bash
cd lab-snort
cp .env.example .env
docker compose up -d --build
```

### Module 08 labs

```bash
cd 08-honey-cowrie && docker compose up -d --build
cd 08-web-honeypot && docker compose up -d --build
cd 08-snort && cp .env.example .env && docker compose up -d --build
```

## Notes

- Run these labs only in isolated classroom/lab environments.
- Do not deploy to production networks.
