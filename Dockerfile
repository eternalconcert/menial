FROM ubuntu:latest
WORKDIR /data

RUN apt-get update && apt-get install -y python3 python3-dev

ADD build/menial.bin .
ADD resources/ ./resources/
ADD deployment/default/static/ /data/html/static

CMD ["/data/menial.bin", "/conf/menial.json"]
