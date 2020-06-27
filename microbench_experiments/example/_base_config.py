#!/usr/bin/python3

import inspect
import subprocess
import os
import sys
import pprint
import datetime
import argparse
import sqlite3
import numpy
import io

######################################################
#### parse command line args
######################################################

parser = argparse.ArgumentParser(description='Run a complete experimental suite and produce plots.')
group = parser.add_mutually_exclusive_group(required=True)
group.add_argument('--testing', dest='testing', action='store_true', help='enable testing mode (determines True/False value of variable <args.testing> that can be used to define experimental parameters differently to enable quick testing of a wide variety of cases, by, e.g., limiting the number of thread counts tested, reducing the time limit for experiments, setting the number of repeated trials to one, and disabling prefilling')
group.add_argument('--production', dest='testing', action='store_false', help='disable testing mode')
parser.add_argument('--no-run', dest='run', action='store_false', help='disable running actual experiments (reuse old data files)')
parser.set_defaults(run=True)
parser.add_argument('--no-compile', dest='compile', action='store_false', help='disable compilation (reuse old binaries)')
parser.set_defaults(compile=True)
parser.add_argument('--no-createdb', dest='createdb', action='store_false', help='disable recreation of sqlite database (reuse old database)')
parser.set_defaults(createdb=True)
parser.add_argument('--no-plot', dest='plot', action='store_false', help='disable creation of plots')
parser.set_defaults(plot=True)
parser.add_argument('--debug-no-plotcmd', dest='debug_exec_plotcmd', action='store_false', help='debug: do not exec the plot cmd')
parser.set_defaults(debug_exec_plotcmd=True)
parser.add_argument('--continue-on-warn-agg-row-count', dest='continue_on_warn_agg_row_count', action='store_true', help='allow execution to continue despite any warnings about aggregating a possibly incorrect number of rows into some data point(s) in a plot')
parser.set_defaults(continue_on_warn_agg_row_count=False)
args = parser.parse_args()
if len(sys.argv) < 2:
    parser.print_help()
    exit(1)

######################################################
#### data structures
######################################################

g = dict()
g['run_params'] = dict()
g['data_fields'] = dict()
g['replacements'] = dict()
g['data_file_paths'] = []
g['plots'] = []
g['log'] = open('log.txt', 'w')
g['sanity_check_failures'] = []

pp_log = pprint.PrettyPrinter(indent=4, stream=g['log'])
pp_stdout = pprint.PrettyPrinter(indent=4)

######################################################
#### private helper functions
######################################################

def shell_to_str(cmd, exit_on_error=False):
    child = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    returncode = child.returncode
    if returncode != 0:
        if child.stderr != None:
            ret_stderr = child.stderr.decode('utf-8').rstrip(' \r\n')
            tee()
            tee('########################################################')
            tee('#### STDERR:')
            tee('########################################################')
            tee()
            tee(ret_stderr)
            tee()
        tee('ERROR: running "{}" ; returncode {}'.format(cmd, returncode))
        if exit_on_error: exit(1)
    ret_stdout = child.stdout.decode('utf-8').rstrip(' \r\n')
    return ret_stdout

def shell_to_list(cmd, sep=' ', exit_on_error=False):
    return shell_to_str(cmd, exit_on_error).split(sep)

def replace(str):
    return str.format(**g['replacements'])

def replace_and_run(str, exit_on_error=False):
    return shell_to_str(replace(str), exit_on_error)

def log(str='', _end='\n'):
    print('{}'.format(str), file=g['log'], end=_end)

def tee(str='', _end='\n'):
    print('{}'.format(str), end=_end)
    print('{}'.format(str), file=g['log'], end=_end)

def log_replace_and_run(str, exit_on_error=False):
    log(replace_and_run(str, exit_on_error), _end='')

def tee_replace_and_run(str, exit_on_error=False):
    tee(replace_and_run(str, exit_on_error), _end='')

g['replacements']['__hostname'] = shell_to_str('hostname')

######################################################
#### validator functions
######################################################

def is_nonempty(value):
    return str(value) != '' and str(value) != 'None' and value != None

def is_positive(value, eps=1e-6):
    return float(value) > eps

## note: this is not a validator per se, but rather RETURNS a validator (obtained via currying an argument)
def is_equal(to_this_value):
    def fn(value):
        return value == to_this_value
    return fn

## note: this is not a validator per se, but rather RETURNS a validator (obtained via currying an argument)
type_failures = []
def is_run_param(param_name):
    global type_failures
    def fn(value):
        # print('param_name={} value={}'.format(param_name, value))
        success = False
        try:
            intval = int(value)
            success = (intval in g['run_params'][param_name])
        except ValueError:
            type_failures.append('is_run_param tried to cast value "{}" to int and failed'.format(value))
        if success: return True
        return (value in g['run_params'][param_name]) or (str(value) in g['run_params'][param_name])
    return fn

######################################################
#### extractor functions
######################################################

data = dict() ## data[filename] = dictionary capturing all lines of form XYZ=[...] in filename
def grep_line(file_name, field_name):   ## memoize files into data to make grepping efficient
    if file_name not in data.keys():
        data_list = shell_to_list('grep -E "^[^ =]+=" {}'.format(file_name), sep='\n', exit_on_error=True)
        # print("data_list=" + repr(data_list))
        newdict = dict()
        for item in data_list:
            # print('splitting "{}"'.format(item))
            tokens = item.split('=')
            # if len(tokens) == 2:
            #     newdict[tokens[0]] = tokens[1]
            # else:
            #     newdict[tokens[0]] = '='.join(tokens[1:])
            newdict[tokens[0]] = '='.join(tokens[1:])
            # print('     into "{}" and "{}"'.format(tokens[0], '='.join(tokens[1:])))
        data[file_name] = newdict

    return data[file_name][field_name]

def run_param(file_name, field_name):
    tee('run_param({}, {}) returns {}'.format(file_name, field_name, g['replacements'][field_name]))
    return g['replacements'][field_name]

######################################################
#### user facing functions
######################################################

def set_dir_compile(str):
    g['replacements']['__dir_compile'] = str

def set_dir_run(str):
    g['replacements']['__dir_run'] = str

def set_dir_data(str):
    g['replacements']['__dir_data'] = str

def set_cmd_compile(str):
    g['cmd_compile'] = str

def set_cmd_run(str):
    g['cmd_run'] = str

def set_file_run_data(str):
    g['file_run_data'] = str

def get_dir_compile():
    return g['replacements']['__dir_compile']

def get_dir_run():
    return g['replacements']['__dir_run']

def get_dir_data():
    return g['replacements']['__dir_data']

def add_run_param(name, value_list):
    # print('{}('{}', {})'.format(inspect.currentframe().f_code.co_name, name, value_list))
    g['run_params'][name] = value_list

def add_data_field(name, coltype='TEXT', validator=is_nonempty, extractor=grep_line):
    g['data_fields'][name] = locals()

def add_plot_set(filename, x_axis, y_axis, plot_type, series='', varying_cols_list=[], filter="", title="", plot_cmd_args=""):
    g['plots'].append(locals())
