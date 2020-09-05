FROM ubuntu:latest
WORKDIR /data

ADD build/*.deb .
RUN apt-get update && apt-get install -y python3 python3-dev
RUN apt-get install ./menial*.deb


CMD ["menial"]
