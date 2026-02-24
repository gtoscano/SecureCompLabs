#!/usr/bin/env sh
set -eu

MODE="${SNORT_MODE:-pcap}"
INTERFACE="${SNORT_INTERFACE:-eth0}"
PCAP_FILE="${SNORT_PCAP:-sample.pcap}"
CONF="${SNORT_CONF:-/etc/snort/snort.conf}"
RULES_FILE="${SNORT_RULES:-/etc/snort/rules/local.rules}"
LOG_DIR="${SNORT_LOG_DIR:-/lab/logs}"

mkdir -p "$LOG_DIR"
mkdir -p /var/log/snort

if [ ! -f "$RULES_FILE" ]; then
  echo "[snort] missing rules file: $RULES_FILE"
  exit 1
fi

if [ ! -f "$CONF" ]; then
  echo "[snort] missing config: $CONF"
  exit 1
fi

echo "[snort] mode=$MODE conf=$CONF rules=$RULES_FILE log_dir=$LOG_DIR"

if [ "$MODE" = "pcap" ]; then
  if [ ! -f "/lab/pcaps/$PCAP_FILE" ]; then
    echo "[snort] missing pcap file: /lab/pcaps/$PCAP_FILE"
    echo "[snort] add a pcap to ./pcaps and set SNORT_PCAP in .env"
    tail -f /dev/null
  fi

  # Loopback/offloaded captures can carry invalid checksums; ignore them in pcap lab mode.
  snort -q -k none -c "$CONF" -A fast -l "$LOG_DIR" -r "/lab/pcaps/$PCAP_FILE" || true
  echo "[snort] pcap scan completed. keeping container alive for log viewer"
  tail -f /dev/null
fi

snort -q -c "$CONF" -A fast -l "$LOG_DIR" -i "$INTERFACE"
