#!/usr/bin/python3

##
## Possible improvements
##
## - plotting sanity checks: optional fields for user predictions in plot preparation? %min/max range threshold for warning? predicted rows per data point?
## - web page generation (specify page variables, then specify which plots to include (filenames with replacement)... can either specify row variable w/range and col variable w/range, or specify where individual plots go in a table (r,c) layout) ; also, add per-row raw data links with text file rows: <a href=...>raw data</a><pre>[usual padded columnar data]</pre>
## - break relative path dependency on tools
##
## - line plots with proper markers and styling
## - stacked bar plots
##
## - "clean" version of example without comments to show that it's still clear, but also way more concise...
## - default-off optional web page deploy (really it's a single shell call, and i can just do that in my own non-example scripts, with host detection first...)
## - test framework on a different user (proper independence from global tools namespace?)
##
## - parallel plot construction
## - animation support? (proper matplotlib animations???) (fractional row plotting with desired frame rate + count? need to specify a time axis...)
##
## - ability to link run_params so it's not just the plain cross product of all run_params that gets explored... (or perhaps a user-specified predicate that tests and rejects combinations of run_params)
## - dependencies? (install on a clean ubuntu vm? docket container?)
##
## More general improvements (mostly to setbench -- written here because I'm lazy, and these are just moving to follow my attention/gaze...)
##
## - clean up compile.sh
## - copy direct clone info into README.md
##
## - advplot into tools
## - equivalent of this for macrobench?
##
## - stress test scripts
## - leak checking scripts
## - flamegr script
## - dashboard for performance comparison of DS
##
## - experiment example for comparison of reclamation algs
## - memhook integration with example experiments
## - worked example of DS perf comparison understanding from basic cycles -> instr/cachemiss -> location w/perfrecord -> memhook layout causation
##      temporal profiling with perf -> temporal flamegr of measured phase
##

from _user_config import *

def quoted(col_name, value, quote_char="'"):
    retval = str(value)
    if col_name not in g['data_fields']: return retval
    if g['data_fields'][col_name]['coltype'].upper() != 'TEXT': return retval
    return quote_char + retval + quote_char

tee('## Script running with the following parameters:')
tee()
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

def print_time(sec):
    if sec == 0:
        return '00s'
    retval = ''
    if sec > 86400:
        retval = retval + '{}d'.format(str(int(sec//86400)).zfill(2))
        sec = sec % 86400
    if sec > 3600:
        retval = retval + '{}h'.format(str(int(sec//3600)).zfill(2))
        sec = sec % 3600
    if sec > 60:
        retval = retval + '{}m'.format(str(int(sec//60)).zfill(2))
        sec = sec % 60
    if sec > 0:
        retval = retval + '{}s'.format(str(int(sec)).zfill(2))
    return retval

def run_param_loops(ix, actually_run=True):
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
        # log()
        # pp_log.pprint(g['replacements'])

        cmd = g['replacements']['__cmd_run']
        fname = g['replacements']['__file_run_data']
        path = g['replacements']['__path_run_data']
        host = g['replacements']['__hostname']

        if actually_run:
            tee()
            tee('step {} / {}: {} -> {}'.format(str(step).zfill(6), str(maxstep).zfill(6), cmd, os.path.relpath(path)))

        ## remember the data file we are about to (or would) create
        g['data_file_paths'].append(path)

        if actually_run:
            ## actually run experimental command
            shell_to_str('echo "__step={}\n__cmd_run={}\n__file_run_data={}\n__path_run_data={}\n__hostname={}\n" > {}'.format(str(step).zfill(6), cmd, os.path.relpath(fname), os.path.relpath(path), host, path), exit_on_error=True)
            replace_and_run('cd {__dir_run} ; {__cmd_run} >> {__path_run_data} 2>&1', exit_on_error=True)

            ## update progress info
            curr_sec = int(shell_to_str('date +%s'))
            elapsed_sec = curr_sec - started_sec
            frac_done = step / maxstep
            elapsed_sec_per_done = elapsed_sec / step
            remaining_sec = elapsed_sec_per_done * (maxstep - step)
            total_est_sec = elapsed_sec + remaining_sec
            print('step={} maxstep={} maxstep-step={}, elapsed_sec_per_done * (maxstep - step)={}, frac_done={} elapsed_sec_per_done={} elapsed_sec={} remaining_sec={} total_est_sec={}'.format(step, maxstep, maxstep-step, elapsed_sec_per_done * (maxstep - step), frac_done, elapsed_sec_per_done, elapsed_sec, remaining_sec, total_est_sec))
            tee('    (' + print_time(elapsed_sec) + ' elapsed, ' + print_time(remaining_sec) + ' est. remaining, est. total ' + print_time(total_est_sec) + ')')

    else:
        param_list = list(g['run_params'].keys())
        param = param_list[ix]
        for value in g['run_params'][param]:
            g['replacements'][param] = value
            run_param_loops(1+ix, actually_run)

step = 0
maxstep = 1
for key in g['run_params']:
    maxstep = maxstep * len(g['run_params'][key])

started_sec = int(shell_to_str('date +%s'))
# tee("started_sec={}".format(started_sec))

started = datetime.datetime.now()

if args.run:
    ## Backup ONE set of old data (to avoid mistakes...)
    tee()
    tee("## Moving old data to *.old")

    tee_replace_and_run('rm -rf {__dir_data}.old 2>&1')
    tee_replace_and_run('mv {__dir_data} {__dir_data}.old 2>&1')
    tee_replace_and_run('mkdir {__dir_data}', exit_on_error=True)

    tee()
    tee('## Running trials (starting at {})'.format(started))

    run_param_loops(0)
else:
    tee()
    tee("## Skip running (reuse old data)")
    run_param_loops(0, actually_run=False)

#########################################################################
#### Create sqlite database
#########################################################################

db_path = 'database.sqlite'.format(g['replacements']['__dir_data'])

tee()
if args.createdb:
    tee("## Creating sqlite database from experimental data")
    shell_to_str('rm -f {}'.format(db_path), exit_on_error=True)
else:
    tee("## Skipping creation of sqlite database (using existing db)")

con = sqlite3.connect(db_path)
cur = con.cursor()

if args.createdb:
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

if args.createdb:
    for data_file_path in g['data_file_paths']:
        short_fname = os.path.relpath(data_file_path)
        log()
        tee('processing {}'.format(short_fname))
        txn = 'INSERT INTO data ({}) VALUES ({})'

        values = ''
        for field in g['data_fields'].keys():
            extractor = g['data_fields'][field]['extractor']
            validator = g['data_fields'][field]['validator']

            value = extractor(data_file_path, field)
            if not validator(value): validation_errors.append(short_fname + ': ' + field + '=' + value)
            log('    extractor for field={} gets value={} valid={}'.format(field, value, validator(value)))

            if "'" in str(value) or '"' in str(value):
                tee("\n\nERROR: value {} contains single or double quotes! this will break sql insertion statements! cannot proceed. exiting...\n".format(value))
                exit(1)

            values += '{}{}'.format(('' if values == '' else ', '), quoted(field, value))

        txn = txn.format(fields, values)
        log()
        log(txn)
        cur.execute(txn)

    con.commit()

#########################################################################
#### Produce plots
#########################################################################

def _cf_any(col_name, col_values):
    return True

def _cf_not_intrinsic(col_name, col_values):
    if len(col_name) >= 2 and col_name[0:2] == '__': return False
    return True

def _cf_not_identical(col_name, col_values):
    first = col_values[0]
    for val in col_values:
        if val != first: return True
    return False

## assumes identical args
def _cf_and(f, g):
    def composed_fn(col_name, col_values):
        return f(col_name, col_values) and g(col_name, col_values)
    return composed_fn

## quote_text only works if there are headers
## column_filters only work if there are headers (even if they filter solely on the data)
## sep has no effect if columns are aligned
def table_to_str(table, headers=None, aligned=False, quote_text=True, sep=' ', column_filter=_cf_any):
    assert(headers or column_filter == _cf_any)
    assert(headers or not quote_text)
    assert(sep == ' ' or not aligned)

    buf = io.StringIO()

    ## first check if headers is
    ## 1. a list of string, or                          (manual input list)
    ## 2. a list of tuples each containing one string   (output of sqlite query for field names)
    ## and if needed transform #2 into #1.
    if headers:
        if sum([len(row) for row in headers]) == len(headers):
            ## we are in case #2
            headers = [row[0] for row in headers]

        ## also compute a bitmap that dictates which columns will be included (not filtered out)
        include_ix = [-1 for x in range(len(headers))]
        for i, col_name in zip(range(len(headers)), headers):
            col_values = [row[i] for row in table]
            include_ix[i] = column_filter(col_name, col_values)
        log('include_ix={}'.format(include_ix))

    if not aligned:
        if headers:
            first = True
            for i, col_name in zip(range(len(headers)), headers):
                if include_ix[i]:
                    if not first: buf.write(sep)
                    buf.write(col_name)
                    first = False
            buf.write('\n')

        for row in table:
            first = True
            for row in table:
                for i, col in zip(range(len(row)), row):
                    if not headers or include_ix[i]:
                        if not first: buf.write(sep)
                        s = str(col)
                        if headers: s = quoted(headers[i], s)
                        buf.write( s )
                        first = False
                buf.write('\n')

    ## aligned
    else:
        if headers:
            cols_w = [ max( [len(str(row[i])) for row in table] + [len(headers[i])] ) for i in range(len(headers)) ]
        else:
            cols_w = [ max( [len(str(row[i])) for row in table] ) for i in range(len(table[0])) ]

        if headers:
            for i, col_name in zip(range(len(headers)), headers):
                if include_ix[i]:
                    buf.write( str(col_name).ljust(3+cols_w[i]) )
            buf.write('\n')

        for row in data_records:
            for i, col in zip(range(len(row)), row):
                if not headers or include_ix[i]:
                    s = str(col)
                    if headers: s = quoted(headers[i], s)
                    buf.write( s.ljust(3+cols_w[i]) )
            buf.write('\n')

    return buf.getvalue()

if args.plot:
    tee()
    tee("## Creating plots")
    plot_set_num = 0
    for plot_set in g['plots']:
        plot_set_num += 1

        ## build a SET of plots (defined by the varying_cols_list of plot_set)

        tee()
        tee("# plot set {}".format(plot_set_num))
        pp_log.pprint(plot_set)
        pp_stdout.pprint(plot_set)

        where_clause = ''
        if plot_set['filter'] != '' and plot_set['filter'] != 'None' and plot_set['filter'] != None:
            where_clause = 'WHERE ({})'.format(plot_set['filter'])

        ## get distinct assignments of columns in varying_cols_list
        varying_cols = ','.join(plot_set['varying_cols_list'])
        txn = 'SELECT DISTINCT {} FROM data {}'.format(varying_cols, where_clause)
        cur.execute(txn)
        varying_cols_records = cur.fetchall()

        #################################################################
        ## Sanity checks on PLOT SET
        #################################################################

        ## sanity check: varying cols list contains a field that is NOT a run_param
        if any([x not in g['run_params'].keys() for x in plot_set['varying_cols_list']]):
            # print(plot_set['varying_cols_list'])
            # print(g['run_params'].keys())
            # print([x not in g['run_params'].keys() for x in plot_set['varying_cols_list']])
            # print(list(filter(lambda col: col not in g['run_params'].keys(), plot_set['varying_cols_list'])))
            s = 'WARNING: plot_set {} list of varying columns {} contains field(s) {} that are NOT run_params! this is most likely a mistake, as the replacement token for such a field will NOT be well defined (since its value would be unique to a single TRIAL, rather than a plot)!'.format( plot_set['filename'], plot_set['varying_cols_list'], list(filter(lambda col: col not in g['run_params'].keys(), plot_set['varying_cols_list'])) )
            tee(s)
            g['sanity_check_failures'].append(s)
            ## note: this is only a fatal error if the user tries to use such a field as a replacement token...

        ## sanity check: plot_set producing UNEXPECTED NUMBER of plots
        if plot_set['filter'] == '' or all([x not in plot_set['filter'] for x in plot_set['varying_cols_list']]):
            ## if there's no filter, or none of the varying cols are in the filter, we can predict # of plots
            cur.execute('SELECT __path_run_data FROM data {}'.format(where_clause))
            plot_run_files = [x[0] for x in cur.fetchall()]
            # log('g[\'run_params\']={} plot_set['varying_cols_list']={} lengths=\'{}\''.format(g['run_params'], plot_set['varying_cols_list'], [ len(g['run_params'][x]) for x in filter(lambda param: param in plot_set['varying_cols_list'], g['run_params'].keys()) ]))
            expected_plot_count = numpy.prod( [ len(g['run_params'][x]) for x in filter(lambda param: param in plot_set['varying_cols_list'], g['run_params'].keys()) ] )

            if len(varying_cols_records) != expected_plot_count:
                tee('txn={}'.format(txn))
                s = 'WARNING: expected plot_set {} to produce {} plots but it produces {}'.format(plot_set['filename'], expected_plot_count, len(varying_cols_records))
                tee(s)
                g['sanity_check_failures'].append(s)

        ## sanity check: plot_set producing ZERO plots
        if len(varying_cols_records) == 0:
            tee('txn={}'.format(txn))
            tee('ERROR: plot_set {} produces ZERO plots'.format(plot_set['filename']))
            exit(1)                                     ## ZERO is definitely a mistake so fail fast

        #################################################################
        ## For each plot
        #################################################################

        ## for each distinct assignment
        for varying_cols_rowstr in varying_cols_records:
            log()
            log()
            # tee("varying_cols_rowstr={}".format(varying_cols_rowstr))

            #############################################################
            ## Data filtering and aggregation
            #############################################################

            ## build a dict for this assignment
            varying_cols_vals = dict()
            for i in range(len(varying_cols_rowstr)):
                varying_cols_vals[plot_set['varying_cols_list'][i]] = varying_cols_rowstr[i]
                g['replacements'][plot_set['varying_cols_list'][i]] = varying_cols_rowstr[i]
            log("varying_cols_vals={}".format(varying_cols_vals))

            ## create new where clause (including these varying_cols_vals in the filter)
            log('preliminary where_clause={}'.format(where_clause))
            where_clause_new = where_clause
            if where_clause_new.strip() == '':
                where_clause_new = 'WHERE (1==1)'
            for key in varying_cols_vals.keys():
                where_clause_new += ' AND {} == {}'.format(key, quoted(key, varying_cols_vals[key]))
            log("amended where_clause_new={}".format(where_clause_new))

            ## check if we have a series column for this set of plots (and if so, construct an appropriate string for our query expression)
            series = plot_set['series']
            if series != '' and series != 'None' and series != None:
                series += ', '

            ## perform sqlite query to fetch raw data
            txn = 'SELECT {}{}, {} FROM data {}'.format(series, plot_set['x_axis'], plot_set['y_axis'], where_clause_new)
            log('txn={}'.format(txn))
            cur.execute(txn)
            data_records = cur.fetchall()

            # ## perform sqlite query to fetch aggregated data
            # txn = 'SELECT {}{}, avg({}) FROM data {} GROUP BY {}{}'.format(series, plot_set['x_axis'], plot_set['y_axis'], where_clause_new, series, plot_set['x_axis'])
            # log('txn={}'.format(txn))
            # cur.execute(txn)
            # data_records = cur.fetchall()

            #############################################################
            ## Dump PLOT DATA to a text file
            #############################################################

            ## now we have the data we want to plot...
            ## write it to a text file (so we can feed it into a plot command)
            pp_log.pprint(data_records)

            plot_filename = g['replacements']['__dir_data'] + '/' + replace(plot_set['filename']).replace(' ', '_')
            plot_txt_filename = plot_filename.replace('.png', '.txt')

            g['replacements']['__plot_filename'] = plot_filename
            g['replacements']['__plot_txt_filename'] = plot_txt_filename

            tee('plot {}'.format(os.path.relpath(plot_filename)))
            tee('plot {}'.format(os.path.relpath(plot_txt_filename)))
            log('plot_filename={}'.format(plot_filename))
            log('plot_txt_filename={}'.format(plot_txt_filename))

            plot_txt_file = open(plot_txt_filename, 'w')
            plot_txt_file.write(table_to_str(data_records, quote_text=False, aligned=False))
            plot_txt_file.close()

            # plot_txt_file = open(plot_txt_filename, 'w')
            # for row in data_records:
            #     for col in row:
            #         if col != row[0]: plot_txt_file.write(' ')
            #         plot_txt_file.write(str(col))
            #     plot_txt_file.write('\n')
            # plot_txt_file.close()

            #############################################################
            ## Create a plot
            #############################################################

            ## determine which command line tool to use to produce the plot
            ## and do a quick check to see if it's legal to use it with the given options
            ptype = plot_set['plot_type'].lower()
            if ptype == 'bars':
                plot_tool_path = 'plotbars.py' if series != '' else 'plotbar.py'
            elif ptype == 'line':
                plot_tool_path = 'plotlines.py' if series != '' else 'plotline.py'
            elif ptype == 'hist2d':
                plot_tool_path = 'plothist2d.py'
                if series != '':
                    tee('ERROR: cannot plot {} with series'.format(ptype))
                    exit(1)
            elif ptype == 'histogram':
                plot_tool_path = 'histogram.py'
                if series != '':
                    tee('ERROR: cannot plot {} with series'.format(ptype))
                    exit(1)
                if y_axis != '':
                    tee('ERROR: cannot plot {} with y_axis'.format(ptype))
                    exit(1)

            plot_tool_path = '../../tools/' + plot_tool_path
            g['replacements']['__plot_tool_path'] = plot_tool_path

            ## do the actual plot

            title = ''
            if plot_set['title'] != '': title = '-t {}'.format(replace(plot_set['title']))
            if '"' in title:
                tee('ERROR: plot title cannot have a double quote character ("); this occurred in title {}'.format(title))
                exit(1)

            g['replacements']['__title'] = title
            g['replacements']['__plot_cmd_args'] = plot_set['plot_cmd_args']

            log('running plot_cmd {}'.format(replace('{__plot_tool_path} -i {__plot_txt_filename} -o {__plot_filename} "{__title}" {__plot_cmd_args}')))
            if args.debug_exec_plotcmd: ## debug option to disable plot cmd exec
                log_replace_and_run('{__plot_tool_path} -i {__plot_txt_filename} -o {__plot_filename} "{__title}" {__plot_cmd_args}')

            #############################################################
            ## Dump FULL COLUMN DATA to the text file
            #############################################################
            ##
            ## fetch data with full detail (all columns)
            ## and append it to the existing text result
            ## (AFTER we've used that text to plot)
            ##

            ## first fetch header info
            cur.execute('SELECT name FROM PRAGMA_TABLE_INFO("data")')
            header_records = cur.fetchall()

            ## then fetch the data
            txn = 'SELECT * FROM data {}'.format(where_clause_new)
            log('txn={}'.format(txn))
            cur.execute(txn)
            data_records = cur.fetchall()

            ## dump to disk
            plot_txt_file = open(plot_txt_filename, 'a')
            plot_txt_file.write('\n\n')
            plot_txt_file.write('## full column dataset\n')
            plot_txt_file.write('\n')
            plot_txt_file.write(table_to_str(data_records, header_records, aligned=True))
            plot_txt_file.close()

            # ##
            # ## concatenate all raw data outfiles created for the plotted runs to the output text file
            # ##

            # cur.execute('SELECT __path_run_data FROM data {}'.format(where_clause_new))
            # plot_run_files = [x[0] for x in cur.fetchall()]
            # log('plot_run_files={}'.format(plot_run_files))

            #############################################################
            ## Sanity checks on PLOT
            #############################################################

            ## perform sqlite query to fetch aggregated data
            log('sanity check: counting number of rows being aggregated into each plot data point')
            txn = 'SELECT count({}), {}{} FROM data {} GROUP BY {}{}'.format(plot_set['y_axis'], series, plot_set['x_axis'], where_clause_new, series, plot_set['x_axis'])
            log('txn={}'.format(txn))
            cur.execute(txn)
            data_records = cur.fetchall()
            row_counts = [row[0] for row in data_records]
            pp_log.pprint(data_records)

            if '__trials' in g['run_params'].keys():
                num_trials = len(g['run_params']['__trials'])
                if any([x != num_trials for x in row_counts]):
                    s = '\n'
                    s += ('WARNING: some data point(s) in {}\n'.format(os.path.relpath(plot_txt_filename)))
                    s += ('         aggregate an unexpected number of rows!\n')
                    s += ('         most likely we should be aggregating ONE row per trial, i.e., {}.\n'.format(num_trials))
                    s += ('         however, row counts for data point(s) are:\n')
                    s += ('             {}\n'.format(row_counts))
                    s += ('         if row counts are smaller than expected, you may have filtered incorrectly.\n')
                    s += ('             where clause: {}\n'.format(where_clause_new))
                    s += ('         if row counts are larger than expected, you may have forgotten a run_param\n')
                    s += ('         that should be part of varying_cols_list for the plot.\n')
                    s += ('\n')

                    ## fetch an example set of possibly problematic rows
                    s += '## Fetching an example set of possibly problematic rows:\n'

                    ## build where clause that incorporates filtering to some problematic row in data_records

                    for row in data_records:
                        if row[0] != num_trials: problem_col_values = row[1:]

                    if series != '':
                        problem_col_names = [plot_set['series'], plot_set['x_axis']]
                    else:
                        problem_col_names = [plot_set['x_axis']]

                    assert(len(problem_col_names) == len(problem_col_values))

                    where_clause_problem = where_clause_new
                    assert(where_clause_problem != None and where_clause_problem != 'None' and where_clause_problem.strip() != '')

                    for col_name, col_value in zip(problem_col_names, problem_col_values):
                        where_clause_problem += ' AND ' + col_name + ' == ' + quoted(col_name, col_value)

                    txn = 'SELECT * FROM data {}'.format(where_clause_problem)
                    # log('txn={}'.format(txn))
                    cur.execute(txn)
                    data_records = cur.fetchall()

                    s += table_to_str(data_records, header_records, aligned=True)

                    ## NOTE TO SELF: show the power of this mechanism by including two "distribution" values but neglecting to add that field to a plot!!

                    s += '\n'
                    s += '## FILTERING TO REMOVE COLUMNS WITH NO DIFFERENCES, AND INTRINSIC COLUMNS...\n'
                    s += '##     any problem is likely in one of these columns:\n'
                    s += table_to_str(data_records, header_records, aligned=True, column_filter=_cf_and(_cf_not_intrinsic, _cf_not_identical))

                    tee(s)
                    g['sanity_check_failures'].append(s)

                    if not args.continue_on_warn_agg_row_count:
                        tee('EXITING... (run with --continue-on-warn-agg-row-count to disable this.)')
                        tee()
                        exit(1)

else:
    tee()
    tee("## Skipping plot creation")

#########################################################################
#### Finish
#########################################################################

tee()
tee('## Zipping results into ./data.zip')
log()
log_replace_and_run('zip -r data.zip {__dir_data} log.txt')

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

if len(g['sanity_check_failures']) > 0:
    tee()
    tee('################################################')
    tee('#### WARNING: there were sanity check failures!')
    tee('################################################')
    tee()
    for item in g['sanity_check_failures']:
        tee('    ' + item)

print()
print("Note: log can be viewed at ./log.txt")

g['log'].close()
