# Cowrie Honeypot Lab (Docker Compose)

This lab runs a low-interaction SSH/Telnet honeypot with Cowrie.

## Quick start

```bash
cd lab-honeypot-cowrie
cp .env.example .env
docker compose up -d --build
docker compose ps
```

## Test from another machine

```bash
ssh -p 2222 root@<host-ip>
telnet <host-ip> 2223
```

## View logs

```bash
docker compose logs -f cowrie
docker compose exec cowrie tail -f /cowrie/cowrie-git/var/log/cowrie/cowrie.json
```

## Customize image

Edit `Dockerfile` for image-level customization.
Note: recent `cowrie/cowrie` images may not include a shell/package manager,
so shell-based `RUN apt-get ...` steps can fail. Prefer bind-mounted config
or use a different base image if you need extra tooling.

You can also pin a base image in `.env`:

```bash
BASE_IMAGE=cowrie/cowrie:latest
```

## Safety notes

- Use only in an isolated classroom/lab network.
- Do not expose to production networks.
- Restrict outbound traffic from the honeypot host.
