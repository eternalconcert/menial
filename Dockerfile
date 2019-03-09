FROM alpine:latest
WORKDIR /data

ADD build/menial.bin .
ADD deployment/default/static/ /data/html/static

CMD ["/data/menial.bin", "/conf/menial.json"]
