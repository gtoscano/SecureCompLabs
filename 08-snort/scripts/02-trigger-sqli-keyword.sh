#!/usr/bin/env bash
set -euo pipefail

# Purpose:
# - Generate HTTP traffic that triggers Snort rule sid:1000001 (SQLi keyword "' OR 1=1").
# - Capture that traffic into a host file: ./pcaps/02-sqli-keyword.pcap.
# - Copy the capture to ./pcaps/sample.pcap so the lab picks it up by default.
# Usage:
# - Run this script, then execute: docker compose restart snort

LAB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PCAP_DIR="$LAB_DIR/pcaps"
PCAP_OUT="$PCAP_DIR/02-sqli-keyword.pcap"
HTTP_CONTAINER="snort-lab-http"

if [[ "$(uname -s)" == "Darwin" ]]; then
  IFACE="lo0"
else
  IFACE="lo"
fi

mkdir -p "$PCAP_DIR"

# Authenticate sudo before background capture to avoid hidden password prompts.
echo "Sudo access is required for tcpdump capture."
sudo -v

cleanup() {
  if [[ -n "${TCPDUMP_PID:-}" ]]; then
    kill "$TCPDUMP_PID" >/dev/null 2>&1 || true
  fi
  docker rm -f "$HTTP_CONTAINER" >/dev/null 2>&1 || true
}
trap cleanup EXIT

docker rm -f "$HTTP_CONTAINER" >/dev/null 2>&1 || true
docker run -d --name "$HTTP_CONTAINER" -p 80:80 nginx:alpine >/dev/null

sudo tcpdump -i "$IFACE" -w "$PCAP_OUT" tcp port 80 >/dev/null 2>&1 &
TCPDUMP_PID=$!
sleep 1

curl -sS -X POST "http://127.0.0.1/" -d "q=' OR 1=1" >/dev/null || true
sleep 1

kill "$TCPDUMP_PID" >/dev/null 2>&1 || true
unset TCPDUMP_PID

if [[ ! -f "$PCAP_OUT" ]]; then
  echo "Failed to create pcap: $PCAP_OUT"
  echo "Check sudo/tcpdump permissions and whether port 80 capture is allowed."
  exit 1
fi

cp "$PCAP_OUT" "$PCAP_DIR/sample.pcap"
echo "Created: $PCAP_OUT"
echo "Updated: $PCAP_DIR/sample.pcap"
echo "Next: cd \"$LAB_DIR\" && docker compose restart snort"
