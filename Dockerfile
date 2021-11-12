FROM --platform=linux/arm/v7 balenalib/rpi-raspbian:buster as rpi-builder

RUN apt-get update && apt-get install -y qtbase5-dev

FROM --platform=linux/amd64 archlinux:latest

ADD . /workdir

RUN chmod a+x /workdir/docker/setup_arch.sh && /workdir/docker/setup_arch.sh