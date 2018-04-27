FROM debian:stretch

ADD server /server/

RUN apt-get update
RUN apt-get install libstdc++6

CMD ["server/menial", "server/menial.json"]
