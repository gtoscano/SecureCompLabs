import os
from pathlib import Path

from flask import Flask, abort, render_template, request

ALERT_FILE = Path(os.getenv("ALERT_FILE", "/logs/alert"))
LOG_VIEW_TOKEN = os.getenv("LOG_VIEW_TOKEN", "change-this-token")
MAX_ROWS = int(os.getenv("MAX_ROWS", "250"))

app = Flask(__name__)


def authorized() -> bool:
    token = request.args.get("token") or request.headers.get("X-Log-Token")
    return bool(token and token == LOG_VIEW_TOKEN)


def read_alerts() -> list[str]:
    if not ALERT_FILE.exists():
        return []

    lines = ALERT_FILE.read_text(encoding="utf-8", errors="ignore").splitlines()
    return list(reversed(lines[-MAX_ROWS:]))


@app.get("/")
def index():
    if not authorized():
        abort(403)

    alerts = read_alerts()
    return render_template(
        "index.html",
        alerts=alerts,
        token=LOG_VIEW_TOKEN,
        alert_file=str(ALERT_FILE),
        max_rows=MAX_ROWS,
    )


@app.get("/health")
def health():
    return {"status": "ok"}


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8082)
