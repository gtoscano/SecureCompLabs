import json
import os
import sqlite3
from datetime import datetime, timezone
from urllib.parse import urlencode

from flask import Flask, abort, g, render_template, request

DB_PATH = os.getenv("DB_PATH", "/data/honeypot.db")
LOG_VIEW_TOKEN = os.getenv("LOG_VIEW_TOKEN", "change-me")
MAX_LOG_ROWS = int(os.getenv("MAX_LOG_ROWS", "200"))

app = Flask(__name__)


def get_db() -> sqlite3.Connection:
    if "db" not in g:
        g.db = sqlite3.connect(DB_PATH)
        g.db.row_factory = sqlite3.Row
    return g.db


def init_db() -> None:
    conn = sqlite3.connect(DB_PATH)
    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS request_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ts TEXT NOT NULL,
            remote_addr TEXT,
            method TEXT,
            path TEXT,
            query_string TEXT,
            user_agent TEXT,
            form_data TEXT,
            json_data TEXT,
            status_code INTEGER,
            notes TEXT
        )
        """
    )
    conn.commit()
    conn.close()


def initialize_storage() -> None:
    db_dir = os.path.dirname(DB_PATH)
    if db_dir:
        os.makedirs(db_dir, exist_ok=True)
    init_db()


def parse_payload() -> tuple[str, str]:
    form_data = ""
    json_data = ""

    if request.form:
        safe_form = {k: request.form.get(k, "") for k in request.form.keys()}
        form_data = json.dumps(safe_form, ensure_ascii=True)

    if request.is_json:
        try:
            payload = request.get_json(silent=True)
            if payload is not None:
                json_data = json.dumps(payload, ensure_ascii=True)
        except Exception:
            json_data = ""

    return form_data, json_data


def build_notes() -> str:
    if request.path == "/login" and request.method == "POST":
        username = request.form.get("username", "")
        password = request.form.get("password", "")
        return f"credential_attempt username={username!r} password={password!r}"
    return ""


def log_request(status_code: int) -> None:
    form_data, json_data = parse_payload()
    conn = get_db()
    conn.execute(
        """
        INSERT INTO request_logs (
            ts, remote_addr, method, path, query_string,
            user_agent, form_data, json_data, status_code, notes
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        """,
        (
            datetime.now(timezone.utc).isoformat(),
            request.headers.get("X-Forwarded-For", request.remote_addr),
            request.method,
            request.path,
            request.query_string.decode("utf-8", errors="ignore"),
            request.headers.get("User-Agent", ""),
            form_data,
            json_data,
            status_code,
            build_notes(),
        ),
    )
    conn.commit()


def check_logs_access() -> None:
    token = request.args.get("token") or request.headers.get("X-Log-Token")
    if not token or token != LOG_VIEW_TOKEN:
        abort(403)


@app.teardown_appcontext
def close_db(_exception) -> None:
    db = g.pop("db", None)
    if db is not None:
        db.close()


@app.after_request
def after(response):
    try:
        if request.path != "/favicon.ico":
            log_request(response.status_code)
    except Exception:
        pass
    return response


@app.get("/health")
def health():
    return {"status": "ok"}


@app.get("/")
def index():
    return render_template("index.html")


@app.post("/login")
def login():
    return render_template("index.html", error="Invalid username or password."), 401


@app.route("/wp-login.php", methods=["GET", "POST"])
def wp_login():
    return render_template("index.html", error="Invalid username or password."), 401


@app.get("/logs")
def logs_view():
    check_logs_access()

    conn = get_db()
    rows = conn.execute(
        """
        SELECT id, ts, remote_addr, method, path, status_code, notes
        FROM request_logs
        ORDER BY id DESC
        LIMIT ?
        """,
        (MAX_LOG_ROWS,),
    ).fetchall()

    top_ips = conn.execute(
        """
        SELECT COALESCE(remote_addr, 'unknown') AS key, COUNT(*) AS count
        FROM request_logs
        GROUP BY key
        ORDER BY count DESC
        LIMIT 10
        """
    ).fetchall()

    top_paths = conn.execute(
        """
        SELECT path AS key, COUNT(*) AS count
        FROM request_logs
        GROUP BY path
        ORDER BY count DESC
        LIMIT 10
        """
    ).fetchall()

    return render_template(
        "logs.html",
        rows=rows,
        top_ips=top_ips,
        top_paths=top_paths,
        token=LOG_VIEW_TOKEN,
        max_rows=MAX_LOG_ROWS,
    )


@app.route("/<path:any_path>", methods=["GET", "POST", "PUT", "DELETE", "PATCH", "OPTIONS"])
def catch_all(any_path):
    return (
        "Not Found",
        404,
    )


initialize_storage()


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)
