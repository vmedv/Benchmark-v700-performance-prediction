## no need to edit below here

import matplotlib.pyplot as plt
import fileinput
from timeit import default_timer as timer

class TimelinePlotter:
    scale=1000000 # nanos to millis
    df = []
    mintime=-1
    maxtime=0
    windowEnd=0

    start_split=0
    last_split=0

    def split(self):
        curr_split = timer()
        diff = curr_split - self.last_split
        difftotal = curr_split - self.start_split
        print("elapsed %.1fs (total %.1fs)" % (diff, difftotal))
        self.last_split = curr_split

    def __init__(self, settings):
        self.windowEnd = settings.windowStart + settings.windowSize
        self.start_split = timer()
        self.last_split = timer()

        ## compute min and max start time

        for line in fileinput.input():
            start = settings.getStart(line)
            end = settings.getEnd(line)

            if self.mintime == -1 or self.mintime > start:
                self.mintime = start
            if self.maxtime < start:
                self.maxtime = start

        print "found min time " + repr(self.mintime) + " and max time " + repr(self.maxtime) + " duration " + repr((self.maxtime - self.mintime) / self.scale) + " ms"
        self.split()

        ## build data frame (process all lines)

        count=0
        for line in fileinput.input():
            if settings.includeLine(line) == True:
                thr = settings.getThread(line)
                start = settings.getStart(line) - self.mintime
                end = settings.getEnd(line) - self.mintime
                duration = end - start

                if duration >= settings.minDuration*self.scale:
                    if thr <= settings.maxThread:
                        if (start >= settings.windowStart*self.scale and start <= self.windowEnd*self.scale) or (end >= settings.windowStart*self.scale and end <= self.windowEnd*self.scale):
                            self.df.append(dict(Task=thr, Start=start, Finish=end))

        print "built data frame; len=" + repr(len(self.df))
        self.split()

        if len(self.df) < settings.maxIntervals:
            yesno = raw_input("Do you want to build the graph [y/n]? ")
            if yesno == 'y' or yesno == 'Y':

                plt.figure(figsize=(16,9), dpi=200)

                self.split()
                print "start drawing hlines"
                lineno=0
                for row in self.df:
                    lineno = lineno + 1
                    if (lineno % 1000) == 0:
                        print "line number " + repr(lineno)
                        self.split()
                    plt.hlines(int(row["Task"]), long(row["Start"]), long(row["Finish"]), lw=1)
                    ## https://datascience.stackexchange.com/questions/33237/how-to-create-a-historical-timeline-using-pandas-dataframe-and-matplotlib
                print "finished drawing hlines"
                self.split()

                print "saving figure"
                plt.savefig(settings.outfile, Format="png")
                self.split()

                print "showing figure"
                plt.show()

        else:
            print "df too large to render in a reasonable time"

        self.split()
