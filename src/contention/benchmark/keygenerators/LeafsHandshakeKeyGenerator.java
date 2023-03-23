package contention.benchmark.keygenerators;

import contention.abstractions.*;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

import java.util.Random;

public class LeafsHandshakeKeyGenerator implements KeyGenerator {
    private final KeyGeneratorData readData;
    private final KeyGeneratorData eraseData;

    private final LeafsHandshakeParameters parameters;
    private final Distribution readDistribution;
    private final MutableDistribution insertDistribution;
    private final Distribution eraseDistribution;
    private final Random random;

    public LeafsHandshakeKeyGenerator(KeyGeneratorData readData,
                                      KeyGeneratorData eraseData,
                                      LeafsHandshakeParameters parameters,
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
    }

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

        if (localDeletedValue == 0 || (isRight && localDeletedValue != parameters.range - 1)) {
            value = localDeletedValue + insertDistribution.next(parameters.range - 1 - localDeletedValue) + 1;
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
