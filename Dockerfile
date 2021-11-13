# Step 1 : Get Qt libs from an armhf raspbian environment
FROM --platform=linux/arm/v7 balenalib/rpi-raspbian:buster as rpi-builder
RUN apt-get update && \
    apt-get install -y \
    libc6-dev \
    zlib1g-dev \
    qt5-default \
    qtbase5-dev

# Step 2 : Create an amd64 Arch Linux build environment (for efficiency)
FROM --platform=linux/amd64 debian:buster

ENV RPI_SYSROOT /rpi-sysroot
ENV OUTPUT_DIR  /output

# Gather the armhf Qt libs
RUN mkdir -p  ${RPI_SYSROOT}/usr ${RPI_SYSROOT}/lib
COPY --from=rpi-builder /lib ${RPI_SYSROOT}/lib
COPY --from=rpi-builder /usr/include ${RPI_SYSROOT}/usr/include
COPY --from=rpi-builder /usr/lib ${RPI_SYSROOT}/usr/lib
COPY --from=rpi-builder /usr/bin ${RPI_SYSROOT}/usr/bin

# Install the toolchains
RUN apt-get update && \
    apt-get install -y \
        cmake \
        gcc-arm-linux-gnueabihf \
        g++-arm-linux-gnueabihf \
        qtbase5-dev \
        qt5-default

ENV QEMU_LD_PREFIX /usr/arm-linux-gnueabihf
#ENV LD_LIBRARY_PATH ${RPI_SYSROOT}/lib/arm-linux-gnueabihf

ADD . /workdir
WORKDIR /workdir
RUN chmod a+x /workdir/docker/build-all.sh

ENTRYPOINT [ "/workdir/docker/build-all.sh" ]