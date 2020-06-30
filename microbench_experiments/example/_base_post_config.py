#!/usr/bin/python3

##
## Possible improvements
##
## - some mechanism for passing token "replacement" information to user_plot_style.py etc...
## - plotting sanity checks: optional fields for user predictions in plot preparation? %min/max range threshold for warning? predicted rows per data point?
## - break relative path dependency on tools
##
## - line plots with proper markers and styling
## - stacked bar plots
##
## - "clean" version of example without comments to show that it's still clear, but also way more concise...
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

from _base_pre_config import *

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
tee('#########################################')
if args.testing:
    tee('## WARNING: RUNNING IN TESTING MODE')
else:
    tee('## NOTE: RUNNING IN PRODUCTION MODE')
tee('#########################################')

#########################################################################
#### Compile into bin_dir
#########################################################################

if args.compile:
    tee()
    tee("## Removing old binaries")

    tee_replace_and_run('rm -rf {__dir_run} 2>&1')
    tee_replace_and_run('mkdir {__dir_run}', exit_on_error=True)

    tee()
    tee(replace('## Compiling into {__dir_run}'))

    log()
    log_replace_and_run('cd {__dir_compile} ; ' + g['cmd_compile'], exit_on_error=True)
else:
    tee()
    tee('## Skipping compilation because of cmdline arg...')

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
            # print('step={} maxstep={} maxstep-step={}, elapsed_sec_per_done * (maxstep - step)={}, frac_done={} elapsed_sec_per_done={} elapsed_sec={} remaining_sec={} total_est_sec={}'.format(step, maxstep, maxstep-step, elapsed_sec_per_done * (maxstep - step), frac_done, elapsed_sec_per_done, elapsed_sec, remaining_sec, total_est_sec))
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
    tee()
    tee("## Removing old data")

    tee_replace_and_run('rm -rf {__dir_data} 2>&1')
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

db_path = 'output_database.sqlite'.format(g['replacements']['__dir_data'])

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

fields = ''
for field in g['data_fields'].keys():
    fields += ('' if fields == '' else ', ') + field

if args.createdb:
    tee()
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
def table_to_str(table, headers=None, aligned=False, quote_text=True, sep=' ', column_filter=_cf_any, row_header_html_link=''):
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
        # log('include_ix={}'.format(include_ix))

    if not aligned:
        if headers:
            first = True

            if headers and row_header_html_link != '' and row_header_html_link in headers:
                buf.write(row_header_html_link)
                first = False

            for i, col_name in zip(range(len(headers)), headers):
                if include_ix[i]:
                    if not first: buf.write(sep)
                    buf.write(col_name)
                    first = False
            buf.write('\n')

        for row in table:
            first = True

            if headers and row_header_html_link != '' and row_header_html_link in headers:
                buf.write('</pre><a href="{}"><pre>{}</pre></a><pre>'.format(row_header_html_link, row_header_html_link))
                first = False

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
            if headers and row_header_html_link != '' and row_header_html_link in headers:
                buf.write( row_header_html_link.ljust(3+len(row_header_html_link)) )

            for i, col_name in zip(range(len(headers)), headers):
                if include_ix[i]:
                    buf.write( str(col_name).ljust(3+cols_w[i]) )
            buf.write('\n')

        for row in data_records:
            if headers and row_header_html_link != '' and row_header_html_link in headers:
                col_of_row_header = list(filter(lambda __i: (headers[__i] == row_header_html_link), range(len(headers))))[0]
                value_of_row_header = row[col_of_row_header]
                desired_width = 3+len(row_header_html_link) - 1
                spaces_to_add = ''
                if desired_width > len(str(value_of_row_header)):
                    spaces_to_add = '.' * (desired_width - len(str(value_of_row_header)))
                buf.write('</pre><a href="{}"><pre>{}</pre></a><pre>{}</pre><pre>'.format( value_of_row_header, '[view raw data]', spaces_to_add ))

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
            s = 'WARNING: plot_set {} list of varying columns {} contains field(s) {} that are NOT run_params! this is most likely a mistake, as the replacement token for such a field will NOT be well defined (since its value would be unique to a single TRIAL, rather than a plot)!'.format( plot_set['name'], plot_set['varying_cols_list'], list(filter(lambda col: col not in g['run_params'].keys(), plot_set['varying_cols_list'])) )
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
                s = 'WARNING: expected plot_set {} to produce {} plots but it produces {}'.format(plot_set['name'], expected_plot_count, len(varying_cols_records))
                tee(s)
                g['sanity_check_failures'].append(s)

        ## sanity check: plot_set producing ZERO plots
        if len(varying_cols_records) == 0:
            tee('txn={}'.format(txn))
            tee('ERROR: plot_set {} produces ZERO plots'.format(plot_set['name']))
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

            #############################################################
            ## Dump PLOT DATA to a text file
            #############################################################

            ## now we have the data we want to plot...
            ## write it to a text file (so we can feed it into a plot command)
            pp_log.pprint(data_records)

            plot_filename = g['replacements']['__dir_data'] + '/' + replace(plot_set['name']).replace(' ', '_')
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
                if plot_set['y_axis'] != '':
                    tee('ERROR: cannot plot {} with y_axis'.format(ptype))
                    exit(1)

            plot_tool_path = get_dir_tools() + '/' + plot_tool_path
            g['replacements']['__plot_tool_path'] = plot_tool_path

            ## do the actual plot

            title = ''
            if plot_set['title'] != '': title = '-t {}'.format(replace(plot_set['title']))
            if '"' in title:
                tee('ERROR: plot title cannot have a double quote character ("); this occurred in title {}'.format(title))
                exit(1)

            g['replacements']['__title'] = title
            g['replacements']['__plot_cmd_args'] = plot_set['plot_cmd_args']

            plot_style_hooks_file = ''
            if plot_set['plot_style_hooks_file'] != '':
                plot_style_hooks_file = ' --style-hooks {}'.format(plot_set['plot_style_hooks_file'])

            cmd='{__plot_tool_path} -i {__plot_txt_filename} -o {__plot_filename} "{__title}" {__plot_cmd_args}' + plot_style_hooks_file
            log('running plot_cmd {}'.format(replace(cmd)))
            if args.debug_exec_plotcmd: ## debug option to disable plot cmd exec
                log_replace_and_run(cmd, exit_on_error=True)

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
            plot_txt_full_filename= plot_filename.replace('.png', '_full.txt')
            plot_txt_full_filename = open(plot_txt_full_filename, 'w')
            plot_txt_full_filename.write(table_to_str(data_records, header_records, aligned=True, row_header_html_link='__file_run_data'))
            plot_txt_full_filename.close()

            #############################################################
            ## Sanity checks on PLOT
            #############################################################

            ## perform sqlite query to fetch aggregated data
            log('sanity check: counting number of rows being aggregated into each plot data point')
            txn = 'SELECT count({}), {}{} FROM data {} GROUP BY {}{}'.format(plot_set['y_axis'], series, plot_set['x_axis'], where_clause_new, series, plot_set['x_axis'])
            cur.execute(txn)
            data_records = cur.fetchall()
            row_counts = [row[0] for row in data_records]

            if '__trials' in g['run_params'].keys():
                num_trials = len(g['run_params']['__trials'])
                if any([x != num_trials for x in row_counts]):
                    log('txn={}'.format(txn))
                    pp_log.pprint(data_records)

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
    tee('## Skipping plot creation')

#########################################################################
#### Create html pages to show plots
#########################################################################

from _templates_html import *

def for_all_combinations(ix, to_enumerate, enumerated_values, func, func_arg):
    if ix == len(to_enumerate):
        return [func(enumerated_values, func_arg)]
    else:
        field = to_enumerate.keys()[ix]
        retlist = []
        for value in to_enumerate[field]:
            enumerated_values[field] = value
            retlist += for_all_combinations(1+ix, to_enumerate, enumerated_values, func, func_arg)
        return retlist

# todo:
# - preload? gif support?
#   - hover_html="onmouseover=\\\"this.src='${prefix_escaped}.gif'\\\" onmouseout=\\\"this.src='${prefix_escaped}.png'\\\""
#   - preloadtags="${preloadtags}<link rel=\\\"preload\\\" as=\\\"image\\\" href=\\\"${prefix_escaped}.gif\\\">"

# design sketch:
#
# - design build_html_page_set and descendents so they don't produce pages/tables/rows/cols
#   for data that is FILTERED OUT.
#   - i think i'll do this by writing a function that looks at the produced image files,
#     filters them by the plot_set's page_field_list, table_field, row_field, column_field,
#     and returns the filtered list.
#   - the filtering will likely be much easier if i DELETE old entries from the replacement
#     dict that i'm passing around these functions.
#     (so it doesn't filter too much by accident)
#   - with this function, i can easily check at each step whether there are *any* plots
#     that satisfy the given filters, and continue or cut off that path in the call graph.
#   - and, i can use this function at the bottom level of the call graph to get
#     a unique image filename to include in the table cell. dual purpose!

def get_filtered_image_paths(replacement_values, page_set):
    image_file_pattern = page_set['image_files']
    to_replace_set = set(s.replace('{', '').replace('}', '') for s in re.findall(r'\{[^\}]*\}', image_file_pattern))
    auto_replace_set = set(k for k in replacement_values.keys())
    to_manually_replace_set = to_replace_set - auto_replace_set
    for to_manually_replace in to_manually_replace_set:
        image_file_pattern = image_file_pattern.replace('{' + to_manually_replace + '}', '*')
    image_file_pattern = image_file_pattern.format(**replacement_values).replace(' ', '_')
    paths_found = glob.glob(get_dir_data() + '/' + image_file_pattern)

    # tee('image_file_pattern       ={}'.format(image_file_pattern))
    # tee('to_replace_set           ={}'.format(to_replace_set))
    # tee('auto_replace_set         ={}'.format(auto_replace_set))
    # tee('to_manually_replace_set  ={}'.format(to_manually_replace_set))
    # tee('get_filtered_image_paths ={}'.format(image_file_pattern))
    # tee('glob for this pattern    ={}'.format(paths_found)))
    return paths_found



filename_to_html = dict()           ## used by all html building functions below
filenames_in_navigation = list()    ## used by all html building functions below



## also builds any raw data subpage linked from this cell
def get_td_contents_html(replacement_values, page_set):
    tee('get_td_contents_html: replacement_values = {}'.format(replacement_values))

    ## locate image file (and obtain corresponding text and html file names)

    plot_filename = get_filtered_image_paths(replacement_values, page_set)
    assert(len(plot_filename) == 1)
    plot_filename = plot_filename[0]
    plot_filename = os.path.relpath(plot_filename, get_dir_data())

    plot_txt_filename = plot_filename.replace('.png', '.txt')
    plot_txt_full_filename = plot_filename.replace('.png', '_full.txt')
    html_filename = plot_filename.replace('.png', '.html')

    ## construct html for the cell

    td_contents_html = '<a href="{}"><img src="{}" border="0"></a>'.format( html_filename, plot_filename )

    ## create html summary and add it to the filename_to_html global dict...
    ##
    ## note: this isn't really the "right" place to do this from an engineering perspective,
    ##       BUT if i did this somewhere else there would be a LOT of repeated code,
    ##       as this is the only point in the code where we've already determined exactly
    ##       which pages, tables, rows, cols, images SHOULD exist in the final output...

    with open(get_dir_data() + '/' + plot_txt_filename, 'r') as f:
        data_txt = f.read()
    with open(get_dir_data() + '/' + plot_txt_full_filename, 'r') as f:
        data_txt_full = f.read()

    content_html = '<pre>{}\n\n{}</pre>'.format(data_txt_full, data_txt)
    filename_to_html[html_filename] = template_html.replace('{template_content}', content_html)

    return td_contents_html

## also builds any raw data subpages linked from this table
def get_table_html(replacement_values, page_set, table_field, table_field_value):
    tee()
    tee('get_table_html: replacement_values = {}'.format(replacement_values))

    ## if this set of replacement_values yields no actual images, prune this TABLE
    if len(get_filtered_image_paths(replacement_values, page_set)) == 0: return ''

    column_field = page_set['column_field']
    column_field_values = [''] if (column_field == '') else g['run_params'][column_field]
    row_field = page_set['row_field']
    row_field_values = [''] if (row_field == '') else g['run_params'][row_field]

    ## header: replacement for {template_table_th_list}
    __table_th_list = ''
    __table_th_list += template_table_th.format(table_th_text=str(row_field)) ## dummy first column so we can have row titles as well

    for cval in column_field_values:
        __table_th_list += template_table_th.format(table_th_text=(column_field+'='+str(cval)))

    ## rows: replacement for {template_table_tr_list}
    __table_tr_list = ''

    for rval in row_field_values:
        replacement_values[row_field] = str(rval)

        ## if this set of replacement_values yields no actual images, prune this ROW
        if len(get_filtered_image_paths(replacement_values, page_set)) == 0:
            tee('pruning row {} in table "{}={} in page {} under replacement_values {}"'.format(rval, table_field, table_field_value, page_set['name'], replacement_values))

        else:
            __table_tr_td_list = ''                             ## all columns in row: replacement for {template_table_tr_td_list}
            __table_tr_td = str(rval)                           ## row header
            __table_tr_td_list += template_table_tr_td.format(table_tr_td_text=__table_tr_td)

            for cval in column_field_values:
                replacement_values[column_field] = str(cval)    ## one table cell: replacement for {template_table_tr_td}

                ## if this set of replacement_values yields no actual images, prune this CELL
                if len(get_filtered_image_paths(replacement_values, page_set)) == 0:
                    tee('pruning cell {} in row {} of table "{}={} in page {} under replacement_values {}"'.format(cval, rval, table_field, table_field_value, page_set['name'], replacement_values))

                else:
                    cell_html = get_td_contents_html(replacement_values, page_set)

                ## cval
                __table_tr_td_list += template_table_tr_td.format(table_tr_td_text=cell_html)
                del replacement_values[column_field]

        ## rval
        __table_tr_list += template_table_tr.format(template_table_tr_td_list=__table_tr_td_list)
        del replacement_values[row_field]

    return \
        template_table.format( \
            template_table_title=('' if (str(table_field) == '') else ('<h3>' + str(table_field) + ' = ' + str(table_field_value) + '</h3><br>')), \
            template_table_th_list=__table_th_list, \
            template_table_tr_list=__table_tr_list \
        )



## also builds any raw data pages linked from this one...
def build_html_page(replacement_values, page_set):
    tee()
    tee('build_html_page: replacement_values = {}'.format(replacement_values)) #dict( (k,g['replacements'][k]) for k in page_set['fields']) )

    ## if this set of replacement_values yields no actual images, prune this PAGE
    if len(get_filtered_image_paths(replacement_values, page_set)) == 0: return None

    content_html = ''

    table_field = page_set['table_field']
    table_field_values = [''] if (table_field == '') else g['run_params'][table_field]
    for table_field_value in table_field_values:
        if table_field != '': replacement_values[table_field] = table_field_value
        content_html += get_table_html(replacement_values, page_set, table_field, table_field_value)
        if table_field != '': del replacement_values[table_field]

    ## add field replacements to filename as appropriate
    filename = page_set['name'].format(replacement_values)
    keys = page_set['page_field_list']
    values = [g['replacements'][k] for k in keys]
    if len(values) > 0:
        assert(len(keys) == len(values))
        filename += '-' + '-'.join([(str(k).replace(' ', '_')+'_'+str(v).replace(' ', '_')) for k,v in zip(keys, values)])
    filename += '.html'

    filenames_in_navigation.append(filename)
    filename_to_html[filename] = template_html.replace('{template_content}', content_html)



def build_html_page_set(i, page_set):
    tee()
    tee('page_set {}'.format(i))
    pp_stdout.pprint(page_set)
    pp_log.pprint(page_set)

    to_enumerate = dict((k, g['run_params'][k]) for k in page_set['page_field_list'])
    enumerated_values = dict()
    for_all_combinations(0, to_enumerate, enumerated_values, build_html_page, page_set)



## design:
##
## all html pages are first created in memory--stored in global dict filename_to_html.
## then, they are all dumped to disk.

if args.pages:
    tee()
    tee('## Creating html pages to show plots')

    for i, page_set in zip(range(len(g['pages'])), g['pages']):
        build_html_page_set(i, page_set)

    ## build index.html from template and provided content_index_html string (if any)

    content_index_html = ''
    if 'content_index_html' in g:
        content_index_html = g['content_index_html']

    filename_to_html['index.html'] = template_html.replace('{template_content}', content_index_html)
    filenames_in_navigation.append('index.html')

    ## build navigation sidebar links

    tee()
    nav_link_list = ''
    for f in filenames_in_navigation:
        pretty_name = f.replace('.html', '').replace('_', ' ').replace('-', ' ')
        tee('adding "{}" a.k.a "{}" to navigation'.format(f, pretty_name))
        nav_link_list += template_nav_link.format(nav_link_href=f, nav_link_text=pretty_name)

    ## add navigation sidebar to pages

    tee()
    for i, fname in zip(range(len(filename_to_html.keys())), filename_to_html.keys()):
        html = filename_to_html[fname]
        filename_to_html[fname] = html.replace('{template_nav_link_list}', nav_link_list)
        tee('adding navigation to filename_to_html[{}]={}'.format(i, fname))
        # tee('html_summary_file = {}, html = {}'.format(fname, html_summary_files[fname]))

    ## dump html files to disk

    tee()
    for i, fname in zip(range(len(filename_to_html.keys())), filename_to_html.keys()):
        html = filename_to_html[fname]
        tee('writing {} to data dir'.format(fname))
        with open(get_dir_data() + '/' + fname, 'w') as f:
           f.write(html)

else:
    tee()
    tee('## Skipping html page creation')

#########################################################################
#### Debug: deploy_dir
#########################################################################

if args.debug_deploy:
    tee()
    tee("## Deploying data directory to linux.cs ...")
    tee(shell_to_str('cd ' + get_dir_tools() + ' ; ./deploy_dir.sh ' + get_dir_data() + ' uharness'))

#########################################################################
#### Finish
#########################################################################

ended = datetime.datetime.now()
ended_sec = int(shell_to_str('date +%s'))
elapsed_sec = ended_sec - started_sec

## Move old ZIP file(s) into backup directory

log(shell_to_str('if [ ! -e _output_backup ] ; then mkdir _output_backup 2>&1 ; fi'))
log(shell_to_str('mv output_*.zip _output_backup/'))

## ZIP results

zip_filename = ended.strftime('output_%yy%mm%dd_%Hh%Mm%Ss.zip')

tee()
tee('## Zipping results into ./{}'.format(zip_filename))
log()

reldir = os.path.relpath(g['replacements']['__dir_data'])
log_replace_and_run('zip -r {} {} output_log.txt *.py *.sqlite'.format(zip_filename, reldir))

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
print("Note: log can be viewed at ./output_log.txt")

g['log'].close()
