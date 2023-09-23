import re 
from pathlib import Path
import argparse
from statistics import variance 

DEFAULT_OUTPUT_FILE_NAME = "experiments-table"

PATTERN_AVG_VAL = r"avg_val=(\S*)"
PATTERN_VALUES = r"values=\[(.*?)\]"
COLUMN_SEP = " & "
ROUND = 2
ROW_SEP = "\n\\hline\n"
PERCENT = 0.95
COLOR = "blue!20"

INDEX = 2

BASE_DS = {"btree", "ist"}

DS_NAME_MAP = {
    "splay_tree": "Splay Tree",
    "sast": "SA2T",
    "btree": "B-Tree",
    "sabt": "SABT",
    "ist": "IST",
    "sait": "SAIT",
    "salt": "SALT"
}

WORKLOAD_NAME_MAP = {
    "70-30": "70/30",
    "80-20": "80/20",
    "90-10": "90/10",
    "95-05": "95/05",
    "99-01": "99/01"
}

def get_workload_name(workload):
    return WORKLOAD_NAME_MAP[workload] if workload in WORKLOAD_NAME_MAP else workload


def format_main_ops(ops):
    sops = '%.2g' % ops
    pos = sops.find("e+0")
    if pos == -1:
        raise RuntimeError("can not find e+0 in " + sops)
    return sops[:pos] + " \cdot {10}^{" + sops[pos+3:pos+4] + "}"


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--benchmark-dirs", nargs="+", type=Path, required=True)
    parser.add_argument("-q", "--query", type=str, required=True)
    parser.add_argument("-s", "--stat", type=str, required=True)
    parser.add_argument("-ds", nargs="+", type=str, required=True)
    parser.add_argument("-w", "--workloads", nargs="+", type=str, required=True)

    args = parser.parse_args()

    workloads_by_ds = {ds: {} for ds in args.ds}

    for benchmark_dir in args.benchmark_dirs:
        for workload_dir in benchmark_dir.iterdir():
            workload = workload_dir.name
            if workload not in args.workloads:
                continue
            # print("workload=", workload)
            for query_dir in workload_dir.iterdir():
                if query_dir.name == args.query:
                    for ds_dir in query_dir.iterdir():
                        ds = ds_dir.name
                        if not ds_dir.is_dir() or ds not in args.ds:
                            continue
                        for stat_file in ds_dir.iterdir():
                            if args.stat in stat_file.name:
                                content = stat_file.read_text()
                                all = re.findall(PATTERN_AVG_VAL, content)
                                # values = list(map(float, all[INDEX].split(", ")))
                                # print("ds=", ds, "\tvar=", variance(values))
                                workloads_by_ds[ds][workload] = float(all[INDEX])

    # print(workloads_by_ds)

    values_by_ds = {}
    base_best_dict = {}

    for i, ds in enumerate(args.ds):
        ds_dict = workloads_by_ds[ds]
        values = []
        for workload in args.workloads:
            value = ds_dict[workload]
            if ds in BASE_DS:
                if workload in base_best_dict:
                    base_best_dict[workload] = max(base_best_dict[workload], value)
                else:
                    base_best_dict[workload] = value
            values.append(value)
        values_by_ds[ds] = values

    table = []
    table.append("\\begin{tabular}{|" + " | ".join(["c" for _ in range(len(args.workloads) + 1)]) + "|}")
    table.append(COLUMN_SEP + COLUMN_SEP.join(map(get_workload_name, args.workloads)) + "\\\\")

    for i, ds in enumerate(args.ds):
        ds_dict = workloads_by_ds[ds]
        values = values_by_ds[ds]
        if i == 0:
            pivot_dict = ds_dict
            row = [f"${format_main_ops(ds_dict[workload])}$" for workload in args.workloads]
        else:
            row = []
            for workload, value in zip(args.workloads, values):
                percent = round(ds_dict[workload] / pivot_dict[workload], ROUND)
                cell = f"x${percent}$"
                if ds not in BASE_DS and value >= PERCENT * base_best_dict[workload]:
                    cell = "\\cellcolor{" + COLOR + "}" + cell
                row.append(cell)
        table.append(DS_NAME_MAP[ds] + COLUMN_SEP + COLUMN_SEP.join(row) + " \\\\")
    
    table.append("\\end{tabular}")
    
    print(ROW_SEP.join(table))
