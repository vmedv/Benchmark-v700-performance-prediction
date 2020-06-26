#!/usr/bin/python3

import inspect
import subprocess
import os
import sys
import pprint
import datetime
import argparse
import sqlite3

######################################################
#### parse command line args
######################################################

parser = argparse.ArgumentParser(description='Run a complete experimental suite and produce plots.')
group = parser.add_mutually_exclusive_group(required=True)
group.add_argument('--testing', dest='testing', action='store_true', help='enable testing mode (determines True/False value of variable <args.testing> that can be used to define experimental parameters differently to enable quick testing of a wide variety of cases, by, e.g., limiting the number of thread counts tested, reducing the time limit for experiments, setting the number of repeated trials to one, and disabling prefilling')
group.add_argument('--production', dest='testing', action='store_false', help='disable testing mode')
parser.add_argument('--no-compile', dest='compile', action='store_false', help='disable compilation (reuse old binaries)')
parser.set_defaults(compile=True)
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
def in_param(param_name):
    global type_failures
    def fn(value):
        # print('param_name={} value={}'.format(param_name, value))
        success = False
        try:
            intval = int(value)
            success = (intval in g['run_params'][param_name])
        except ValueError:
            type_failures.append('in_param tried to cast value "{}" to int and failed'.format(value))
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

def add_plots(filename, x_axis, y_axis, plot_type, series=None, groupby_list=[], filter="", title="", plot_cmd_args=""):
    g['plots'].append(locals())

###############################################################################
###############################################################################
#### BEGIN CONFIG
###############################################################################
###############################################################################

set_dir_compile ( os.getcwd() + '/../../microbench' )   ## working directory where the compilation command should be executed
set_dir_run     ( os.getcwd() + '/bin' )                ## working directory where your program should be executed
set_dir_data    ( os.getcwd() + '/data' )               ## directory for data files produced by individual runs of your program

##
## add parameters that you want your experiments to be run with.
## your program will be run once for each set of values in the CROSS PRODUCT of all parameters.
## (i.e., we will run your program with every combination of parameters)
##

add_run_param ( '_dist'           , ['uniform', 'zipf'] )
add_run_param ( '_insdel'         , ['0.0 0.0', '0.5 0.5', '20.0 10.0'] )
add_run_param ( '_keyrange'       , [2000000, 20000000] )
add_run_param ( '_alg'            , ['brown_ext_ist_lf', 'brown_ext_abtree_lf', 'bronson_pext_bst_occ'] )
add_run_param ( '_thread_pinning' , ['-pin ' + shell_to_str('cd ../../tools ; ./get_pinning_cluster.sh', exit_on_error=True)] )

if args.testing:
    add_run_param ( '_dist'           , ['uniform'] )   ########## TODO: remove
    add_run_param ( '_insdel'         , ['0.5 0.5'] )   ########## TODO: remove
    add_run_param ( '_keyrange'       , [2000000] )     ########## TODO: remove

    add_run_param ( '_trial'          , [1] )
    add_run_param ( '_nthreads'       , [1, shell_to_str('cd ../../tools ; ./get_thread_counts_max.sh', exit_on_error=True)] )
else:
    add_run_param ( '_trial'          , [1, 2, 3] )
    add_run_param ( '_nthreads'       , shell_to_list('cd ../../tools ; ./get_thread_counts_numa_nodes.sh', exit_on_error=True) )

##
## specify how to compile and run your program.
##
## you can use any of the parameters you defined above to dynamically replace {_tokens_like_this}.
## you can also get the directories saved above by using {__dir_compile} {__dir_run} {__dir_data}.
##
## the following replacement token is also defined for you:
##      {__step}            the number of runs done so far, padded to six digits with leading zeros
##
## your compile command will be executed in the compile directory above.
## your run command will be executed in the run directory above.
##

set_cmd_compile ( 'make -j all bin_dir={__dir_run}' )

if args.testing:
    set_cmd_run ( 'LD_PRELOAD=../../../lib/libjemalloc.so timeout 300 numactl --interleave=all time ./ubench_{_alg}.alloc_new.reclaim_debra.pool_none.out -nwork {_nthreads} -dist-{_dist} -insdel {_insdel} -k {_keyrange} -t 100 {_thread_pinning} -rq 0 -rqsize 1 -nrq 0' )
else:
    set_cmd_run ( 'LD_PRELOAD=../../../lib/libjemalloc.so timeout 300 numactl --interleave=all time ./ubench_{_alg}.alloc_new.reclaim_debra.pool_none.out -nwork {_nthreads} -nprefill {_nthreads} -dist-{_dist} -insdel {_insdel} -k {_keyrange} -t 5000 {_thread_pinning} -rq 0 -rqsize 1 -nrq 0' )

set_file_run_data ( 'data{__step}.txt' )                ## pattern for output filenames. note: filenames cannot contain spaces.

##
## add data fields to be fetched from all output files.
## each of these becomes a column in a table of data.
##
## by default, a field "XYZ" will be fetched from each data file using extractor grep_line,
##      which greps (searches) for a line of the form "XYZ=[arbitrary string]"
##
## if your field is not stored in that format, you can specify a custom "extractor" function,
##      as we do in our example below, to extract the max resident size
##      from the 6th space-separated column of the output of the linux "time" command
##
## also note: each of these fields becomes a replacement token, e.g., {PAPI_L3_TCM}.
##
## the following special fields are also defined for you:
##      {__step}            the number of runs done so far, padded to six digits with leading zeros
##      {__cmd_run}         your cmd_run string with any tokens replaced appropriately for this run
##      {__file_run_data}   the output filename for the current run's data
##      {__path_run_data}   the relative path to the output file for the current run's data
##      {__hostname}        the result of running the hostname command on the machine
##

## extractor functions are provided a file to load data from, and a field name
def get_maxres(file_name, field_name):
    ## manually parse the maximum resident size from the output of `time` and add it to the data file
    maxres_kb_str = shell_to_str('grep "maxres" {} | cut -d" " -f6 | cut -d"m" -f1'.format(file_name))
    return float(maxres_kb_str) / 1000

## note: in the following, defaults are "validator=is_nonempty" and "extractor=grep_line"

add_data_field ( 'distribution'           , coltype='TEXT' )
add_data_field ( 'INS_DEL_FRAC'           , coltype='TEXT'    , validator=in_param('_insdel') )
add_data_field ( 'MAXKEY'                 , coltype='INTEGER' , validator=in_param('_keyrange') )
add_data_field ( 'DS_TYPENAME'            , coltype='TEXT'    , validator=in_param('_alg') )
add_data_field ( 'TOTAL_THREADS'          , coltype='INTEGER' , validator=in_param('_nthreads') )
add_data_field ( 'total_throughput'       , coltype='INTEGER' , validator=is_positive )
add_data_field ( 'PAPI_L3_TCM'            , coltype='REAL' )
add_data_field ( 'PAPI_L2_TCM'            , coltype='REAL' )
add_data_field ( 'PAPI_TOT_CYC'           , coltype='REAL' )
add_data_field ( 'PAPI_TOT_INS'           , coltype='REAL' )
add_data_field ( 'maxresident_mb'         , coltype='REAL'    , validator=is_positive , extractor=get_maxres ) ## note the custom extractor
add_data_field ( 'tree_stats_height'      , coltype='INTEGER' , validator=is_positive )
add_data_field ( 'tree_stats_avgKeyDepth' , coltype='REAL'    , validator=is_positive )
add_data_field ( 'tree_stats_sizeInBytes' , coltype='INTEGER' , validator=is_positive )
add_data_field ( 'validate_result'        , coltype='TEXT'    , validator=is_equal('success') )
add_data_field ( 'MILLIS_TO_RUN'          , coltype='TEXT'    , validator=is_positive )
add_data_field ( 'RECLAIM'                , coltype='TEXT' )
add_data_field ( 'POOL'                   , coltype='TEXT' )
add_data_field ( 'ACTUAL_THREAD_BINDINGS' , coltype='TEXT' )
add_data_field ( '__hostname'             , coltype='TEXT' )
add_data_field ( '__cmd_run'              , coltype='TEXT' )
add_data_field ( '__file_run_data'        , coltype='TEXT' )
add_data_field ( '__path_run_data'        , coltype='TEXT' )

##
## plots with multiple series
##

add_plots ( filename='plot_throughput-u{INS_DEL_FRAC}-k{MAXKEY}.png'  , title='u={INS_DEL_FRAC} k={MAXKEY}: Throughput vs thread count'     , groupby_list=['INS_DEL_FRAC', 'MAXKEY'], series='DS_TYPENAME', x_axis='TOTAL_THREADS', y_axis='total_throughput', plot_type='bars' )
add_plots ( filename='plot_l2miss-u{INS_DEL_FRAC}-k{MAXKEY}.png'      , title='u={INS_DEL_FRAC} k={MAXKEY}: L2 misses/op vs thread count'   , groupby_list=['INS_DEL_FRAC', 'MAXKEY'], series='DS_TYPENAME', x_axis='TOTAL_THREADS', y_axis='PAPI_L2_TCM'     , plot_type='bars' )
add_plots ( filename='plot_l3miss-u{INS_DEL_FRAC}-k{MAXKEY}.png'      , title='u={INS_DEL_FRAC} k={MAXKEY}: L3 misses/op vs thread count'   , groupby_list=['INS_DEL_FRAC', 'MAXKEY'], series='DS_TYPENAME', x_axis='TOTAL_THREADS', y_axis='PAPI_L3_TCM'     , plot_type='bars' )
add_plots ( filename='plot_cycles-u{INS_DEL_FRAC}-k{MAXKEY}.png'      , title='u={INS_DEL_FRAC} k={MAXKEY}: Cycles/op vs thread count'      , groupby_list=['INS_DEL_FRAC', 'MAXKEY'], series='DS_TYPENAME', x_axis='TOTAL_THREADS', y_axis='PAPI_TOT_CYC'    , plot_type='bars' )
add_plots ( filename='plot_instructions-u{INS_DEL_FRAC}-k{MAXKEY}.png', title='u={INS_DEL_FRAC} k={MAXKEY}: Instructions/op vs thread count', groupby_list=['INS_DEL_FRAC', 'MAXKEY'], series='DS_TYPENAME', x_axis='TOTAL_THREADS', y_axis='PAPI_TOT_INS'    , plot_type='bars' )

##
## plots with only one series (no series field defined)
##

add_plots ( filename='plot_maxresident-u{INS_DEL_FRAC}-k{MAXKEY}.png', title='u={INS_DEL_FRAC} k={MAXKEY}: Max resident size (MB)', groupby_list=['INS_DEL_FRAC', 'MAXKEY'], x_axis='DS_TYPENAME', y_axis='maxresident_mb'        , plot_type='bars' )
add_plots ( filename='plot_avgkeydepth-u{INS_DEL_FRAC}-k{MAXKEY}.png', title='u={INS_DEL_FRAC} k={MAXKEY}: Average key depth'     , groupby_list=['INS_DEL_FRAC', 'MAXKEY'], x_axis='DS_TYPENAME', y_axis='tree_stats_avgKeyDepth', plot_type='bars' )

##
## example: a plot with data filtered to maximum thread count (only),
##          where the max thread count for this system is computed by a shell script
##

max_threads = shell_to_str('cd ../../tools ; ./get_thread_counts_max.sh', exit_on_error=True)
filter_string = 'TOTAL_THREADS == {}'.format(max_threads)

add_plots ( filename='plot_sizebytes-u{INS_DEL_FRAC}-k{MAXKEY}.png', title='u={INS_DEL_FRAC} k={MAXKEY}: Data structure size (bytes)', filter=filter_string, groupby_list=['INS_DEL_FRAC', 'MAXKEY'], x_axis='DS_TYPENAME', y_axis='tree_stats_sizeInBytes', plot_type='bars' )

##
## example: a plot with data filtered to single threaded runs
##

add_plots ( filename='plot_sequential-u{INS_DEL_FRAC}-k{MAXKEY}.png', title='u={INS_DEL_FRAC} k={MAXKEY}: Single threaded throughput', filter='TOTAL_THREADS == 1', groupby_list=['INS_DEL_FRAC', 'MAXKEY'], x_axis='DS_TYPENAME', y_axis='total_throughput', plot_type='bars' )

## TODO: how should the filtering and aggregation be done?

###############################################################################
###############################################################################
#### END CONFIG
###############################################################################
###############################################################################

tee('## Script running with the following parameters:')
pp_stdout.pprint(g)
pp_log.pprint(g)
tee()

tee()
tee('#########################################')
if args.testing:
    tee('## WARNING: RUNNING IN TESTING MODE')
else:
    tee('## NOTE: RUNNING IN PRODUCTION MODE')
tee('#########################################')
tee()

#########################################################################
#### Backup ONE set of old data and binaries (to avoid mistakes...)
#########################################################################

tee()
tee("## Moving old data to *.old")
tee()

tee_replace_and_run('rm -rf {__dir_data}.old 2>&1')
tee_replace_and_run('mv {__dir_data} {__dir_data}.old 2>&1')
tee_replace_and_run('mkdir {__dir_data}', exit_on_error=True)

#########################################################################
#### Compile into bin_dir
#########################################################################

if args.compile:
    tee()
    tee("## Moving old binaries to *.old")
    tee()

    tee_replace_and_run('rm -rf {__dir_run}.old 2>&1')
    tee_replace_and_run('mv {__dir_run} {__dir_run}.old 2>&1')
    tee_replace_and_run('mkdir {__dir_run}', exit_on_error=True)

    tee()
    tee(replace('## Compiling into {__dir_run}'))
    tee()

    log_replace_and_run('cd {__dir_compile} ; ' + g['cmd_compile'], exit_on_error=True)
    log()
else:
    tee()
    tee('## Skipping compilation because of cmdline arg...')
    tee()

#########################################################################
#### Run trials
#########################################################################

started = datetime.datetime.now()
tee('## Running trials (starting at {})'.format(started))

step = 0
maxstep = 1
for key in g['run_params']:
    maxstep = maxstep * len(g['run_params'][key])

# tee("maxstep={}".format(maxstep))



started_sec = int(shell_to_str('date +%s'))
# tee("started_sec={}".format(started_sec))

def print_time(sec):
    if sec == 0:
        return '00s'
    retval = ''
    if sec > 86400:
        retval = retval + '{}d'.format(str(sec//86400).zfill(2))
        sec = sec % 86400
    if sec > 3600:
        retval = retval + '{}h'.format(str(sec//3600).zfill(2))
        sec = sec % 3600
    if sec > 60:
        retval = retval + '{}m'.format(str(sec//60).zfill(2))
        sec = sec % 60
    if sec > 0:
        retval = retval + '{}s'.format(str(sec).zfill(2))
    return retval

# tee("print_time(started_sec)={}".format(print_time(started_sec)))



def run_param_loops(ix):
    global step
    global done

    if ix == len(g['run_params']):
        step += 1

        g['replacements']['__step'] = str(step).zfill(6)
        g['replacements']['__cmd_run'] = replace(g['cmd_run'])
        g['replacements']['__file_run_data'] = replace(g['file_run_data'])
        g['replacements']['__path_run_data'] = g['replacements']['__dir_data'] + '/' + g['replacements']['__file_run_data']

        # print('{}'.format(file_run_data_replaced))
        # print('{}'.format(cmd_run_replaced))
        log()
        pp_log.pprint(g['replacements'])

        cmd = g['replacements']['__cmd_run']
        fname = g['replacements']['__file_run_data']
        path = g['replacements']['__path_run_data']
        host = g['replacements']['__hostname']

        shell_to_str('echo "__step={}\n__cmd_run={}\n__file_run_data={}\n__path_run_data={}\n__hostname={}\n" > {}'.format(str(step).zfill(6), cmd, os.path.relpath(fname), os.path.relpath(path), host, path), exit_on_error=True)

        tee()
        tee('step {} / {}: {} -> {}'.format(str(step).zfill(6), str(maxstep).zfill(6), cmd, os.path.relpath(path)))

        ## actually run experimental command

        replace_and_run('cd {__dir_run} ; {__cmd_run} >> {__path_run_data} 2>&1', exit_on_error=True)

        ## remember the data file we just created
        g['data_file_paths'].append(g['replacements']['__path_run_data'])

        ## update progress info

        curr_sec = int(shell_to_str('date +%s'))
        elapsed_sec = curr_sec - started_sec
        frac_done = step / maxstep
        elapsed_sec_per_done = elapsed_sec / step
        remaining_sec = elapsed_sec_per_done * (maxstep - step)
        total_est_sec = elapsed_sec - remaining_sec

        tee('    (' + print_time(elapsed_sec) + ' elapsed, ' + print_time(remaining_sec) + ' est. remaining, est. total ' + print_time(total_est_sec) + ')')

    else:
        param_list = list(g['run_params'].keys())
        param = param_list[ix]
        for value in g['run_params'][param]:
            g['replacements'][param] = value
            run_param_loops(1+ix)

step = 0
run_param_loops(0)

#########################################################################
#### Create sqlite database
#########################################################################

db_path = 'database.sqlite'.format(g['replacements']['__dir_data'])
shell_to_str('rm -f {}'.format(db_path), exit_on_error=True)

con = sqlite3.connect(db_path)
cur = con.cursor()

txn = 'CREATE TABLE data (__id INTEGER PRIMARY KEY'
for key in g['data_fields'].keys():
    txn += ', ' + key + ' ' + g['data_fields'][key]['coltype']
txn += ')'

# print("\ntxn='{}'".format(txn))
cur.execute(txn)
con.commit()

#########################################################################
#### Extract data from output files and create sqlite database
#########################################################################

validation_errors = []

tee()

fields = ''
for field in g['data_fields'].keys():
    fields += ('' if fields == '' else ', ') + field

# validator = is_equal('hello')
# print('{} {}'.format(validator('hello'), validator('not hello')))

# validator = in_param('_dist')
# print('{} {}'.format(validator('uniform'), validator('not uniform')))

# v = g['data_fields']['distribution']['validator']
# print('{} {}'.format(v('uniform'), v('not uniform')))

# g['data_fields']['distribution']['validator'] = validator
# v = g['data_fields']['distribution']['validator']
# print('{} {}'.format(v('uniform'), v('not uniform')))

for data_file_path in g['data_file_paths']:
    short_fname = os.path.relpath(data_file_path)
    tee()
    tee('processing {}'.format(short_fname))
    txn = 'INSERT INTO data ({}) VALUES ({})'

    values = ''
    for field in g['data_fields'].keys():
        extractor = g['data_fields'][field]['extractor']
        validator = g['data_fields'][field]['validator']

        value = extractor(data_file_path, field)
        if not validator(value): validation_errors.append(short_fname + ': ' + field + '=' + value)
        tee('    extractor for field={} gets value={} valid={}'.format(field, value, validator(value)))

        if "'" in str(value) or '"' in str(value):
            tee("\n\nERROR: value {} contains single or double quotes! this will break sql insertion statements! cannot proceed. exiting...\n".format(value))
            exit(1)

        quote = ('"' if (g['data_fields'][field]['coltype'] == 'TEXT') else '')
        values += '{}{}{}{}'.format(('' if values == '' else ', '), quote, value, quote)

    txn = txn.format(fields, values)
    tee()
    tee(txn)
    cur.execute(txn)

con.commit()

#########################################################################
#### Produce plots
#########################################################################



#########################################################################
#### Finish
#########################################################################

ended = datetime.datetime.now()
ended_sec = int(shell_to_str('date +%s'))
elapsed_sec = ended_sec - started_sec

tee()
tee('## Finish at {} (started {}); total {}'.format(ended, started, print_time(elapsed_sec)))

if len(validation_errors) > 0:
    tee()
    tee('################################################')
    tee('#### WARNING: there were data validation errors!')
    tee('################################################')
    tee()
    for item in validation_errors:
        tee('    ' + item)
    exit(1)

# echo | tee -a $flog
# echo "## Zipping results into ./${host}_data.zip" | tee -a $flog
# echo | tee -a $flog
# zip -r ./${host}_data.zip *.sh ${compile_dir}/Makefile ${data_dir} >> $flog 2>&1

print("Note: log can be viewed at ./log.txt")
