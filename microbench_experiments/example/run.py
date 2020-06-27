#!/usr/bin/python3

##
## Possible improvements
##
## - for debugging / perf debugging, full dump of all data (all columns) in addition to the simple plot_txt_file... (as well as concatenated run logs...)
## - parallel plot construction
## - web page generation
## - plotting sanity check: should be able to predict # of rows per data point (exactly number of trials)... more rows suggests you've missed some run_param fields that are varying (so you're aggregating over a bad group-by)...
##
## - line plots with proper markers and styling
## - stacked bar plots
##
## - "clean" version of example without comments to show that it's still clear, but also way more concise...
## - default-off optional web page deploy (really it's a single shell call, and i can just do that in my own non-example scripts, with host detection first...)
## - test framework on a different user (proper independence from global tools namespace?)
##
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
            total_est_sec = elapsed_sec - remaining_sec
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

            quote = ('"' if (g['data_fields'][field]['coltype'].upper() == 'TEXT') else '')
            values += '{}{}{}{}'.format(('' if values == '' else ', '), quote, value, quote)

        txn = txn.format(fields, values)
        log()
        log(txn)
        cur.execute(txn)

    con.commit()

#########################################################################
#### Produce plots
#########################################################################

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

        ## for each distinct assignment
        for varying_cols_rowstr in varying_cols_records:
            log()
            log()
            # tee("varying_cols_rowstr={}".format(varying_cols_rowstr))

            ## build a dict for this assignment
            varying_cols_vals = dict()
            for i in range(len(varying_cols_rowstr)):
                varying_cols_vals[plot_set['varying_cols_list'][i]] = varying_cols_rowstr[i]
                g['replacements'][plot_set['varying_cols_list'][i]] = varying_cols_rowstr[i]
            log("varying_cols_vals={}".format(varying_cols_vals))

            ## create new where clause (including these varying_cols_vals in the filter)
            log('prelimiary where_clause={}'.format(where_clause))
            where_clause_new = where_clause
            if where_clause_new.strip() == '':
                where_clause_new = 'WHERE (1==1)'
            for key in varying_cols_vals.keys():
                quote = ('"' if g['data_fields'][key]['coltype'].upper() == 'TEXT' else '')
                where_clause_new += ' AND {} == {}{}{}'.format(key, quote, varying_cols_vals[key], quote)
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

            ## now we have the data we want to plot...
            ## write it to a text file (so we can feed it into a plot command)
            pp_log.pprint(data_records)

            plot_filename = g['replacements']['__dir_data'] + '/' + replace(plot_set['filename']).replace(' ', '_')
            plot_txt_filename = plot_filename.replace('.png', '.txt')

            g['replacements']['__plot_filename'] = plot_filename
            g['replacements']['__plot_txt_filename'] = plot_txt_filename

            tee('plot {}'.format(os.path.relpath(plot_filename)))
            log('plot_filename={}'.format(plot_filename))
            log('plot_txt_filename={}'.format(plot_txt_filename))

            plot_txt_file = open(plot_txt_filename, 'w')
            for row in data_records:
                for col in row:
                    if col != row[0]: plot_txt_file.write(' ')
                    plot_txt_file.write(str(col))
                plot_txt_file.write('\n')
            plot_txt_file.close()

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
            log_replace_and_run('{__plot_tool_path} -i {__plot_txt_filename} -o {__plot_filename} "{__title}" {__plot_cmd_args}')
else:
    tee()
    tee("## Skipping plot creation")

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

g['log'].close()
