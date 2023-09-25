package contention.benchmark.tools;

import java.util.Formatter;
import java.util.Locale;

public class StringFormat {
    private StringFormat() {
    }

    public static String formatDouble(double result) {
        Formatter formatter = new Formatter(Locale.US);
        return formatter.format("%.2f", result).out().toString();
    }

    public static String indentedTitle(String title, int indents, int indentLength) {
        return " ".repeat(indentLength * indents) + title + ":\n";
    }

    public static String indentedTitle(String title, int indents) {
        return indentedTitle(title, indents, 2);
    }

    public static String indentedTitle(String title) {
        return indentedTitle(title, 1);
    }

    public static <T> String indentedTitleWithData(String title, T data, int indents, int lineLength, int indentLength) {
        return " ".repeat(indentLength * indents) + title + ":"
                + (title.length() + indents * indentLength < lineLength
                ? " ".repeat(lineLength - title.length() - indentLength * indents) : "\t")
                + data + "\n";
    }

    public static <T> String indentedTitleWithData(String title, T data, int indents, int lineLength) {
        return indentedTitleWithData(title, data, indents, lineLength, 2);
    }

    public static <T> String indentedTitleWithData(String title, T data, int indents) {
        return indentedTitleWithData(title, data, indents, 28);
    }

    public static <T> String indentedTitleWithData(String title, T data) {
        return indentedTitleWithData(title, data, 1);
    }

    public static <T> String indentedTitleWithDataPercent(String title, T data, double percent,
                                                          int indents, int lineLength, int indentLength) {
        return " ".repeat(indentLength * indents) + title + ":"
                + (title.length() + indents * indentLength < lineLength
                ? " ".repeat(lineLength - title.length() - indentLength * indents) : "\t")
                + data + "\t( " + formatDouble(percent * 100) + " %)" + "\n";
    }

    public static <T> String indentedTitleWithDataPercent(String title, T data, double percent,
                                                          int indents, int lineLength) {
        return indentedTitleWithDataPercent(title, data, percent, indents, lineLength, 2);
    }

    public static <T> String indentedTitleWithDataPercent(String title, T data, double percent, int indents) {
        return indentedTitleWithDataPercent(title, data, percent, indents, 28);
    }

    public static <T> String indentedTitleWithDataPercent(String title, T data, double percent) {
        return indentedTitleWithDataPercent(title, data, percent, 1);
    }

    public static String getStringStage(String stageName) {
        return getLine() + "\n" + stageName + "\n" + getLine() + "\n";
    }


    public static String getLine() {
        return String.valueOf('-').repeat(80);
    }
}
