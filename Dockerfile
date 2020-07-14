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

RUN cd root \
        && git clone https://bitbucket.org/icl/papi.git \
        && cd papi/src \
        && ./configure \
        && make -j \
        && make install \
        && ldconfig \
        && cd ../..

RUN cd root \
        && echo
        && echo '###############################################'
        && echo '## CLONE SETBENCH: branch data_framework'
        && echo '###############################################'
        && echo
        && git clone https://gitlab.com/trbot86/setbench.git --recurse-submodules \
        && cd setbench \
        && git checkout data_framework \
        && git submodule update
        && echo
        && echo '###############################################'
        && echo '## COMPILE & TEST: microbench'
        && echo '###############################################'
        && echo
        && cd microbench_experiments/example \
        && ./run_testing.sh \
        && rm -r bin data \
        && echo
        && echo '###############################################'
        && echo '## COMPILE & TEST: macrobench'
        && echo '###############################################'
        && echo
        && cd ../../macrobench_experiments/istree_exp1 \
        && ./run_testing.sh \
        && rm -r ../../macrobench/bin data
