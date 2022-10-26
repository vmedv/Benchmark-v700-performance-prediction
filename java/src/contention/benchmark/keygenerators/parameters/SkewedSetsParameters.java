package contention.benchmark.keygenerators.parameters;

import contention.benchmark.Parameters;

public class SkewedSetsParameters extends Parameters {
    public static SkewedSetParameters READ = new SkewedSetParameters(0, 0);
    public static SkewedSetParameters WRITE = new SkewedSetParameters(0, 0);
    public static double INTERSECTION = 0;
}
