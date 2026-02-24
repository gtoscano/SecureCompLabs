FROM python:3.12-slim

WORKDIR /app
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY app.py .
COPY templates ./templates

ENV ALERT_FILE=/logs/alert
ENV LOG_VIEW_TOKEN=change-this-token
ENV MAX_ROWS=250

EXPOSE 8082
CMD ["gunicorn", "--bind", "0.0.0.0:8082", "--workers", "2", "app:app"]
