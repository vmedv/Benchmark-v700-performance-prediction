#!/usr/bin/python3
from _basic_functions import *
import numpy as np

def define_experiment(exp_dict, args):
    set_dir_run         (exp_dict, os.getcwd() + '/../../macrobench')
    set_dir_tools       (exp_dict, os.getcwd() + '/../../tools' )
    set_dir_compile     (exp_dict, os.getcwd() + '/../../macrobench')
    set_cmd_compile     (exp_dict, './compile.sh')

    binaries_rel_list   = [ \
        'bin/rundb_YCSB_bronson_pext_bst_occ', \
        'bin/rundb_YCSB_brown_ext_abtree_lf', \
        'bin/rundb_YCSB_brown_ext_ist_lf' \
    ]

    thread_count_list   = shell_to_list('cd ' + get_dir_tools(exp_dict) + ' ; ./get_thread_counts_numa_nodes.sh', exit_on_error=True)
    max_thread_count    = int(shell_to_str('cd ' + get_dir_tools(exp_dict) + ' ; ./get_thread_counts_max.sh', exit_on_error=True))
    thread_pinning_str  = '-pin 0-{}'.format(max_thread_count-1)

    ## compute a suitable initial database size that will work FOR ALL thread counts.
    ## this is needed to work around a *peculiarity* of DBx1000:
    ## the size must be divisible by every thread count you run!
    init_size = compute_suitable_initsize(thread_count_list, 100000000)

    add_run_param       (exp_dict, '__trials'       , [1, 2, 3])
    add_run_param       (exp_dict, 'binary'         , binaries_rel_list)
    add_run_param       (exp_dict, 'zeta'           , [0.01, 0.1, 0.5, 0.9])
    add_run_param       (exp_dict, 'rw_ratio'       , ['-r0.9 -w0.1'])
    add_run_param       (exp_dict, 'init_size'      , [init_size])
    add_run_param       (exp_dict, 'thread_count'   , thread_count_list)
    add_run_param       (exp_dict, 'thread_pinning' , [thread_pinning_str])

    ## i like to have a testing mode (enabled with argument --testing) that runs for less time,
    ##  with fewer parameters (to make sure nothing will blow up before i run for hours...)
    if args.testing:
        add_run_param   (exp_dict, '__trials'       , [1])
        add_run_param   (exp_dict, 'zeta'           , [0.1])
        add_run_param   (exp_dict, 'thread_count'   , [max_thread_count])
        ## smaller initial size for testing
        init_size = compute_suitable_initsize([max_thread_count], 10000000)
        add_run_param   (exp_dict, 'init_size'      , [init_size])

    set_cmd_run(exp_dict, ''' \
            ASAN_OPTIONS=new_delete_type_mismatch=0 \
            LD_PRELOAD=../lib/libjemalloc.so \
            numactl --interleave=all \
            time ./{binary} -t{thread_count} -s{init_size} {rw_ratio} -z{zeta} \
            {thread_pinning}
    '''.replace('    ', ''))

    add_data_field      (exp_dict, 'alg'            , extractor=extract_alg_from_binary_name)
    add_data_field      (exp_dict, 'throughput'     , coltype='REAL', extractor=get_extractor_for_summary_subfield_numeric(32), validator=is_positive)
    add_data_field      (exp_dict, 'runtime'        , coltype='REAL', extractor=get_extractor_for_summary_subfield_numeric(4), validator=is_positive)
    add_data_field      (exp_dict, 'maxresident_mb' , coltype='REAL', extractor=extract_maxres, validator=is_positive)

    add_plot_set( \
            exp_dict \
          , name='throughput-z{zeta}.png' \
          , title='z={zeta}: tx/sec vs thread count' \
          , varying_cols_list=['zeta'] \
          , series='alg' \
          , x_axis='thread_count' \
          , y_axis='throughput' \
          , plot_type='bars' \
    )

    p = get_run_param(exp_dict, 'thread_count')
    min_threads = p[0]
    max_threads = p[len(p)-1]
    filter_string = 'thread_count in ({}, {})'.format(min_threads, max_threads)

    add_plot_set( \
            exp_dict \
          , name='maxresident-z{zeta}-n{thread_count}.png' \
          , title='z={zeta} {thread_count} threads: max resident size (MB)' \
          , filter=filter_string \
          , varying_cols_list=['zeta', 'thread_count'] \
          , series='alg' \
          , x_axis='thread_count' \
          , y_axis='maxresident_mb' \
          , plot_type='bars' \
    )
    ## note: we include thread_count above as a "dummy" x-axis so "alg" can be the *series*,
    #        and we can get a plot style consistent with the above throughput plots.
    #        (you cannot have series without an x-axis.)
    #        since the thread count is filtered to a single element,
    #        no difference is actually visible on the x-axis...

    ## render one legend for all plots (since the legend is the same for all)
    add_plot_set(exp_dict, name='legend.png', series='alg', x_axis='thread_count', \
            y_axis='throughput', plot_type='bars', \
            plot_cmd_args='--legend-only --legend-columns 3')

    ## we place the above legend on each HTML page by providing "legend_file"
    add_page_set(exp_dict, image_files='throughput-z{zeta}.png', legend_file='legend.png')
    add_page_set(exp_dict, image_files='maxresident-z{zeta}-n{thread_count}.png', legend_file='legend.png')

    ## POSSIBLE TODO:
    # more easy subfields:
    #  'txn_cnt=18614184',
    #  'abort_cnt=47430',
    #  'run_time=873.902393',
    #  'ixTimeContains=572.101776',
    #  'ixTotalTime=572.101776',
    #  'ixThroughput=98991842.440466',
    #  'throughput=4047013.703837',
    #
    # more info from time (% cpu, sys vs user):
    #     1269.07user 48.47system 0:09.30elapsed 14162%CPU (0avgtext+0avgdata 12697680maxresident)k
    #     0inputs+104outputs (0major+3176941minor)pagefaults 0swaps
    #
    # thread fairness graph:
    #     [tid=29] txn_cnt=97370,abort_cnt=257
    #
    # also try with TPCC?
    # subfield extraction by name instead of by ix (actually eliminates the need for a curried subextractor...)


def get_extractor_for_summary_subfield_numeric(subfield_ix):
    def curried_extractor(exp_dict, file_name, field_name):
        # s = shell_to_str('grep summary {file_name} | cut -d" " -f{subfield_ix}'.format(file_name=file_name, subfield_ix=subfield_ix))
        s = shell_to_str('grep summary {file_name} | cut -d" " -f{subfield_ix} | tr -d ",a-zA-Z_= "'.format(file_name=file_name, subfield_ix=subfield_ix))
        if '.' in s: return float(s)
        if not s:
            print(Back.RED+Fore.BLACK+'## ERROR: subfield_ix {} of field {} and file {} was empty'.format(subfield_ix, field_name, file_name)+Style.RESET_ALL)
        return int(s)
    return curried_extractor

def get_extractor_for_summary_subfield_str(subfield_ix):
    def curried_extractor(exp_dict, file_name, field_name):
        return shell_to_str('grep summary {file_name} | cut -d" " -f{subfield_ix} | tr -d ","'.format(file_name=file_name, subfield_ix=subfield_ix))
    return curried_extractor

def extract_alg_from_binary_name(exp_dict, file_name, field_name):
    binary = grep_line(exp_dict, file_name, 'binary')
    tokens = binary.split('_')
    return '_'.join(tokens[2:])

def extract_maxres(exp_dict, file_name, field_name):
    ## manually parse the maximum resident size from the output of `time` and add it to the data file
    maxres_kb_str = shell_to_str('grep "maxres" {} | cut -d" " -f6 | cut -d"m" -f1'.format(file_name))
    return float(maxres_kb_str) / 1000

## compute smallest number that is at least desired_size,
## AND has, as its unique prime factors,
## all unique prime factors across all numbers in thread_count_list
def compute_suitable_initsize(thread_count_list, desired_size):
    # print('compute_suitable_initsize: thread_count_list={} desired_size={}'.format(thread_count_list, desired_size))

    factors = get_common_prime_factors(thread_count_list)
    fprod = 1
    for f in factors.keys():
        fprod *= (f**factors[f])

    # fprod = int(np.prod(list(factors)))
    div = desired_size // fprod
    remainder = desired_size % fprod
    # print('    fprod={} div={} remainder={}'.format(fprod, div, remainder))

    retval = fprod*div if not remainder else fprod*(div+1)
    # print('    retval={}'.format(retval))
    # sys.exit(1)
    return retval

def get_prime_factors_bruteforce(n):
    factors = dict()
    if n <= 1: return factors
    i = 2
    while i * i <= n:
        if n % i:
            i += 1
        else:
            n //= i
            if i not in factors.keys(): factors[i] = 1
            else: factors[i] += 1
    if n > 1:
        if n not in factors.keys(): factors[n] = 1
        else: factors[n] += 1
    return factors

def get_common_prime_factors(number_list):
    factors = dict()
    for t in number_list:
        t = int(t)
        factormap = get_prime_factors_bruteforce(t)
        # print('    factormap({})={}'.format(t, factormap))
        for f in factormap.keys():
            if f not in factors.keys(): factors[f] = factormap[f]
            else: factors[f] = max(factormap[f], factors[f])
    # print('    factors={}'.format(factors))
    return factors
