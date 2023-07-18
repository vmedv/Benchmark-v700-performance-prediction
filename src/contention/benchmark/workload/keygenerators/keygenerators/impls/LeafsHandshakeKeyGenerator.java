package contention.benchmark.workload.keygenerators.keygenerators.impls;

import contention.benchmark.workload.keygenerators.keygenerators.parameters.LeafsHandshakeParameters;
import contention.benchmark.workload.datamap.abstractions.DataMap;
import contention.benchmark.workload.distributions.abstractions.Distribution;
import contention.benchmark.workload.distributions.abstractions.MutableDistribution;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGenerator;

import java.util.Random;

public class LeafsHandshakeKeyGenerator implements KeyGenerator {
    private final DataMap readData;
    private final DataMap eraseData;

    private final LeafsHandshakeParameters parameters;
    private final Distribution readDistribution;
    private final MutableDistribution insertDistribution;
    private final Distribution eraseDistribution;
    private final Random random;
    private final int range;

    public LeafsHandshakeKeyGenerator(DataMap readData,
                                      DataMap eraseData,
                                      LeafsHandshakeParameters parameters,
                                      int range,
                                      Distribution readDistribution,
                                      MutableDistribution insertDistribution,
                                      Distribution eraseDistribution) {
        this.readData = readData;
        this.eraseData = eraseData;
        this.parameters = parameters;
        this.readDistribution = readDistribution;
        this.insertDistribution = insertDistribution;
        this.eraseDistribution = eraseDistribution;
        this.random = new Random();
        this.range = range;
    }

    public LeafsHandshakeKeyGenerator(DataMap readData,
                                      DataMap eraseData,
                                      LeafsHandshakeParameters parameters,
                                      int range) {
        this.readData = readData;
        this.eraseData = eraseData;
        this.parameters = parameters;
        this.range = range;
        this.readDistribution = parameters.readDistBuilder.build(range);
        this.insertDistribution = parameters.insertDistBuilder.build();
        this.eraseDistribution = parameters.removeDistBuilder.build(range);
        this.random = new Random();
    }

//    public LeafsHandshakeKeyGenerator(LeafsHandshakeParameters parameters,
//                                      int range) {
////        this.readData = parameters.readDataMapBuilder.build(range);
////        this.eraseData = parameters.eraseDataMapBuilder.build(range);
//        this.parameters = parameters;
//        this.range = range;
//        this.readDistribution = parameters.readDistBuilder.build(range);
//        this.insertDistribution = parameters.insertDistBuilder.build();
//        this.eraseDistribution = parameters.removeDistBuilder.build(range);
//        this.random = new Random();
//    }

    @Override
    public int nextGet() {
        int index = readDistribution.next();
        return readData.get(index);
    }

    @Override
    public int nextInsert() {
        int localDeletedValue = parameters.deletedValue.get();

        int value;

        boolean isRight = random.nextDouble() >= 0.5;

        if (localDeletedValue == 1 || (isRight && localDeletedValue != range)) {
            value = localDeletedValue + insertDistribution.next(range - 1 - localDeletedValue) + 1;
        } else {
            value = localDeletedValue - insertDistribution.next(localDeletedValue) - 1;
        }

        return value;
    }

    @Override
    public int nextRemove() {
        int localDeletedValue = parameters.deletedValue.get();
        int index = eraseDistribution.next();
        int value = eraseData.get(index);

        //todo learn the difference between all kinds of weakCompareAndSet
        parameters.deletedValue.weakCompareAndSet(localDeletedValue, value);

        return value;
    }

    @Override
    public int nextPrefill() {
        return nextGet();
    }

}
