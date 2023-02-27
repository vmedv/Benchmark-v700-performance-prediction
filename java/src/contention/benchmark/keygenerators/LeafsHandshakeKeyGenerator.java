package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.abstractions.MutableDistribution;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.data.SimpleKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

public class LeafsHandshakeKeyGenerator implements KeyGenerator {
    public static SimpleKeyGeneratorData readData = null;
    public static SimpleKeyGeneratorData eraseData = null;

    private final int range;
    private final Distribution readDistribution;
    private final MutableDistribution insertDistribution;
    private final Distribution eraseDistribution;
    private final Random random;

    public static AtomicInteger deletedValue = new AtomicInteger(0);

    public LeafsHandshakeKeyGenerator(int range,
                                      Distribution readDistribution,
                                      MutableDistribution insertDistribution,
                                      Distribution eraseDistribution) {
        this.range = range;
        this.readDistribution = readDistribution;
        this.insertDistribution = insertDistribution;
        this.eraseDistribution = eraseDistribution;
        this.random = new Random();
    }

    @Override
    public int nextRead() {
        int index = readDistribution.next();
        return readData == null ? index : readData.get(index);
    }

    @Override
    public int nextInsert() {
        int localDeletedValue = deletedValue.get();

        int value;

        boolean isRight = random.nextDouble() >= 0.5;

        if (localDeletedValue == 0 || (isRight && localDeletedValue != range - 1)) {
            value = localDeletedValue + insertDistribution.next(range - localDeletedValue) + 1;
        } else {
            value = localDeletedValue - insertDistribution.next(localDeletedValue) - 1;
        }

        return value;
    }

    @Override
    public int nextErase() {
        int localDeletedValue = deletedValue.get();
        int index = eraseDistribution.next();
        int value = eraseData == null ? index : eraseData.get(index);

        //todo learn the difference between all kinds of weakCompareAndSet
        deletedValue.weakCompareAndSet(localDeletedValue, value);

        return value;
    }

    @Override
    public int nextPrefill() {
        return nextRead();
    }

}
