package contention.benchmark.keygenerators.parameters;

import contention.benchmark.Parameters;

import java.util.Arrays;

/**
    n — { xi — yi — ti — rti } // либо   n — rt — { xi — yi — ti }
        n — количество элементов
        xi — процент элементов i-ого множества
        yi — вероятность чтения элемента из i-ого множества
            // 100% - yi — чтение остальных элементов
        ti — время / количество итераций работы в режиме горячего вызова i-ого множества
        rti / rt (relax time) — время / количество итераций работы в обычном режиме (равномерное распределение на все элементы)
            // rt — если relax time всегда одинаковый
            // rti — relax time после горячей работы с i-ым множеством
 */
public class TemporarySkewedParameters extends Parameters {
    public static int setCount = 0;
    public static double[] setSizes;
    public static double[] hotProbs;
    public static int[] hotTimes;
    public static int[] relaxTimes;
    public static int hotTime = -1;
    public static int relaxTime = -1;

    public void setSetCount(final int setCount) {
        this.setCount = setCount;
        setSizes = new double[setCount];
        hotProbs = new double[setCount];
        hotTimes = new int[setCount];
        relaxTimes = new int[setCount];

        /**
         * if hotTimes[i] == -1, we will use hotTime
         * relaxTime analogically
         */
        Arrays.fill(hotTimes, hotTime);
        Arrays.fill(relaxTimes, relaxTime);
    }

    public void setSetSize(final int i, final double setSize) {
        assert (i < setCount);
        setSizes[i] = setSize;
    }

    public void setHotProb(final int i, final double hotProb) {
        assert (i < setCount);
        hotProbs[i] = hotProb;
    }

    public void setHotTime(final int i, final int hotTime) {
        assert (i < setCount);
        hotTimes[i] = hotTime;
    }

    public void setRelaxTimes(final int i, final int relaxTime) {
        assert (i < setCount);
        relaxTimes[i] = relaxTime;
    }

    public void setCommonHotTime(final int hotTime) {
        this.hotTime = hotTime;

        for (int i = 0; i < setCount; ++i) {
            if (hotTimes[i] == -1) {
                hotTimes[i] = this.hotTime;
            }
        }
    }

    public void setCommonRelaxTime(final int relaxTime) {
        this.relaxTime = relaxTime;

        for (int i = 0; i < setCount; ++i) {
            if (relaxTimes[i] == -1) {
                relaxTimes[i] = this.relaxTime;
            }
        }
    }
}
