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
        time

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

## development support
RUN apt update && DEBIAN_FRONTEND="noninteractive" TZ="America/New_York" apt install -y \
        openssh-server \
        htop \
        vim \
        nano \
        && rm -rf /var/lib/apt/lists/*

COPY . /root/setbench/
WORKDIR /root/setbench

## allow INSECURE ssh to container as root (this is not a "real" service, so this is fine...)
## see: https://docs.docker.com/engine/examples/running_ssh_service/
RUN mkdir /var/run/sshd
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile
RUN bash -c "echo -e 'PermitRootLogin yes\nPasswordAuthentication yes\nPermitEmptyPasswords yes' >> /etc/ssh/sshd_config"
## note VERY INSECURE password "root"
RUN echo "root:root" | chpasswd
# EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
