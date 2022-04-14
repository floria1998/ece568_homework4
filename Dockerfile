FROM ubuntu:20.04
ENV TZ=Etc/UTC
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone && apt-get update && apt-get install make
RUN mkdir /code
ADD . /code/
WORKDIR /code
RUN ls -a && apt-get update && apt-get -y install libboost-all-dev build-essential libpqxx-dev postgresql && make
