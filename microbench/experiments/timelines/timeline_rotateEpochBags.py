## define the following constants and functions for your input

class Constants:
    outfile="timeline_output_rotateEpochBags.png"

    maxIntervals=10000  ## cap on total number of intervals (to limit render time)
    maxThread=192       ## cap on number of threads rendered

    windowStart=0       ## only include intervals starting from x ms
    windowSize=60000    ## only include intervals for y ms after windowStart
    minDuration=20      ## min duration for an interval to be rendered

    ## should return the thread number for a given line
    def getThread(self, line):
        return long(line.split(" ")[0])

    ## should return the interval start for a given line
    def getStart(self, line):
        return long(line.split(" ")[1])

    ## should return the interval end for a given line
    def getEnd(self, line):
        return long(line.split(" ")[2])

    ## should return True if the line is to be included in the graph
    ## and False otherwise
    def includeLine(self, line):
        return True

from timeline_plot import TimelinePlotter
plot = TimelinePlotter(Constants())
