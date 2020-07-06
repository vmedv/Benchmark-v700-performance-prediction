# For information, installation and usage, visit the SetBench Wiki!
https://gitlab.com/trbot86/setbench/wikis/home

Note that the git clone command is complicated by the existence of *submodules* `common/recordmgr` and `tools`:

`git clone git@gitlab.com:trbot86/setbench.git --recurse-submodules`

See the wiki for more instructions.

## Prerequisites on a clean install of Ubuntu 20.04

`sudo apt install build-essential make g++ git libnuma-dev numactl python3 python3-pip`

`pip3 install numpy matplotlib pandas seaborn`

LibPAPI:
    - `git clone https://bitbucket.org/icl/papi.git`
    - `cd papi/src`
    - `./configure`
    - `sudo sh -c "echo 2 > /proc/sys/kernel/perf_event_paranoid"`
    - `./configure`
    - `make`
    - `make test`
    - `sudo make install`
    - `sudo ldconfig`

