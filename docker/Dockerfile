FROM ubuntu:20.04

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

RUN apt update && apt install -y \
        linux-tools-generic \
        linux-tools-common \
        && rm -rf /var/lib/apt/lists/* \
        && echo alias perf=$(find / -wholename "*-generic/perf") > ~/.bash_aliases

RUN pip3 install \
        numpy \
        matplotlib \
        pandas \
        seaborn \
        ipython \
        ipykernel \
        jinja2 \
        colorama

RUN cd root \
        && git clone https://bitbucket.org/icl/papi.git \
        && cd papi/src \
        && ./configure \
        && make -j \
        && make install \
        && ldconfig \
        && cd ../..

RUN apt update && apt install -y \
        nano \
        htop \
        && rm -rf /var/lib/apt/lists/*

COPY . /root/setbench/

WORKDIR /root/setbench

CMD ["/bin/bash"]
