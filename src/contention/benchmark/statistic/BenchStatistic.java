package contention.benchmark.statistic;

import static contention.benchmark.tools.StringFormat.formatDouble;

public class BenchStatistic {
    public double throughput;
    //    public double averageTraversalLength;
    public long effectiveUpdates;
    public long finalSize;
    /**
     * #nodes (inc. deleted)
     * if the data structure is not instance of MaintenanceAlg, the numNodes = -1
     */
    public long numNodes = -1;
    public ThreadStatistic[] threadStatistics;
    public ThreadStatistic commonStatistic;
    public double elapsedTime;
    public long prefillSize;

    public String toString() {
        //        int finalSize = dataStructure.size();
        //        curBenchStats.finalSize = finalSize;
        StringBuilder stringBuilder = new StringBuilder()
//                .append("Benchmark statistics")
                .append("  Average traversal length: \t")
                .append((double) commonStatistic.nodesTraversed / (double) commonStatistic.getCount)
                .append("\n")
                .append("  Struct Modifications:     \t")
                .append(commonStatistic.structMods)
                .append("\n")
                .append("  Throughput (ops/s):       \t")
                .append(throughput)
                .append("\n")
                .append("  Elapsed time (s):         \t")
                .append(elapsedTime)
                .append("\n")
                .append("  Operations:               \t")
                .append(commonStatistic.total)
                .append("\t( 100 %)")
                .append("\n")
                .append("    effective updates:     \t")
                .append(commonStatistic.numAdd + commonStatistic.numRemove
                        + commonStatistic.numAddAll + commonStatistic.numRemoveAll)
                .append("\t( ")
                .append(formatDouble(((double) (commonStatistic.numAdd + commonStatistic.numRemove
                        + commonStatistic.numAddAll + commonStatistic.numRemoveAll) * 100)
                        / (double) commonStatistic.total))
                .append(" %)")
                .append("\n")
                .append("    |--add successful:     \t")
                .append(commonStatistic.numAdd)
                .append("\t( ")
                .append(formatDouble(((double) commonStatistic.numAdd / (double) commonStatistic.total) * 100))
                .append(" %)")
                .append("\n")
                .append("    |--remove succ.:       \t")
                .append(commonStatistic.numRemove)
                .append("\t( ")
                .append(formatDouble(((double) commonStatistic.numRemove / (double) commonStatistic.total) * 100))
                .append(" %)")
                .append("\n")
                .append("    |--addAll succ.:       \t")
                .append(commonStatistic.numAddAll)
                .append("\t( ")
                .append(formatDouble(((double) commonStatistic.numAddAll / (double) commonStatistic.total) * 100))
                .append(" %)")
                .append("\n")
                .append("    |--removeAll succ.:    \t")
                .append(commonStatistic.numRemoveAll)
                .append("\t( ")
                .append(formatDouble(((double) commonStatistic.numRemoveAll / (double) commonStatistic.total) * 100))
                .append(" %)")
                .append("\n")
                .append("    size successful:       \t")
                .append(commonStatistic.numSize)
                .append("\t( ")
                .append(formatDouble(((double) commonStatistic.numSize / (double) commonStatistic.total) * 100))
                .append(" %)")
                .append("\n")
                .append("    contains succ.:        \t")
                .append(commonStatistic.numContains)
                .append("\t( ")
                .append(formatDouble(((double) commonStatistic.numContains / (double) commonStatistic.total) * 100))
                .append(" %)")
                .append("\n")
                .append("    unsuccessful ops:      \t")
                .append(commonStatistic.failures)
                .append("\t( ")
                .append(formatDouble(((double) commonStatistic.failures / (double) commonStatistic.total) * 100))
                .append(" %)")
                .append("\n")
                .append("  Final size:              \t")
                .append(finalSize)
                .append("\n");

        if (numNodes != -1) {
            stringBuilder
                    .append("  #nodes (inc. deleted): \t")
                    .append(numNodes)
                    .append("\n");
        }

        return stringBuilder.toString();
    }

}
