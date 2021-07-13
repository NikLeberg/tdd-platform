FROM ubuntu:hirsute

ENV DEBIAN_FRONTEND=noninteractive
ENV LC_ALL=C

RUN apt-get -q -y update && apt-get -q -y install \
        git \
        gcc-8 \
        cmake \
        doxygen \
        graphviz \
        cppcheck \
        libcmocka-dev

# Extensions for ESP32 support
RUN apt-get -q -y update && apt-get -q -y install \
        libusb-1.0-0-dev \
        python3 \
        python3-pip \
    && update-alternatives --install \
        /usr/bin/python python /usr/bin/python3.9 1 \
    && update-alternatives --set \
        python /usr/bin/python3.9

ARG IDF_CLONE_URL=https://github.com/espressif/esp-idf.git
ARG IDF_CLONE_BRANCH=release/v4.3

RUN mkdir ~/.espressif \
    && cd ~/.espressif \
    && git clone $IDF_CLONE_URL -b $IDF_CLONE_BRANCH \
    && ./esp-idf/install.sh

RUN echo "source ~/.espressif/esp-idf/export.sh" >> ~/.bashrc
