FROM debian:sid-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    clang \
    clang-format \
    cmake \
    curl \
    gdb \
    git \
    less \
    libcurl4-openssl-dev \
    locales \
    openssl \
    pkg-config \
    python3-dev \
    python-is-python3 \
    ssh \
    sudo \
    tmux \
    wget \
    zsh \
 && rm -rf /var/lib/apt/lists/*

RUN echo "ALL ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/user \
 && chmod 0440 /etc/sudoers.d/user \
 ;

RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LC_ALL C

RUN apt-get update && apt-get install -y --no-install-recommends \
    nodejs \
    swi-prolog-nox \
    qtbase5-dev \
    libqt5websockets5-dev \
 && rm -rf /var/lib/apt/lists/*
