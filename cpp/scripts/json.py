import argparse
import os
from pathlib import Path

PROJECT_NAME = "SETBENCH"
DEFAULT_OUTPUT_DIR_NAME = "json-parameters"
TEMPLATE_FORMAT = ".cpp"
OUT_FORMAT = ".out"


def to_define_name(name):
    return "_".join(name.upper().replace("/", "_").split())


def get_beginning(file_name):
    return (f"#ifndef {PROJECT_NAME}_{to_define_name(file_name)}_CPP\n"
            f"#define {PROJECT_NAME}_{to_define_name(file_name)}_CPP\n")


def get_ending(file_name):
    return f"#endif //{PROJECT_NAME}_{to_define_name(file_name)}_CPP\n"


def get_builders_from_impls(workloads_path, entity_name, builder_name):
    inclusion = ""
    paths = sorted(Path(workloads_path / entity_name / "impls").glob('*.h'))

    for impl_file_name in list(map(os.path.basename, paths)):
        impl_file = open(Path(workloads_path / entity_name / "impls" / impl_file_name), "r")
        lines = impl_file.readlines()
        impl_file.close()

        for i in range(len(lines)):
            if "struct" in lines[i] or "class" in lines[i]:
                declaration = lines[i]
                while "{" not in declaration:
                    i += 1
                    declaration += " " + lines[i]

                dec_begin = declaration.find("struct")
                if dec_begin == -1:
                    dec_begin = declaration.find("class")

                declaration = declaration[dec_begin:]

                dec_end = declaration.find("{")
                declaration = declaration[:dec_end]

                interfaces_begin = declaration.find(":")
                declaration = declaration[interfaces_begin:]

                delimiters = [":", " ", ",", "\n", "<", ">"]

                for delimiter in delimiters:
                    declaration = " ".join(declaration.split(delimiter))

                interfaces = declaration.split()
                if builder_name in interfaces:
                    inclusion += f"#include \"workloads/{entity_name}/impls/{impl_file_name}\"\n"
                    break

    return inclusion


def get_builders(workloads_path, entity_name, builder_name):
    inclusion = f"\n/* {builder_name}s */\n"
    paths = sorted(Path(workloads_path / entity_name / "builders").glob('*.h'))

    for builder_file in list(map(os.path.basename, paths)):
        inclusion += f"#include \"workloads/{entity_name}/builders/{builder_file}\"\n"

    # if the builder is in the impl file
    inclusion += get_builders_from_impls(workloads_path, entity_name, builder_name)

    return inclusion


def get_inclusion(args):
    inclusion = ("#include <fstream>\n\n"
                "#include \"json/single_include/nlohmann/json.hpp\"\n"
                "#include \"globals_extern.h\"\n\n")

    workloads_path = args.setbench_dir / "microbench" / "workloads"

    # all included builder are in convectors

    # inclusion += "\n/* StopConditions */\n"
    # paths = sorted(Path(workloads_path / "stop_condition" / "impls").glob('*.h'))
    #
    # for stop_condition in list(map(os.path.basename, paths)):
    #     inclusion += f"#include \"workloads/stop_condition/impls/{stop_condition}\"\n"
    #
    # inclusion += get_builders(workloads_path, "data_maps", "DataMapBuilder")
    # inclusion += get_builders(workloads_path, "distributions", "DistributionBuilder")
    # inclusion += get_builders_from_impls(workloads_path, "distributions", "MutableDistributionBuilder")
    # inclusion += get_builders(workloads_path, "args_generators", "ArgsGeneratorBuilder")
    # inclusion += get_builders(workloads_path, "thread_loops", "ThreadLoopBuilder")

    inclusion += "#include \"workloads/stop_condition/stop_condition_json_convector.h\"\n"
    inclusion += "#include \"workloads/data_maps/data_map_json_convector.h\"\n"
    inclusion += "#include \"workloads/distributions/distribution_json_convector.h\"\n"
    inclusion += "#include \"workloads/args_generators/args_generator_json_convector.h\"\n"
    inclusion += "#include \"workloads/thread_loops/thread_loop_json_convector.h\"\n"
    inclusion += "#include \"workloads/bench_parameters.h\"\n"

    return inclusion


def get_json_convector():
    return ("void convert(const BenchParameters &benchParameters, const std::string &fileName) {\n"
            "\tnlohmann::json json = benchParameters;\n"
            "\tstd::ofstream out(fileName);\n"
            "\tout << json.dump(4);\n"
            "}\n")


def get_main():
    return ("int main() {\n"
            "\t/**\n"
            "\t * The first step is the creation the BenchParameters class.\n"
            "\t */\n"
            "\tBenchParameters benchParameters;\n"
            "\n"
            "\t/**\n"
            "\t * Set the range of keys.\n"
            "\t */\n"
            "\tbenchParameters.setRange(2048);\n"
            "\n"
            "\t/**\n"
            "\t *Create the Parameters class for benchmarking (test).\n"
            "\t */\n"
            "\tParameters * test = new Parameters();\n"
            "\n"
            "\t/**\n"
            "\t * We will need to set the stop condition and workloads.\n"
            "\t */\n"
            "\tStopCondition * stopCondition = new Timer(10000);\n"
            "\n"
            "\t/**\n"
            "\t *Setup a workload.\n"
            "\t */\n"
            "\tArgsGeneratorBuilder * argsGeneratorBuilder =\n"
            "\t\t\t(new DefaultArgsGeneratorBuilder())\n"
            "\t\t\t\t\t->setDistributionBuilder(\n"
            "\t\t\t\t\t\t\t(new ZipfDistributionBuilder())\n"
            "\t\t\t\t\t\t\t\t\t->setAlpha(1.0)\n"
            "\t\t\t\t\t)\n"
            "\t\t\t\t\t->setDataMapBuilder(\n"
            "\t\t\t\t\t\t\tnew ArrayDataMapBuilder()\n"
            "\t\t\t\t\t);\n"
            "\n"
            "\tThreadLoopBuilder * threadLoopBuilder =\n"
            "\t\t\t(new DefaultThreadLoopBuilder())\n"
            "\t\t\t\t\t->setInsRatio(0.1)\n"
            "\t\t\t\t\t->setRemRatio(0.1)\n"
            "\t\t\t\t\t->setRqRatio(0)\n"
            "\t\t\t\t\t->setArgsGeneratorBuilder(argsGeneratorBuilder);\n"
            "\n"
            "\ttest->addThreadLoopBuilder(\n"
            "\t\t\t\t\tthreadLoopBuilder, 8,\n"
            "\t\t\t\t\tnew int[8]{-1, -1, 0, 0, 1, 2, 3, 3}\n"
            "\t\t\t)\n"
            "\t\t\t->setStopCondition(stopCondition);\n"
            "\n"
            "\tbenchParameters\n"
            "\t\t\t.setTest(test)\n"
            "\t\t\t.createDefaultPrefill();\n"
            "\n"
            "\tconvert(benchParameters, \"json_file.txt\");\n"
            "}\n")


def create_template_file(args):
    out_file = args.output_dir / f"{args.file_name}{TEMPLATE_FORMAT}"

    with open(out_file, "w") as out:
        out.write(get_beginning(args.file_name))
        out.write("\n")
        out.write(get_inclusion(args))
        out.write("\n")
        out.write(get_json_convector())
        out.write("\n")
        out.write(get_main())
        out.write("\n")
        out.write(get_ending(args.file_name))


# all:
# 	g++ -I../microbench -I../common -I../.. -I.. -pthread -ldl -mrtm json_parameters.cpp -o json_parameters.out
#
# run:
# 	./json_parameters.out


def create_make_file(args):
    out_file = args.output_dir / f"{args.makefile_name}"

    makefile = ("all:\n"
                f"\tg++ -I{args.setbench_dir}/microbench -I{args.setbench_dir}/common "
                f"-I{args.setbench_dir} -pthread -ldl -mrtm {args.file_name}{TEMPLATE_FORMAT}"
                f" -o {args.file_name}{OUT_FORMAT}\n"
                "\n"
                "run:\n"
                f"\t./{args.file_name}{OUT_FORMAT}")

    with open(out_file, "w") as out:
        out.write(makefile)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="""
        Script for creating an empty .cpp template and Make files to configure launch options.
    """)

    template_group = parser.add_argument_group("template args")

    template_group.add_argument("-fn", "--file-name", type=str, default="json_parameters",
                                help="The name of .cpp template file")
    template_group.add_argument("-mfn", "--makefile-name", type=str, default="Makefile",
                                help="The name of Makefile. "
                                     "To run non-default name use the ‘-f name’ or ‘--file=name’ arguments")
    template_group.add_argument("-o", "--output-dir", type=Path,
                                default=Path.cwd() / DEFAULT_OUTPUT_DIR_NAME,
                                help="Directory where results will be stored")
    template_group.add_argument("-s", "--setbench-dir", type=Path,
                                default=Path.cwd().parent, help="Directory where setbench is located")

    args = parser.parse_args()

    args.output_dir.mkdir(parents=True, exist_ok=True)

    create_template_file(args)

    create_make_file(args)
    # print_hi('PyCharm')
