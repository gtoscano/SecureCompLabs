FROM python:3.12-slim

WORKDIR /app

RUN addgroup --system app && adduser --system --ingroup app app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY app.py .
COPY templates ./templates

RUN mkdir -p /data && chown -R app:app /app /data

ENV DB_PATH=/data/honeypot.db
ENV LOG_VIEW_TOKEN=change-me
ENV MAX_LOG_ROWS=200

USER app

EXPOSE 8080

CMD ["gunicorn", "--bind", "0.0.0.0:8080", "--workers", "2", "app:app"]
