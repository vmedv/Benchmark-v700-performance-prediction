package contention.benchmark.workload.stop.condition;

import contention.benchmark.workload.Parameters;

import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicBoolean;

public class Timer implements StopCondition {
//    private long startTime;

//    private long endTime;

    private AtomicBoolean stop;

    public long workTime;

    public Timer() {
    }

    public Timer(long workTime) {
        this.workTime = workTime;
    }

    @Override
    public void start(Parameters parameters) {
        stop = new AtomicBoolean(false);
//        startTime = System.currentTimeMillis();
        java.util.Timer timer = new java.util.Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                stop.set(true);
            }
        }, workTime);
    }


//    public void waitStopCondition() {
//        try {
//            Thread.sleep(workTime);
//            stop.set(true);
//        } catch (InterruptedException e) {
//            throw new RuntimeException(e);
//        }
//    }

    @Override
    public boolean isStopped(int id) {
        return stop.get();
    }

    @Override
    public StringBuilder toStringBuilder() {
        //TODO toStringBuilder()
        return new StringBuilder();
    }
}
