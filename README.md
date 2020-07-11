# Warning: CLONING requires a special command!

Cloning this repo is complicated by the existence of *submodules*:

```
git clone https://gitlab.com/trbot86/setbench.git --recurse-submodules
```

Note: if you check out a branch, you must run `git submodule update` to pull the correct versions of the submodules for that branch.

# Setting up SetBench on Ubuntu 20.04 or 18.04

Installing necessary build tools, libraries and python packages:
```
sudo apt install build-essential make g++ git libnuma-dev numactl dos2unix parallel python3 python3-pip
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
sudo apt install build-essential make g++ git libnuma-dev numactl dos2unix parallel python3 python3-pip
pip3 install --upgrade pip
pip3 install numpy matplotlib pandas seaborn ipython ipykernel jinja2 colorama
```

*The rest is the same as in Ubuntu 18.04+ (above).*

# For usage instructions visit the SetBench Wiki!
https://gitlab.com/trbot86/setbench/wikis/home

Also see: `setbench/microbench_experiments/example/instructions_data.ipynb`, a Jupyter notebook tutorial that we recommend opening in the free/open source IDE VSCode (after installing the VSCode Python (Microsoft) extension).
