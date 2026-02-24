# Web Honeypot Lab (Flask)

This lab provides:
- A fake login web interface (`/`) to attract credential attempts.
- A protected log dashboard (`/logs?token=...`) to inspect attacker behavior.

## Quick start

```bash
cd lab-web-honeypot
cp .env.example .env
docker compose up -d --build
```

Open:
- Honeypot page: `http://localhost:8080/`
- Logs page: `http://localhost:8080/logs?token=<your-token>`

## What gets logged

Each request records:
- Timestamp (UTC)
- Source IP (`X-Forwarded-For` if present, else remote address)
- HTTP method + path + query string
- User-Agent
- Submitted form/json payload
- HTTP status code
- Notes (includes captured credentials from `/login` attempts)

## Test examples

```bash
curl -i http://localhost:8080/
curl -i -X POST http://localhost:8080/login -d "username=admin&password=admin123"
curl -i http://localhost:8080/wp-login.php
curl -i "http://localhost:8080/logs?token=$(grep LOG_VIEW_TOKEN .env | cut -d= -f2)"
```

## Safety notes

- Run only in an isolated lab network.
- Do not expose this to production infrastructure.
- Use a strong `LOG_VIEW_TOKEN` before any internet exposure.
