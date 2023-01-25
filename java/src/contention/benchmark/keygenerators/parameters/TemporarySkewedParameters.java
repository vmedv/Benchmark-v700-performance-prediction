package contention.benchmark.keygenerators.parameters;

import contention.abstractions.Parameters;

import java.util.Arrays;

/**
 * n — { xi — yi — ti — rti } // либо   n — rt — { xi — yi — ti }
 * n — количество элементов
 * xi — процент элементов i-ого множества
 * yi — вероятность чтения элемента из i-ого множества
 * // 100% - yi — чтение остальных элементов
 * ti — время / количество итераций работы в режиме горячего вызова i-ого множества
 * rti / rt (relax time) — время / количество итераций работы в обычном режиме (равномерное распределение на все элементы)
 * // rt — если relax time всегда одинаковый
 * // rti — relax time после горячей работы с i-ым множеством
 */
public class TemporarySkewedParameters extends Parameters {
    public int setCount = 0;
    public double[] setSizes;
    public double[] hotProbs;
    public int[] hotTimes;
    public int[] relaxTimes;
    public int hotTime = -1;
    public int relaxTime = -1;

    public void setSetCount(final int setCount) {
        this.setCount = setCount;
        setSizes = new double[setCount];
        hotProbs = new double[setCount];
        hotTimes = new int[setCount];
        relaxTimes = new int[setCount];

        /*
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

    @Override
    public void parse(String[] args) {
        argNumber = 1;

        while (argNumber < args.length) {
            switch (args[argNumber]) {
                case "-set-count":
                    setSetCount(Integer.parseInt(args[++argNumber]));
                    break;
                case "-rt":
                    setCommonRelaxTime(Integer.parseInt(args[++argNumber]));
                    break;
                case "-ht":
                    setCommonHotTime(Integer.parseInt(args[++argNumber]));
                    break;
                case "-si":
                    setSetSize(Integer.parseInt(args[++argNumber]), Double.parseDouble(args[++argNumber]));
                    break;
                case "-pi":
                    setHotProb(Integer.parseInt(args[++argNumber]), Double.parseDouble(args[++argNumber]));
                    break;
                case "-hti":
                    setHotTime(Integer.parseInt(args[++argNumber]), Integer.parseInt(args[++argNumber]));
                    break;
                case "-rti":
                    setRelaxTimes(Integer.parseInt(args[++argNumber]), Integer.parseInt(args[++argNumber]));
                    break;
                default:
                    super.parse(args);
                    break;
            }

            argNumber++;
        }
    }

    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder params = super.toStringBuilder();
        params
                .append("\n")
                .append("  Number of Sets:          \t")
                .append(this.setCount)
                .append("\n")
                .append("  Sets sizes:              \t")
                .append("\n");
        for (int i = 0; i < setCount; i++) {
            params.append("    Set size ").append(i).append(":              \t")
                    .append(this.setSizes[i]).append("\n");
        }

        params
                .append("  Common Hot Time:         \t")
                .append(this.hotTime)
                .append("\n")
                .append("  Hot Times:               \t")
                .append("\n");
        for (int i = 0; i < setCount; i++) {
            params.append("    Hot Time ").append(i).append(":              \t")
                    .append(this.setSizes[i]).append("\n");
        }

        params
                .append("  Common Relax Time:       \t")
                .append(this.relaxTime)
                .append("\n")
                .append("  Relax Times:             \t")
                .append("\n");
        for (int i = 0; i < setCount; i++) {
            params.append("    Relax Time ").append(i).append(":            \t")
                    .append(this.setSizes[i]).append("\n");
        }

        return params;
    }
}
