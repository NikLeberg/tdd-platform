FROM ubuntu:hirsute

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -q -y update && apt-get -q -y install \
    git \
    gcc-8 \
    cmake \
    doxygen \
    graphviz \
    cppcheck \
    libcmocka-dev
