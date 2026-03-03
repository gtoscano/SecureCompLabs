# docker build -t ubuntu-12 .
# docker run -it ubuntu-12
# docker exec -it ubuntu-12 bash
#
FROM ubuntu:12.04

# Point to old-releases.ubuntu.com to get around the 404 errors
RUN sed -i 's|http://archive.ubuntu.com/ubuntu|http://old-releases.ubuntu.com/ubuntu|g' /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y build-essential g++ vim gdb gcc-multilib g++-multilib libc6-dev-i386 john && \
    apt-get clean

RUN pwunconv
COPY vuln*.cpp /

CMD [ "bash" ]

