# Warning: CLONING requires a special command!

Cloning this repo is complicated by the existence of *submodules*:

```
git clone https://gitlab.com/trbot86/setbench.git --recurse-submodules
```

Note: if you check out a branch, you must run `git submodule update` to pull the correct versions of the submodules for that branch.

# For usage instructions visit the SetBench Wiki!
https://gitlab.com/trbot86/setbench/wikis/home

Also see: `setbench/microbench_experiments/example/instructions_data.ipynb`, a Jupyter notebook tutorial that we recommend opening in the free/open source IDE VSCode (after installing the VSCode Python (Microsoft) extension).

# Setting up SetBench on Ubuntu 20.04 or 18.04

Installing necessary build tools, libraries and python packages:
```
sudo apt install build-essential make g++ git time libnuma-dev numactl dos2unix parallel python3 python3-pip zip
pip3 install numpy matplotlib pandas seaborn ipython ipykernel jinja2 colorama
```

Installing LibPAPI (needed for per-operation cache-miss counts, cycle counts, etc.):
```
git clone https://bitbucket.org/icl/papi.git
cd papi/src
./configure
sudo sh -c "echo 2 > /proc/sys/kernel/perf_event_paranoid"
./configure
make -j
make test
sudo make install
sudo ldconfig
```

Clone and build SetBench:
```
git clone https://gitlab.com/trbot86/setbench.git --recurse-submodules
cd setbench/microbench
make -j<NUMBER_OF_CORES>
cd ../macrobench
./compile.sh
```

## Setting up SetBench on Ubuntu 16.04

Installing necessary build tools, libraries and python packages:
```
sudo apt update
sudo apt install build-essential make g++ git time libnuma-dev numactl dos2unix parallel python3 python3-pip zip
pip3 install --upgrade pip
pip3 install numpy matplotlib pandas seaborn ipython ipykernel jinja2 colorama
```

*The rest is the same as in Ubuntu 18.04+ (above).*

## Setting up SetBench on Fedora 32

Installing necessary build tools, libraries and python packages:
```
dnf update -y
dnf install -y @development-tools dos2unix gcc g++ git make numactl numactl-devel parallel python3 python3-pip time findutils hostname zip perf papi papi-static papi-libs
pip3 install numpy matplotlib pandas seaborn ipython ipykernel jinja2 colorama
```

*The rest is the same as in Ubuntu 18.04+ (above).*

## Other operating systems

- Debian: Should probably work, as it's very similar to Ubuntu... may need some minor tweaks to package names (can usually be resolved by googling "debian cannot find package xyz").

- Windows (WSL): Should work if you disable `USE_PAPI` in the Makefile(s), and eliminate any mention of `numactl`. Note that hardware counters won't work.

- FreeBSD: Could possibly make this work with a lot of tweaking.

- Mac / OSX: Suspect it should work with minor tweaking, but haven't tested.

## Other architectures

This benchmark is for Intel/AMD x86_64.

It likely needs new memory fence placements, or a port to use `atomics`, if it is to run correctly on ARM or POWER (except in x86 memory model emulation mode).

## Docker

Docker files, and pre-built containers for Ubuntu 20.04 and Fedora 32, are provided in `docker/`. You can pull the mainline Ubuntu 20.04 image with `docker pull trbot86/setbench`.

## Virtual machines

Note that you won't have access to hardware counters for tracking cache misses, etc., if you are using a virtual machine (except possibly in VMWare Player with PMU/PMC counter virtualization, although such results might not be 100% reliable).

However, we are working on preparing one or more VM images.
