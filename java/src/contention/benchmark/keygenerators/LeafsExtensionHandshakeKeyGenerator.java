package contention.benchmark.keygenerators;

import contention.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.abstractions.MutableDistribution;
import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

import java.util.Random;

public class LeafsExtensionHandshakeKeyGenerator implements KeyGenerator {
    private final KeyGeneratorData readData;
    private final KeyGeneratorData eraseData;
    private final LeafsHandshakeParameters parameters;

    private final MutableDistribution readDistribution;
    private final MutableDistribution insertDistribution;
    private final MutableDistribution eraseDistribution;
    private final Random random;


    public int center;

    public LeafsExtensionHandshakeKeyGenerator(KeyGeneratorData readData,
                                               KeyGeneratorData eraseData,
                                               LeafsHandshakeParameters parameters,
                                               MutableDistribution readDistribution,
                                               MutableDistribution insertDistribution,
                                               MutableDistribution eraseDistribution) {
        this.readData = readData;
        this.eraseData = eraseData;
        this.parameters = parameters;
        this.center = parameters.range / 2;
        this.readDistribution = readDistribution;
        this.insertDistribution = insertDistribution;
        this.eraseDistribution = eraseDistribution;
        this.random = new Random();
    }

    @Override
    public int nextRead() {
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

        if (localDeletedValue == 0 || (isRight && localDeletedValue != parameters.range - 1)) {
            value = localDeletedValue + insertDistribution.next(parameters.range - localDeletedValue) + 1;
        } else {
            value = localDeletedValue - insertDistribution.next(localDeletedValue) - 1;
        }

        parameters.curRange.incrementAndGet();
        return value;
    }

    @Override
    public int nextErase() {
        int localDeletedValue = parameters.deletedValue.get();
        int localCurRange = parameters.curRange.get();

        int index = eraseDistribution.next(localCurRange);
        int rawValue = eraseData == null ? index : eraseData.get(index);
        int value = center - localCurRange / 2 + rawValue;

        //todo learn the difference between all kinds of weakCompareAndSet
        parameters.deletedValue.weakCompareAndSet(localDeletedValue, value);

        return value;
    }

    @Override
    public int nextPrefill() {
        parameters.curRange.incrementAndGet();
        return nextRead();
    }

}
