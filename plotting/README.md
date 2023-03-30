# PLOTTING

Use [plotter.py](plotting/plotter.py) to benchmark and then plot results.

Before launching the script - install required dependecies via command:

```shell
pip install --upgrade -r requirements.txt
```

To get more info about laucnhing [plotter.py](plotting/plotter.py) script do the following:

```shell
python plotter.py --help
```

Example:

```shell
python3 plotter.py \
--stat total-find total-ops total-key-depth total-key-search \
--nprocess 2 \
--ds bt_8 sabt_8 ist_50 saist_50 \
--workload dist-uniform "skewed-sets -rs 0.3 -rp 0.7 -ws 0.3 -wp 0.7 -inter 0.3" "skewed-sets -rs 0.2 -rp 0.8 -ws 0.2 -wp 0.8 -inter 0.2" "skewed-sets -rs 0.1 -rp 0.9 -ws 0.1 -wp 0.9 -inter 0.1" "skewed-sets -rs 0.05 -rp 0.95 -ws 0.05 -wp 0.95 -inter 0.05" "skewed-sets -rs 0.01 -rp 0.99 -ws 0.01 -wp 0.99 -inter 0.01" \
--workload-name uniform 70-30 80-20 90-10 95-05 99-01 \
--insdel 0.0/0.0 0.3/0.3 \
--key 10000 100000 1000000 8000000 \
--prefill-size 5000 50000 500000 4000000 \
--time 10000
```

## Troubleshooting

If some errors occur while launching because of OS, try this:

```shell
sudo sysctl kernel.perf_event_paranoid=1
```
