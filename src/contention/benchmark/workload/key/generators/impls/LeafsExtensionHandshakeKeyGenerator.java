package contention.benchmark.workload.key.generators.impls;

import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.key.generators.abstractions.KeyGenerator;
import contention.benchmark.workload.key.generators.parameters.LeafsHandshakeParameters;
import contention.benchmark.workload.data.map.abstractions.DataMap;
import contention.benchmark.workload.distributions.abstractions.MutableDistribution;

import java.util.Random;

public class LeafsExtensionHandshakeKeyGenerator implements KeyGenerator {
    private final DataMap readData;
    private final DataMap eraseData;
    private final LeafsHandshakeParameters parameters;

    private final MutableDistribution readDistribution;
    private final MutableDistribution insertDistribution;
    private final MutableDistribution eraseDistribution;
    private final Random random;
    private final int range;


    public int center;

    public LeafsExtensionHandshakeKeyGenerator(DataMap readData,
                                               DataMap eraseData,
                                               LeafsHandshakeParameters parameters,
                                               int range,
                                               MutableDistribution readDistribution,
                                               MutableDistribution insertDistribution,
                                               MutableDistribution eraseDistribution) {
        this.readData = readData;
        this.eraseData = eraseData;
        this.parameters = parameters;
        this.center = range / 2;
        this.readDistribution = readDistribution;
        this.insertDistribution = insertDistribution;
        this.eraseDistribution = eraseDistribution;
        this.random = new Random();
        this.range = range;
    }

    public LeafsExtensionHandshakeKeyGenerator(DataMap readData,
                                               DataMap eraseData,
                                               LeafsHandshakeParameters parameters,
                                               int range) {
        this.readData = readData;
        this.eraseData = eraseData;
        this.parameters = parameters;
        this.center = range / 2;
        //TODO mutable dists
        this.readDistribution = ((MutableDistributionBuilder) parameters.readDistBuilder).build();
        this.insertDistribution = parameters.insertDistBuilder.build();
        this.eraseDistribution = ((MutableDistributionBuilder) parameters.removeDistBuilder).build();
        this.random = new Random();
        this.range = range;
    }

//    public LeafsExtensionHandshakeKeyGenerator(LeafsHandshakeParameters parameters,
//                                               int range) {
//        this.readData = parameters.readDataMapBuilder.build(range);
//        this.eraseData = parameters.eraseDataMapBuilder.build(range);
//        this.parameters = parameters;
//        this.center = range / 2;
//        this.readDistribution = parameters.readDistBuilder.build();
//        this.insertDistribution = parameters.insertDistBuilder.build();
//        this.eraseDistribution = parameters.removeDistBuilder.build();
//        this.random = new Random();
//        this.range = range;
//    }

    @Override
    public int nextGet() {
        int localCurRange = parameters.curRange.get();
        int index = readDistribution.next(localCurRange);
        int rawValue = readData == null ? index : readData.get(index);

        return center - localCurRange / 2 + rawValue;
    }

    @Override
    public int nextInsert() {
        int localDeletedValue = parameters.deletedValue.get();

        int value;

        boolean isRight = random.nextDouble() >= 0.5;

        if (localDeletedValue == 0 || (isRight && localDeletedValue != range - 1)) {
            value = localDeletedValue + insertDistribution.next(range - 1 - localDeletedValue) + 1;
        } else {
            value = localDeletedValue - insertDistribution.next(localDeletedValue) - 1;
        }

        parameters.curRange.incrementAndGet();
        return value;
    }

    @Override
    public int nextRemove() {
        int localDeletedValue = parameters.deletedValue.get();
        int localCurRange = parameters.curRange.get();

        int index = eraseDistribution.next(localCurRange);
        int rawValue = eraseData == null ? index : eraseData.get(index);
        int value = center - localCurRange / 2 + rawValue;

        //todo learn the difference between all kinds of weakCompareAndSet
        parameters.deletedValue.weakCompareAndSet(localDeletedValue, value);

        return value;
    }

    public int nextPrefill() {
        parameters.curRange.incrementAndGet();
        return nextGet();
    }

}
