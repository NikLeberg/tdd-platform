FROM ubuntu:hirsute

ENV DEBIAN_FRONTEND=noninteractive
ENV LC_ALL=C

RUN apt-get -q -y update && apt-get -q -y install --no-install-recommends \
        git=1:2.30* \
        libc6-dev=2.33* \
        gcc=4:10.3* \
        make=4.3* \
        cmake=3.18* \
        ccache=4.2* \
        doxygen=1.9* \
        graphviz=2.42* \
        cppcheck=2.3* \
    && rm -rf /var/lib/apt/lists/*

# Install packages for ESP8266 & ESP32 support
RUN apt-get -q -y update && apt-get -q -y install --no-install-recommends \
        libusb-1.0-0-dev=2:1.0* \
        jq=1.6* \
        python3=3.9* \
        python3-pip=20.3* \
    && rm -rf /var/lib/apt/lists/* \
    && update-alternatives --install \
        /usr/bin/python python /usr/bin/python3.9 1 \
    && update-alternatives --set \
        python /usr/bin/python3.9

# Install esp-idf for ESP32 support
ARG ESP32_CLONE_URL=https://github.com/espressif/esp-idf.git
ARG ESP32_CLONE_TAG=v4.3

RUN mkdir -p ~/.espressif \
    && cd ~/.espressif \
    && git clone $ESP32_CLONE_URL -b $ESP32_CLONE_TAG --depth 1 \
    && cd ./esp-idf \
    && jq '.tools[] .install = "never"' \
        ./tools/tools.json > ./tools/tools_never.json \
    && jq '(.tools[] | select(.name == "xtensa-esp32-elf" or .name == "esp32ulp-elf" or .name == "openocd-esp32")).install = "always"' \
        ./tools/tools_never.json > ./tools/tools.json \
    && rm ./tools/tools_never.json \
    && ./install.sh

# Install sdk for ESP8266 support
ARG ESP8266_CLONE_URL=https://github.com/espressif/ESP8266_RTOS_SDK.git
ARG ESP8266_CLONE_TAG=v3.4

RUN mkdir -p ~/.espressif \
    && cd ~/.espressif \
    && git clone $ESP8266_CLONE_URL -b $ESP8266_CLONE_TAG --depth 1 \
    && ./ESP8266_RTOS_SDK/install.sh
