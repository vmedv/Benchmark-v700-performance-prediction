package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.abstractions.MutableDistribution;
import contention.benchmark.keygenerators.data.SimpleKeyGeneratorData;

import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

public class LeafsExtensionHandshakeKeyGenerator implements KeyGenerator {
    public static SimpleKeyGeneratorData readData = null;
    public static SimpleKeyGeneratorData eraseData = null;

    private final int range;
    private final MutableDistribution readDistribution;
    private final MutableDistribution insertDistribution;
    private final MutableDistribution eraseDistribution;
    private final Random random;

    public static AtomicInteger deletedValue = new AtomicInteger(0);
    public static AtomicInteger curRange;
    public int center;

    public LeafsExtensionHandshakeKeyGenerator(int range,
                                               MutableDistribution readDistribution,
                                               MutableDistribution insertDistribution,
                                               MutableDistribution eraseDistribution) {
        this.range = range;
        this.center = range / 2;
        this.curRange = new AtomicInteger(Math.max(10, range));
        this.readDistribution = readDistribution;
        this.insertDistribution = insertDistribution;
        this.eraseDistribution = eraseDistribution;
        this.random = new Random();
    }

    @Override
    public int nextRead() {
        int localCurRange = curRange.get();
        int index = readDistribution.next(localCurRange);
        int rawValue = readData == null ? index : readData.get(index);

        return center - localCurRange / 2 + rawValue;
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

        curRange.incrementAndGet();
        return value;
    }

    @Override
    public int nextErase() {
        int localDeletedValue = deletedValue.get();
        int localCurRange = curRange.get();

        int index = eraseDistribution.next(localCurRange);
        int rawValue = eraseData == null ? index : eraseData.get(index);
        int value = center - localCurRange / 2 + rawValue;

        //todo learn the difference between all kinds of weakCompareAndSet
        deletedValue.weakCompareAndSet(localDeletedValue, value);

        return value;
    }

    @Override
    public int nextPrefill() {
        curRange.incrementAndGet();
        return nextRead();
    }

}
