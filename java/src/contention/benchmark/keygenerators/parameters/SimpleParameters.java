package contention.benchmark.keygenerators.parameters;

import contention.abstractions.DistributionType;
import contention.benchmark.Parameters;

public class SimpleParameters extends Parameters {
    public static DistributionType distributionType = DistributionType.UNIFORM;
    public static double zipfParm;
    public static SkewedSetParameters skewedSetParameters = new SkewedSetParameters(0, 0);
}
