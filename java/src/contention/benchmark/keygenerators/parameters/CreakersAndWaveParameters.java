package contention.benchmark.keygenerators.parameters;

import contention.abstractions.DistributionBuilder;
import contention.abstractions.DistributionType;
import contention.abstractions.Parameters;
import contention.benchmark.distributions.parameters.ZipfParameters;

/**
 * старички + волна
 * n — g — gx — gk — cp — ca
 * n — количество элементов
 * g (grand) — процент старичков
 * // 100% - g — процент новичков
 * gy — вероятность их вызова
 * // 100% - gx — вероятность вызова новичков
 * gk — на сколько стары наши старички
 * | преподсчёт - перед началом теста делаем gk количество запросов к старичкам
 * cp (child) — распределение вызовов среди новичков
 * // по умолчанию Zipf 1
 * // при желании можно сделать cx — cy
 * ca (child add) — вероятность добавления нового элемента
 * // 100% - gx - ca — чтение новичка, ca добавление нового новичка
 * // при желании можно переработать на "100% - ca — чтение, ca — запись"
 * (то есть брать ca не от общей вероятности, а только при чтении"
 */
public class CreakersAndWaveParameters extends Parameters {
    public double CREAKERS_SIZE = 0;
    public double CREAKERS_PROB = 0;
    public long CREAKERS_AGE = 0;
    public double WAVE_SIZE = 0;
    public DistributionBuilder creakersDistBuilder = new DistributionBuilder();
    public DistributionBuilder waveDistBuilder = new DistributionBuilder(DistributionType.ZIPF)
            .setParameters(new ZipfParameters(1));

    @Override
    protected void parseArg() {
        switch (args[argNumber]) {
            case "-gs", "-cs" -> this.CREAKERS_SIZE = Double.parseDouble(args[++argNumber]);
            case "-gp", "-cp" -> this.CREAKERS_PROB = Double.parseDouble(args[++argNumber]);
            case "-ws" -> this.WAVE_SIZE = Double.parseDouble(args[++argNumber]);
            case "-g-age", "-c-age" -> this.CREAKERS_AGE = Integer.parseInt(args[++argNumber]);
            case "-g-dist", "-c-dist" -> argNumber = this.creakersDistBuilder.parseDistribution(args, argNumber);
            case "-w-dist" -> argNumber = this.waveDistBuilder.parseDistribution(args, argNumber);
            default -> super.parseArg();
        }
    }

    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder params = super.toStringBuilder();
        params
                .append("\n")
                .append("  Creakers size:           \t")
                .append(this.CREAKERS_SIZE)
                .append("\n")
                .append("  Wave size:               \t")
                .append(this.WAVE_SIZE)
                .append("\n")
                .append("  Creakers probability:    \t")
                .append(this.CREAKERS_PROB)
                .append("\n")
                .append("  Creakers age:            \t")
                .append(this.CREAKERS_AGE)
                .append("\n")
                .append("  Creakers distribution:   \t")
                .append(creakersDistBuilder.distributionType)
                .append(creakersDistBuilder.toStringBuilderParameters())
                .append("\n")
                .append("  Wave distribution:       \t")
                .append(waveDistBuilder.distributionType)
                .append(waveDistBuilder.toStringBuilderParameters());

        return params;
    }
}
