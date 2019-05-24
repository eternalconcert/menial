FROM ubuntu:latest
WORKDIR /data

RUN apt-get update && apt-get install -y python3 python3-dev

ADD build/menial.bin .
ADD resources/ ./resources/
ADD website/build/ ./website/
ADD website/pyresponse/ ./pyresponse/
ADD src/python ./pyresponse/
ADD build/menial.bin ./website/static/
COPY menial.tar.gz ./website/static/

CMD ["/data/menial.bin", "/conf/menial.json"]
