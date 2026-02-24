FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
      snort \
      ca-certificates \
      iproute2 \
      iputils-ping \
      tcpdump \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /lab

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
