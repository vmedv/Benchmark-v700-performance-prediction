import re 
from pathlib import Path
import argparse

DEFAULT_OUTPUT_FILE_NAME = "experiments-table"

PATTERN = r"avg_val=(\S*)"
COLUMN_SEP = " & "
ROUND = 2
ROW_SEP = "\n\\hline\n"

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
            for query_dir in workload_dir.iterdir():
                if query_dir.name == args.query:
                    for ds_dir in query_dir.iterdir():
                        ds = ds_dir.name
                        if not ds_dir.is_dir() or ds not in args.ds:
                            continue
                        for stat_file in ds_dir.iterdir():
                            if args.stat in stat_file.name:
                                content = stat_file.read_text()
                                all = re.findall(PATTERN, content)
                                workloads_by_ds[ds][workload] = float(all[2])

    # print(workloads_by_ds)

    table = []
    table.append("\\begin{tabular}{|" + " | ".join(["c" for _ in range(len(args.workloads) + 1)]) + "|}")
    table.append(COLUMN_SEP + COLUMN_SEP.join(args.workloads) + "\\\\")

    pivot_dict = workloads_by_ds[args.ds[0]]

    for i, ds in enumerate(args.ds):
        ds_dict = workloads_by_ds[ds]
        if i == 0:
            values = [f"${ds_dict[workload]}$" for workload in args.workloads]
        else:
            values = [f"x${round(ds_dict[workload] / pivot_dict[workload], ROUND)}$" for workload in args.workloads]
        table.append(ds + COLUMN_SEP + COLUMN_SEP.join(values) + " \\\\")
    
    table.append("\\end{tabular}")
    
    print(ROW_SEP.join(table))
