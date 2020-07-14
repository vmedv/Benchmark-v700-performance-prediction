FROM ubuntu:18.04

RUN apt update && apt install -y \
        build-essential \
        dos2unix \
        g++ \
        git \
        libnuma-dev \
        make \
        numactl \
        parallel \
        python3 \
        python3-pip \
        time \
        && rm -rf /var/lib/apt/lists/*

RUN pip3 install \
        numpy \
        matplotlib \
        pandas \
        seaborn \
        ipython \
        ipykernel \
        jinja2 \
        colorama

RUN git clone https://bitbucket.org/icl/papi.git \
        && cd papi/src \
        && sh -c 'echo 2 > /proc/sys/kernel/perf_event_paranoid' \
        && ./configure \
        && make -j \
        && make install \
        && ldconfig \
        && make test \
        && cd ../..

RUN git clone https://gitlab.com/trbot86/setbench.git --recurse-submodules \
        && cd setbench \
        && git checkout data_framework \
        && git submodule update
