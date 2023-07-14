package contention.benchmark.tools;

import java.util.Formatter;
import java.util.Locale;

public class StringFormat {
    private StringFormat(){}

    public static String formatDouble(double result) {
        Formatter formatter = new Formatter(Locale.US);
        return formatter.format("%.2f", result).out().toString();
    }
}
