# Build image for QLog on Debian 13 (trixie): Qt 6.8, Hamlib 4.6.2.
#
#   docker build -f packaging/docker/Dockerfile -t qlog-build:trixie packaging
#   docker run --rm --user $(id -u):$(id -g) -e HOME=/tmp -v $PWD:/src -w /src/build \
#       qlog-build:trixie bash -c 'qmake6 ../QLog.pro CONFIG+=release && make -j$(nproc)'
FROM debian:trixie
ENV DEBIAN_FRONTEND=noninteractive
COPY appimage/packages.txt /tmp/packages.txt
RUN apt-get update \
    && xargs -a /tmp/packages.txt apt-get install -y --no-install-recommends \
    && apt-get install -y --no-install-recommends libhamlib-dev qtkeychain-qt6-dev qt6-base-dev-tools libqt6sql6-sqlite git \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
