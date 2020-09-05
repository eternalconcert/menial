FROM ubuntu:latest
WORKDIR /data

ADD website/build/ ./website/
ADD website/pyresponse/ ./pyresponse/
ADD src/python ./pyresponse/
ADD build/menial.bin ./website/static/
COPY build/menial_*.deb ./website/static/
COPY menial.tar.gz ./website/static/

COPY build/*.deb .
RUN apt-get update && apt-get install -y python3 python3-dev
RUN apt-get install ./menial*.deb
RUN rm menial*.deb


CMD ["menial"]
