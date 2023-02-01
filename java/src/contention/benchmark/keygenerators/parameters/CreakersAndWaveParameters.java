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
    public DistributionBuilder creakersDist = new DistributionBuilder();
    public DistributionBuilder waveDist = new DistributionBuilder(DistributionType.ZIPF)
            .setParameters(new ZipfParameters(1));

    @Override
    protected void parseArg() {
        //todo
        super.parseArg();
    }

    @Override
    public StringBuilder toStringBuilder() {
        //todo
        return super.toStringBuilder();
    }
}
