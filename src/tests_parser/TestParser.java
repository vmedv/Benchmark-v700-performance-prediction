package tests_parser;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;

public class TestParser {
    private static String folderName = "C:\\Users\\Huawei\\OneDrive\\Документы\\ИТМО\\LAB\\synchrobench\\java\\synchrobench_tests_results\\";
    private static String[] algoNames = new String[]{
            "trees.lockbased.LockBasedFriendlyTreeMap",
            "trees.lockbased.LockBasedStanfordTreeMap",
            "trees.lockbased.FixedLockBasedFriendlyTreeMap"
            ,            "trees.lockbased.ConcurrencyOptimalTreeMap"
    };

    private static String[] algoNames2 = new String[]{
            "LockBasedFriendlyTreeMap",
            "LockBasedStanfordTreeMap",
            "FixedLockBasedFriendlyTreeMap"
            ,            "ConcurrencyOptimalTreeMap"
    };
    private static String[] shortAlgoNames = new String[]{"Friendly", "Stanford", "FixedFriendly"
            , "Optimal"
    };

    private static <K, V> Map<V, K> reverse(Map<K, V> map) {
        Map<V, K> reversedMap = new HashMap<>();

        for (Map.Entry<K, V> entry : map.entrySet()) {
            reversedMap.put(entry.getValue(), entry.getKey());
        }
        return reversedMap;
    }

    public static void oldMain(String[] args) throws IOException {
        for (String algoName : algoNames) {
            File folder = new File(folderName + algoName);

            File[] testResults = folder.listFiles();

//            BufferedWriter writer = Files.newBufferedWriter(Paths.get((folderName + "\\tables\\" + algoName + "_uniform.txt")));
//            BufferedWriter zipfWriter = Files.newBufferedWriter(Paths.get((folderName + "\\tables\\" + algoName + "_zipf.txt")));
            BufferedWriter writerTemp2 = Files.newBufferedWriter(Paths.get((folderName + "\\tables\\" + algoName + "_temp2.txt")));
            BufferedWriter writerTemp4 = Files.newBufferedWriter(Paths.get((folderName + "\\tables\\" + algoName + "_temp4.txt")));

            String stats = "Number of threads\tWrite ratio\tAverage traversal length\tThroughput (ops/s)\tOperations\n";
//            writer.write(stats);
//            zipfWriter.write(stats);
            writerTemp2.write(stats);
            writerTemp4.write(stats);

            assert testResults != null;
            for (File testResult : testResults) {
//                BufferedReader input = Files.newBufferedReader(testResult.toPath());
//                FileInputStream inputStream = new FileInputStream(testResult);
                Scanner input = new Scanner(testResult);

                String numberOfThreads = null;
                String writeRatio = null;
                String averageLength = null;
                String throughput = null;
                String operations = null;
                boolean isZipf = false;
                boolean isTemp2 = false;
                boolean isTemp4 = false;

                while (input.hasNextLine()) {
                    String line = input.nextLine().trim();

                    if (line.startsWith("Number of threads:")) {
                        numberOfThreads = line.replace("Number of threads:", "").trim();
                    } else if (line.startsWith("Write ratio:")) {
                        writeRatio = line.replace("Write ratio:", "").replace("%", "").trim();
                    } else if (line.startsWith("Average traversal length:")) {
                        averageLength = line.replace("Average traversal length:", "").replace(".", ",").trim();
                    } else if (line.startsWith("Throughput (ops/s):")) {
                        throughput = line.replace("Throughput (ops/s):", "").replace(".", ",").trim();
                    } else if (line.startsWith("Operations:")) {
                        operations = line.replace("Operations:", "").replace("( 100 %)", "").trim();
                    } else if (line.startsWith("Distribution:")) {
                        isZipf = line.endsWith("ZIPF");
                    } else if (line.startsWith("Number of Sets:")) {
                        String numOfSets = line.replace("Number of Sets:", "").trim();
                        if (numOfSets.equals("2")) {
                            isTemp2 = true;
                        } else {
                            isTemp4 = true;
                        }
                    }
                }

                String result = numberOfThreads + "\t" + writeRatio + "\t" + averageLength + "\t" + throughput + "\t" + operations + "\n";

                if (isZipf) {
//                    zipfWriter.write(result);
//
                } else if (isTemp2) {
                    writerTemp2.write(result);
                } else if (isTemp4) {
                    writerTemp4.write(result);
                } else {
//                    writer.write(result);
                }
            }
//            writer.close();
//            zipfWriter.close();
            writerTemp2.close();
            writerTemp4.close();
        }
    }

    public static void oldMain2(String[] args) throws IOException {
        String[][][][] table = new String[8][4][8][2]; // [workload][write ratio][num of threads][algo][throughput]
        Map<String, Integer> workloads = new HashMap<>();
        Map<String, Integer> writeRatios = new HashMap<>();
        Map<String, Integer> numberOfThreadss = new HashMap<>();

        workloads.put("UNIFORM", 0);
        workloads.put("ZIPF", 1);
        workloads.put("TEMP2 ht 10^4", 2);
        workloads.put("TEMP4 ht 10^4", 3);
        workloads.put("TEMP2 ht 10^5", 4);
        workloads.put("TEMP4 ht 10^5", 5);
        workloads.put("TEMP2 ht 10^6", 6);
        workloads.put("TEMP4 ht 10^6", 7);

        writeRatios.put("0", 0);
        writeRatios.put("2", 1);
        writeRatios.put("10", 2);
        writeRatios.put("20", 3);

        numberOfThreadss.put("1", 0);
        numberOfThreadss.put("2", 1);
        numberOfThreadss.put("4", 2);
        numberOfThreadss.put("8", 3);
        numberOfThreadss.put("16", 4);
        numberOfThreadss.put("24", 5);
        numberOfThreadss.put("36", 6);
        numberOfThreadss.put("48", 7);

        Map<Integer, String> workloadsRev = reverse(workloads);
        Map<Integer, String> writeRatiosRev = reverse(writeRatios);
        Map<Integer, String> numberOfThreadssRev = reverse(numberOfThreadss);


        for (int al = 0; al < 2; al++) {
            String algoName = algoNames[al];
            File folder = new File(folderName + algoName);

            File[] testResults = folder.listFiles();


            assert testResults != null;
            for (File testResult : testResults) {
                Scanner input = new Scanner(testResult);

                String numberOfThreads = null;
                String writeRatio = null;
                String throughput = null;
                String workload = null;

                while (input.hasNextLine()) {
                    String line = input.nextLine().trim();

                    if (line.startsWith("Number of threads:")) {
                        numberOfThreads = line.replace("Number of threads:", "").trim();
                    } else if (line.startsWith("Write ratio:")) {
                        writeRatio = line.replace("Write ratio:", "").replace("%", "").trim();
                    } else if (line.startsWith("Throughput (ops/s):")) {
                        throughput = line.replace("Throughput (ops/s):", "").replace(".", ",").trim();
                    } else if (line.startsWith("Workload:")) {
                        workload = line.replace("Workload:", "").trim();
                    } else if (line.startsWith("Distribution:")) {
                        if (line.endsWith("ZIPF")) {
                            workload = "ZIPF";
                        } else if (line.endsWith("UNIFORM")) {
                            workload = "UNIFORM";
                        }
                    } else if (line.startsWith("Number of Sets:")) {
                        String numOfSets = line.replace("Number of Sets:", "").trim();
                        if (numOfSets.equals("2")) {
                            workload = "TEMP2 ";
                        } else if (numOfSets.equals("4")) {
                            workload = "TEMP4 ";
                        }
                    } else if (line.startsWith("Common Hot Time:")) {
                        String numOfSets = line.replace("Common Hot Time:", "").trim();
                        switch (numOfSets) {
                            case "10000" -> workload += "ht 10^4";
                            case "100000" -> workload += "ht 10^5";
                            case "1000000" -> workload += "ht 10^6";
                        }
                    }
                }

                try {
                    table[workloads.get(workload)]
                            [writeRatios.get(writeRatio)]
                            [numberOfThreadss.get(numberOfThreads)]
                            [al] = throughput;
                } catch (Exception e) {
                    throw e;
                }
            }

            try (BufferedWriter writer = Files.newBufferedWriter(Paths.get((folderName + "\\tables\\FullTable.txt")))) {

                writer.write("\t" + shortAlgoNames[0] + "\t" + shortAlgoNames[1] + "\n");

                for (int workloadIndex = 0; workloadIndex < workloads.size(); workloadIndex++) {

                    String workload = workloadsRev.get(workloadIndex);
                    for (int writeRatioIndex = 0; writeRatioIndex < writeRatios.size(); writeRatioIndex++) {
                        String writeRatio = writeRatiosRev.get(writeRatioIndex);

                        writer.write(workload + " wr-" + writeRatio + "\n");

                        for (int numberOfThreadsIndex = 0; numberOfThreadsIndex < numberOfThreadss.size(); numberOfThreadsIndex++) {
                            String numberOfThreads = numberOfThreadssRev.get(numberOfThreadsIndex);
                            writer.write(numberOfThreads
                                    + "\t" + table[workloadIndex][writeRatioIndex][numberOfThreadsIndex][0]
                                    + "\t" + table[workloadIndex][writeRatioIndex][numberOfThreadsIndex][1]
                                    + "\n");
                        }
                    }
                    writer.write("\n");
                }

            }
        }
    }

    public static void leafDeleteMain(String[] args) throws IOException {
        String[][][] table = new String[8][8][2]; // [range][num of threads][algo][throughput]
        Map<String, Integer> ranges = new HashMap<>();
        Map<String, Integer> numberOfThreadss = new HashMap<>();

        String workload = "DELETE_LEAFS";

        ranges.put("131071", 0);
        ranges.put("262143", 1);
        ranges.put("524287", 2);
        ranges.put("1048575", 3);
        ranges.put("2097151", 4);
        ranges.put("4194303", 5);
        ranges.put("8388607", 6);
        ranges.put("16777215", 7);

        numberOfThreadss.put("1", 0);
        numberOfThreadss.put("2", 1);
        numberOfThreadss.put("4", 2);
        numberOfThreadss.put("8", 3);
        numberOfThreadss.put("16", 4);
        numberOfThreadss.put("24", 5);
        numberOfThreadss.put("36", 6);
        numberOfThreadss.put("48", 7);

        Map<Integer, String> rangesRev = reverse(ranges);
        Map<Integer, String> numberOfThreadssRev = reverse(numberOfThreadss);


        for (int al = 0; al < 2; al++) {
            String algoName = algoNames[al];
            File folder = new File(folderName + algoName);

            File[] testResults = folder.listFiles();


            assert testResults != null;
            for (File testResult : testResults) {
                Scanner input = new Scanner(testResult);

                String numberOfThreads = null;
                String throughput = null;
                String range = null;
                boolean isDeleteLeafs = false;

                while (input.hasNextLine()) {
                    String line = input.nextLine().trim();

                    if (line.startsWith("Number of threads:")) {
                        numberOfThreads = line.replace("Number of threads:", "").trim();
                    } else if (line.startsWith("Throughput (ops/s):")) {
                        throughput = line.replace("Throughput (ops/s):", "").replace(".", ",").trim();
                    } else if (line.startsWith("Workload:") && line.endsWith("DELETE_LEAFS")) {
                        isDeleteLeafs = true;
                    } else if (line.startsWith("Range:")) {
                        range = line.replace("Range:", "")
                                .replace("elts", "").replace(".", ",").trim();
                    }
                }

                if (isDeleteLeafs) {
                    try {
                        table[ranges.get(range)]
                                [numberOfThreadss.get(numberOfThreads)]
                                [al] = throughput;
                    } catch (Exception e) {
                        throw e;
                    }
                }
            }

            try (BufferedWriter writer = Files.newBufferedWriter(Paths.get((folderName + "\\tables\\DeleteLeafsTable.txt")))) {

                writer.write("\t" + shortAlgoNames[0] + "\t" + shortAlgoNames[1] + "\n");

                for (int rangeIndex = 0; rangeIndex < ranges.size(); rangeIndex++) {

                    String range = rangesRev.get(rangeIndex);
                    writer.write(workload + " range " + range + "\n");

                    for (int numberOfThreadsIndex = 0; numberOfThreadsIndex < numberOfThreadss.size(); numberOfThreadsIndex++) {
                        String numberOfThreads = numberOfThreadssRev.get(numberOfThreadsIndex);
                        writer.write(numberOfThreads
                                + "\t" + table[rangeIndex][numberOfThreadsIndex][0]
                                + "\t" + table[rangeIndex][numberOfThreadsIndex][1]
                                + "\n");
                    }
                }
                writer.write("\n");
            }

        }
    }

    public static void main(String[] args) throws IOException {
        for (int index = 1; index <= 1; index++) {
            String[][][][] table = new String[2][1][4][4]; // [range][write ratio][num of threads][algo][throughput]
            Map<String, Integer> ranges = new HashMap<>();
            Map<String, Integer> writeRatios = new HashMap<>();
            Map<String, Integer> numberOfThreadss = new HashMap<>();

            String workload = "LEAF_INSERT";

//            ranges.put("524287", 0);
//            ranges.put("1048575", 1);
//            ranges.put("2097151", 2);
            ranges.put("4194303", 0);
            ranges.put("8388607", 1);
//        ranges.put("16777215", 5);

//            writeRatios.put("0.05", 0);
//            writeRatios.put("0.1", 1);
            writeRatios.put("0.2", 0);

            numberOfThreadss.put("16", 0);
            numberOfThreadss.put("24", 1);
            numberOfThreadss.put("36", 2);
            numberOfThreadss.put("48", 3);

            Map<Integer, String> rangesRev = reverse(ranges);
            Map<Integer, String> writeRatiosRev = reverse(writeRatios);
            Map<Integer, String> numberOfThreadssRev = reverse(numberOfThreadss);


            for (int al = 0; al < algoNames2.length; al++) {
                String algoName = algoNames2[al];
                File folder = new File(folderName + "leaf_insert2" + "\\" + algoName);

                File[] testResults = folder.listFiles();


                assert testResults != null;
                for (File testResult : testResults) {
                    if (testResult.isDirectory()) {
                        continue;
                    }
//                if(!testResult.toString().contains("fixed")) {
//                    continue;
//                }
                    Scanner input = new Scanner(testResult);

                    String range = null;
                    String writeRatio = null;
                    String numberOfThreads = null;
                    String throughput = null;
                    boolean isLeafInsert = false;

                    while (input.hasNextLine()) {
                        String line = input.nextLine().trim();

                        if (line.startsWith("Number of threads:")) {
                            numberOfThreads = line.replace("Number of threads:", "").trim();
                        } else if (line.startsWith("Write ratio:")) {
                            writeRatio = line.replace("Write ratio:", "").replace("%", "").trim();
                        } else if (line.startsWith("Throughput (ops/s):")) {
                            throughput = line.replace("Throughput (ops/s):", "").replace(".", ",").trim();
                        } else if (line.startsWith("Key Generator:") && line.endsWith("LEAF_INSERT")) {
                            isLeafInsert = true;
                        } else if (line.startsWith("Range:")) {
                            range = line.replace("Range:", "")
                                    .replace("elts", "").replace(".", ",").trim();
                        }
                    }

                    if (isLeafInsert) {
                        try {
                            table[ranges.get(range)]
                                    [writeRatios.get(writeRatio)]
                                    [numberOfThreadss.get(numberOfThreads)]
                                    [al] = throughput;
                        } catch (Exception e) {
//                            throw e;
                        }
                    }
                }



            }

            try (BufferedWriter writer = Files.newBufferedWriter(
                    Paths.get((folderName + "\\tables\\LeafInsertTable2" + ".txt"))
            )) {

                writer.write("\t" + shortAlgoNames[0]
                        + "\t" + shortAlgoNames[1]
                        + "\t" + shortAlgoNames[2]
                        + "\t" + shortAlgoNames[3]
                        + "\n");

                for (int rangeIndex = 0; rangeIndex < ranges.size(); rangeIndex++) {
                    String range = rangesRev.get(rangeIndex);

                    for (int writeRatioIndex = 0; writeRatioIndex < writeRatios.size(); writeRatioIndex++) {
                        String writeRatio = writeRatiosRev.get(writeRatioIndex);

                        writer.write(workload + " range " + range + " wr-" + writeRatio + "\n");

                        for (int numberOfThreadsIndex = 0; numberOfThreadsIndex < numberOfThreadss.size(); numberOfThreadsIndex++) {
                            String numberOfThreads = numberOfThreadssRev.get(numberOfThreadsIndex);
                            writer.write(numberOfThreads
                                    + "\t" + table[rangeIndex][writeRatioIndex][numberOfThreadsIndex][0]
                                    + "\t" + table[rangeIndex][writeRatioIndex][numberOfThreadsIndex][1]
                                    + "\t" + table[rangeIndex][writeRatioIndex][numberOfThreadsIndex][2]
                                    + "\t" + table[rangeIndex][writeRatioIndex][numberOfThreadsIndex][3]
                                    + "\n");
                        }
                    }
                    writer.write("\n");
                }
            }
        }
    }

    public static void mainLEAFS_HANDSHAKE(String[] args) throws IOException {
        String[][][][][] table = new String[1][3][3][4][3]; // [range][erase ratio][insert ratio][num of threads][algo][throughput]
        Map<String, Integer> ranges = new HashMap<>();
        Map<Integer, Integer> insertRatios = new HashMap<>();
        Map<String, Integer> eraseRatios = new HashMap<>();
        Map<String, Integer> numberOfThreadss = new HashMap<>();

        String workload = "LEAFS_HANDSHAKE";

        ranges.put("524287", 0);
//        ranges.put("1048575", 1);
//        ranges.put("2097151", 2);
//        ranges.put("4194303", 0);
//        ranges.put("8388607", 4);
//        ranges.put("16777215", 5);

        eraseRatios.put("2", 0);
        eraseRatios.put("4", 1);
        eraseRatios.put("8", 2);

        insertRatios.put(1, 0);
        insertRatios.put(2, 1);
        insertRatios.put(3, 2);
//        insertRatios.put(4, 2);

        numberOfThreadss.put("16", 0);
        numberOfThreadss.put("24", 1);
        numberOfThreadss.put("36", 2);
        numberOfThreadss.put("48", 3);

        Map<Integer, String> rangesRev = reverse(ranges);
        Map<Integer, Integer> insertRatiosRev = reverse(insertRatios);
        Map<Integer, String> eraseRatiosRev = reverse(eraseRatios);
        Map<Integer, String> numberOfThreadssRev = reverse(numberOfThreadss);

        String neededSize = "65535";

        for (int al = 0; al < algoNames2.length; al++) {
            String algoName = algoNames2[al];
            File folder = new File(folderName + "leafs_handshake\\" + algoName);

            File[] testResults = folder.listFiles();


            assert testResults != null;
            for (File testResult : testResults) {
                if (testResult.isDirectory()) {
                    continue;
                }
                Scanner input = new Scanner(testResult);

                String range = null;
                Integer insertRatio = null;
                String eraseRatio = null;
                String numberOfThreads = null;
                String throughput = null;
                boolean isNeededWorkload = true;

                while (input.hasNextLine()) {
                    String line = input.nextLine().trim();

                    if (line.startsWith("Number of threads:")) {
                        numberOfThreads = line.replace("Number of threads:", "").trim();
                    } else if (line.startsWith("Insert ratio:")) {
                        insertRatio = Integer.parseInt(
                                line.replace("Insert ratio:", "")
                                        .replace("%", "").trim()
                        );
                    } else if (line.startsWith("Erase ratio:")) {
                        eraseRatio = line.replace("Erase ratio:", "").replace("%", "").trim();
//                        isNeededWorkload = Integer.parseInt(writeRatio) / Integer.parseInt(eraseRatio) == 2;
                        insertRatio /= Integer.parseInt(eraseRatio);
                        if (insertRatio == 4)
                            insertRatio = 3;
                    } else if (line.startsWith("Throughput (ops/s):")) {
                        throughput = line.replace("Throughput (ops/s):", "").replace(".", ",").trim();
                    } else if (line.startsWith("Key Generator:") && line.endsWith("LEAFS_HANDSHAKE")) {
//                        isNeededWorkload = true;
                    } else if (line.startsWith("Range:")) {
                        range = line.replace("Range:", "")
                                .replace("elts", "").replace(".", ",").trim();
                    } else if (line.startsWith("Size:") && !line.endsWith(neededSize + " elts")) {
                        isNeededWorkload = false;
                    }
                }

                if (isNeededWorkload) {
                    try {
                        table[ranges.get(range)]
                                [eraseRatios.get(eraseRatio)]
                                [insertRatios.get(insertRatio)]
                                [numberOfThreadss.get(numberOfThreads)]
                                [al] = throughput;
                    } catch (Exception e) {
//                        throw e;
                    }
                }
            }

            try (BufferedWriter writer = Files.newBufferedWriter(
                    Paths.get((folderName + "\\tables\\FullLeafHandShakeTable_-i-" + neededSize + ".txt"))
            )) {

                writer.write("\t" + shortAlgoNames[0] + "\t" + shortAlgoNames[1] + "\t" + shortAlgoNames[2] + "\n");

                for (int rangeIndex = 0; rangeIndex < ranges.size(); rangeIndex++) {
                    String range = rangesRev.get(rangeIndex);

                    for (int eraseRatioIndex = 0; eraseRatioIndex < eraseRatios.size(); eraseRatioIndex++) {
                        String eraseRatio = eraseRatiosRev.get(eraseRatioIndex);
                        for (int insertRatioIndex = 0; insertRatioIndex < insertRatios.size(); insertRatioIndex++) {
                            int insertRatio = insertRatiosRev.get(insertRatioIndex);

                            writer.write(workload + " range " + range
                                    + " ue-" + eraseRatio + " ui-ue*" + insertRatio + "\n");

                            for (int numberOfThreadsIndex = 0; numberOfThreadsIndex < numberOfThreadss.size(); numberOfThreadsIndex++) {
                                String numberOfThreads = numberOfThreadssRev.get(numberOfThreadsIndex);
                                writer.write(numberOfThreads
                                        + "\t" + table[rangeIndex][eraseRatioIndex][insertRatioIndex][numberOfThreadsIndex][0]
                                        + "\t" + table[rangeIndex][eraseRatioIndex][insertRatioIndex][numberOfThreadsIndex][1]
                                        + "\t" + table[rangeIndex][eraseRatioIndex][insertRatioIndex][numberOfThreadsIndex][2]
                                        + "\n");
                            }
                        }
                    }
                    writer.write("\n");
                }
            }

        }
    }

    public static void mainLEAFS_EXTENSION_HANDSHAKE(String[] args) throws IOException {
        String[] neededSizes = new String[]{"0"}; //"262143", "524288"};
        for (String neededSize : neededSizes) {

            String[][][][][] table = new String[1][4][3][4][3]; // [range][erase ratio][insert ratio][num of threads][algo][throughput]
            Map<String, Integer> ranges = new HashMap<>();
            Map<Integer, Integer> insertRatios = new HashMap<>();
            Map<String, Integer> eraseRatios = new HashMap<>();
            Map<String, Integer> numberOfThreadss = new HashMap<>();

            String workload = "LEAFS_EXTENSION_HANDSHAKE";

//        ranges.put("524287", 0);
//        ranges.put("1048575", 1);
            ranges.put("2097151", 0);
//        ranges.put("4194303", 0);
//        ranges.put("8388607", 4);
//        ranges.put("16777215", 5);

            eraseRatios.put("2", 0);
            eraseRatios.put("4", 1);
            eraseRatios.put("8", 2);
            eraseRatios.put("16", 3);

            insertRatios.put(1, 0);
            insertRatios.put(2, 1);
            insertRatios.put(3, 2);
//        insertRatios.put(4, 2);

            numberOfThreadss.put("16", 0);
            numberOfThreadss.put("24", 1);
            numberOfThreadss.put("36", 2);
            numberOfThreadss.put("48", 3);

            Map<Integer, String> rangesRev = reverse(ranges);
            Map<Integer, Integer> insertRatiosRev = reverse(insertRatios);
            Map<Integer, String> eraseRatiosRev = reverse(eraseRatios);
            Map<Integer, String> numberOfThreadssRev = reverse(numberOfThreadss);


            for (int al = 0; al < algoNames2.length; al++) {
                String algoName = algoNames2[al];
                File folder = new File(folderName + "leafs_extension_handshake\\" + algoName);

                File[] testResults = folder.listFiles();


                assert testResults != null;
                for (File testResult : testResults) {
                    if (testResult.isDirectory()) {
                        continue;
                    }
                    Scanner input = new Scanner(testResult);

                    String range = null;
                    Integer insertRatio = null;
                    String eraseRatio = null;
                    String numberOfThreads = null;
                    String throughput = null;
                    boolean isNeededWorkload = true;

                    while (input.hasNextLine()) {
                        String line = input.nextLine().trim();

                        if (line.startsWith("Number of threads:")) {
                            numberOfThreads = line.replace("Number of threads:", "").trim();
                        } else if (line.startsWith("Insert ratio:")) {
                            insertRatio = Integer.parseInt(
                                    line.replace("Insert ratio:", "")
                                            .replace("%", "").trim()
                            );
                        } else if (line.startsWith("Erase ratio:")) {
                            eraseRatio = line.replace("Erase ratio:", "").replace("%", "").trim();
//                        isNeededWorkload = Integer.parseInt(writeRatio) / Integer.parseInt(eraseRatio) == 2;
                            insertRatio /= Integer.parseInt(eraseRatio);
//                        if (insertRatio == 4)
//                            insertRatio = 3;
                        } else if (line.startsWith("Throughput (ops/s):")) {
                            throughput = line.replace("Throughput (ops/s):", "").replace(".", ",").trim();
                        } else if (line.startsWith("Key Generator:") && line.endsWith(workload)) {
//                        isNeededWorkload = true;
                        } else if (line.startsWith("Range:")) {
                            range = line.replace("Range:", "")
                                    .replace("elts", "").replace(".", ",").trim();
                        } else if (line.startsWith("Size:") && !line.endsWith(neededSize + " elts")) {
                            isNeededWorkload = false;
                        }
                    }

                    if (isNeededWorkload) {
                        try {
                            table[ranges.get(range)]
                                    [eraseRatios.get(eraseRatio)]
                                    [insertRatios.get(insertRatio)]
                                    [numberOfThreadss.get(numberOfThreads)]
                                    [al] = throughput;
                        } catch (Exception e) {
                            throw e;
                        }
                    }
                }

                try (BufferedWriter writer = Files.newBufferedWriter(
                        Paths.get((folderName + "\\tables\\FullLeafsExtensionHandShakeTable_-i-" + neededSize + ".txt"))
                )) {

                    writer.write("\t" + shortAlgoNames[0] + "\t" + shortAlgoNames[1] + "\t" + shortAlgoNames[2] + "\n");

                    for (int rangeIndex = 0; rangeIndex < ranges.size(); rangeIndex++) {
                        String range = rangesRev.get(rangeIndex);

                        for (int eraseRatioIndex = 0; eraseRatioIndex < eraseRatios.size(); eraseRatioIndex++) {
                            String eraseRatio = eraseRatiosRev.get(eraseRatioIndex);
                            for (int insertRatioIndex = 0; insertRatioIndex < insertRatios.size(); insertRatioIndex++) {
                                int insertRatio = insertRatiosRev.get(insertRatioIndex);

                                writer.write(workload + " range " + range
                                        + " ue-" + eraseRatio + " ui-ue*" + insertRatio + "\n");

                                for (int numberOfThreadsIndex = 0; numberOfThreadsIndex < numberOfThreadss.size(); numberOfThreadsIndex++) {
                                    String numberOfThreads = numberOfThreadssRev.get(numberOfThreadsIndex);
                                    writer.write(numberOfThreads
                                            + "\t" + table[rangeIndex][eraseRatioIndex][insertRatioIndex][numberOfThreadsIndex][0]
                                            + "\t" + table[rangeIndex][eraseRatioIndex][insertRatioIndex][numberOfThreadsIndex][1]
                                            + "\t" + table[rangeIndex][eraseRatioIndex][insertRatioIndex][numberOfThreadsIndex][2]
                                            + "\n");
                                }
                            }
                        }
                        writer.write("\n");
                    }
                }

            }
        }
    }

    public static void mainTempOper(String[] args) throws IOException {
        String[] neededSizes = new String[]{"524287"};//{"0", "262143"}; // "524288"};
        for (String neededSize : neededSizes) {

            Double[][][][][][] table = new Double[1][3][2][4][4][3]; // [range][operTimes][erase ratio][num of threads][algo][throughput]
            Map<String, Integer> ranges = new HashMap<>();
            Map<String, Integer> operTimes = new HashMap<>();
            Map<String, Integer> eraseRatios = new HashMap<>();
            Map<String, Integer> numberOfThreadss = new HashMap<>();

            String workload = "TEMPORARY_OPERATIONS";

//        ranges.put("524287", 0);
            ranges.put("1048575", 0);
//            ranges.put("2097151", 0);
//        ranges.put("4194303", 0);
//        ranges.put("8388607", 4);
//        ranges.put("16777215", 5);

            eraseRatios.put("40", 0);
            eraseRatios.put("33", 1);
//            eraseRatios.put("8", 2);
//            eraseRatios.put("16", 3);

            operTimes.put("12000", 0);
            operTimes.put("16000", 1);
            operTimes.put("20000", 2);

//            insertRatios.put(1, 0);
//            insertRatios.put(2, 1);
//            insertRatios.put(3, 2);
//        insertRatios.put(4, 2);

            numberOfThreadss.put("16", 0);
            numberOfThreadss.put("24", 1);
            numberOfThreadss.put("36", 2);
            numberOfThreadss.put("48", 3);

            Map<Integer, String> rangesRev = reverse(ranges);
            Map<Integer, String> operTimesRev = reverse(operTimes);
            Map<Integer, String> eraseRatiosRev = reverse(eraseRatios);
            Map<Integer, String> numberOfThreadssRev = reverse(numberOfThreadss);


            for (int al = 0; al < algoNames2.length; al++) {
                String algoName = algoNames2[al];
                File folder = new File(folderName + "temp_oper_rep2\\" + algoName);

                File[] testResults = folder.listFiles();


                assert testResults != null;
                for (File testResult : testResults) {
                    if (testResult.isDirectory()) {
                        continue;
                    }
                    Scanner input = new Scanner(testResult);

                    String range = null;
                    String operTime = null;
                    String eraseRatio = null;
                    String numberOfThreads = null;
                    String throughput = null;
                    boolean isNeededWorkload = true;

                    while (input.hasNextLine()) {
                        String line = input.nextLine().trim();

                        if (line.startsWith("Number of threads:")) {
                            numberOfThreads = line.replace("Number of threads:", "").trim();
                        } else if (line.startsWith("Time of 1:")) {
                            operTime =
                                    line.replace("Time of 1:", "")
                                            .replace("op.", "").trim()
                            ;
                        } else if (line.startsWith("Erase ratio of 0:")) {
                            eraseRatio = line.replace("Erase ratio of 0:", "").replace("%", "").trim();
//                        isNeededWorkload = Integer.parseInt(writeRatio) / Integer.parseInt(eraseRatio) == 2;
//                            insertRatio /= Integer.parseInt(eraseRatio);
//                        if (insertRatio == 4)
//                            insertRatio = 3;
                        } else if (line.startsWith("Throughput (ops/s):")) {
                            throughput = line.replace("Throughput (ops/s):", "").trim();
                        } else if (line.startsWith("Key Generator:") && line.endsWith(workload)) {
//                        isNeededWorkload = true;
                        } else if (line.startsWith("Range:")) {
                            range = line.replace("Range:", "")
                                    .replace("elts", "").replace(".", ",").trim();
                        } else if (line.startsWith("Size:") && !line.endsWith(neededSize + " elts")) {
                            isNeededWorkload = false;
                        }
                    }

                    if (isNeededWorkload) {
                        try {
                            double cur_throughput = Double.parseDouble(throughput);

                            Double cur_max = table[ranges.get(range)]
                                    [operTimes.get(operTime)]
                                    [eraseRatios.get(eraseRatio)]
                                    [numberOfThreadss.get(numberOfThreads)]
                                    [al][1];

                            Double cur_min = table[ranges.get(range)]
                                    [operTimes.get(operTime)]
                                    [eraseRatios.get(eraseRatio)]
                                    [numberOfThreadss.get(numberOfThreads)]
                                    [al][2];


                            if (table[ranges.get(range)]
                                    [operTimes.get(operTime)]
                                    [eraseRatios.get(eraseRatio)]
                                    [numberOfThreadss.get(numberOfThreads)]
                                    [al][0] == null) {
                                table[ranges.get(range)]
                                        [operTimes.get(operTime)]
                                        [eraseRatios.get(eraseRatio)]
                                        [numberOfThreadss.get(numberOfThreads)]
                                        [al][0] = 0.0;
                            }

                            table[ranges.get(range)]
                                    [operTimes.get(operTime)]
                                    [eraseRatios.get(eraseRatio)]
                                    [numberOfThreadss.get(numberOfThreads)]
                                    [al][0] += cur_throughput;

                            if (cur_max == null || cur_max < cur_throughput) {
                                table[ranges.get(range)]
                                        [operTimes.get(operTime)]
                                        [eraseRatios.get(eraseRatio)]
                                        [numberOfThreadss.get(numberOfThreads)]
                                        [al][1] = cur_throughput;
                            }

                            if (cur_min == null || cur_min > cur_throughput) {
                                table[ranges.get(range)]
                                        [operTimes.get(operTime)]
                                        [eraseRatios.get(eraseRatio)]
                                        [numberOfThreadss.get(numberOfThreads)]
                                        [al][2] = cur_throughput;
                            }

                        } catch (Exception e) {
                            throw e;
                        }
                    }
                }


            }

            try (BufferedWriter writer = Files.newBufferedWriter(
//                        Paths.get((folderName + "\\tables\\TempOperLeafsHandshakeTableREP_-i-" + neededSize + ".txt"))
                    Paths.get((folderName + "\\tables\\TempOperLeafsHandshakeTableREP2.txt"))
            )) {

                writer.write("\t" + shortAlgoNames[0] + "\t" + shortAlgoNames[1] + "\t"
                        + shortAlgoNames[2] + "\t" + shortAlgoNames[3] + "\t");

                writer.write(shortAlgoNames[0] + "_max\t" + shortAlgoNames[1] + "_max\t"
                        + shortAlgoNames[2] + "_max\t" + shortAlgoNames[3] + "_max\t");

                writer.write(shortAlgoNames[0] + "_min\t" + shortAlgoNames[1] + "_min\t"
                        + shortAlgoNames[2] + "_min\t" + shortAlgoNames[3] + "_min\n");


                for (int rangeIndex = 0; rangeIndex < ranges.size(); rangeIndex++) {
                    String range = rangesRev.get(rangeIndex);

                    for (int eraseRatioIndex = 0; eraseRatioIndex < eraseRatios.size(); eraseRatioIndex++) {
                        String eraseRatio = eraseRatiosRev.get(eraseRatioIndex);
                        for (int operTimeIndex = 0; operTimeIndex < operTimes.size(); operTimeIndex++) {
                            String operTime = operTimesRev.get(operTimeIndex);

                            writer.write(workload + " range " + range
                                    + " opTime-" + operTime
                                    + " ue-" + eraseRatio + "\n");

                            for (int numberOfThreadsIndex = 0; numberOfThreadsIndex < numberOfThreadss.size(); numberOfThreadsIndex++) {
                                String numberOfThreads = numberOfThreadssRev.get(numberOfThreadsIndex);
                                writer.write(numberOfThreads);

                                for (int al = 0; al < algoNames.length; al++) {
                                    writer.write("\t"
                                            + Double.toString(table[rangeIndex][operTimeIndex][eraseRatioIndex][numberOfThreadsIndex][al][0] / 5)
                                            .replace(".", ","));
                                }

                                for (int al = 0; al < algoNames.length; al++) {
                                    writer.write("\t"
                                            + Double.toString(table[rangeIndex][operTimeIndex][eraseRatioIndex][numberOfThreadsIndex][al][1])
                                            .replace(".", ","));
                                }

                                for (int al = 0; al < algoNames.length; al++) {
                                    writer.write("\t"
                                            + Double.toString(table[rangeIndex][operTimeIndex][eraseRatioIndex][numberOfThreadsIndex][al][2])
                                            .replace(".", ","));
                                }

                                writer.write("\n");
                            }
                        }
                    }
                    writer.write("\n");
                }
            }
        }
    }


    public static void mainCREAKERS_AND_WAVE(String[] args) throws IOException {
        String[] neededSizes = new String[]{"100000"};//{"0", "262143"}; // "524288"};
        for (String neededSize : neededSizes) {

            Double[][][][][] table = new Double[2][3][4][4][3]; // [preffil_size][write ratio][num of threads][algo][throughput]
            Map<String, Integer> waveSizes = new HashMap<>();
            Map<String, Integer> writeRatios = new HashMap<>();
            Map<String, Integer> numberOfThreadss = new HashMap<>();

            String workload = "CREAKERS_AND_WAVE";

//        prefillSizes.put("524287", 0);
            waveSizes.put("0.1", 0);
            waveSizes.put("0.2", 1);
//            prefillSizes.put("2097151", 0);
//        prefillSizes.put("4194303", 0);
//        prefillSizes.put("8388607", 4);
//        prefillSizes.put("16777215", 5);

            writeRatios.put("0.05", 0);
            writeRatios.put("0.1", 1);
            writeRatios.put("0.2", 2);

            numberOfThreadss.put("16", 0);
            numberOfThreadss.put("24", 1);
            numberOfThreadss.put("36", 2);
            numberOfThreadss.put("48", 3);

            Map<Integer, String> waveSizeRev = reverse(waveSizes);
            Map<Integer, String> eraseRatiosRev = reverse(writeRatios);
            Map<Integer, String> numberOfThreadssRev = reverse(numberOfThreadss);


            for (int al = 0; al < algoNames2.length; al++) {
                String algoName = algoNames2[al];
                File folder = new File(folderName + "creakers_and_wave_shuffle\\" + algoName);

                File[] testResults = folder.listFiles();

                assert testResults != null;
                for (File testResult : testResults) {
                    if (testResult.isDirectory()) {
                        continue;
                    }
                    Scanner input = new Scanner(testResult);

                    String waveSize = null;
                    String writeRatio = null;
                    String numberOfThreads = null;
                    String throughput = null;
                    boolean isNeededWorkload = true;

                    while (input.hasNextLine()) {
                        String line = input.nextLine().trim();

                        if (line.startsWith("Number of threads:")) {
                            numberOfThreads = line.replace("Number of threads:", "").trim();
                        } else if (line.startsWith("Write ratio:")) {
                            writeRatio = line.replace("Write ratio:", "").replace("%", "").trim();
                        } else if (line.startsWith("Throughput (ops/s):")) {
                            throughput = line.replace("Throughput (ops/s):", "").trim();
                        } else if (line.startsWith("Key Generator:") && line.endsWith(workload)) {
//                        isNeededWorkload = true;
                        } else if (line.startsWith("Wave size:")) {
                            waveSize = line.replace("Wave size:", "").trim();
//                        } else if (line.startsWith("Size:") && !line.endsWith(neededSize + " elts")) {
//                            isNeededWorkload = false;
                        }
                    }

                    if (isNeededWorkload) {
                        try {
                            double cur_throughput = Double.parseDouble(throughput);

                            Double cur_max = table[waveSizes.get(waveSize)]
                                    [writeRatios.get(writeRatio)]
                                    [numberOfThreadss.get(numberOfThreads)]
                                    [al][1];

                            Double cur_min = table[waveSizes.get(waveSize)]
                                    [writeRatios.get(writeRatio)]
                                    [numberOfThreadss.get(numberOfThreads)]
                                    [al][2];


                            if (table[waveSizes.get(waveSize)]
                                    [writeRatios.get(writeRatio)]
                                    [numberOfThreadss.get(numberOfThreads)]
                                    [al][0] == null) {
                                table[waveSizes.get(waveSize)]
                                        [writeRatios.get(writeRatio)]
                                        [numberOfThreadss.get(numberOfThreads)]
                                        [al][0] = 0.0;
                            }

                            table[waveSizes.get(waveSize)]
                                    [writeRatios.get(writeRatio)]
                                    [numberOfThreadss.get(numberOfThreads)]
                                    [al][0] += cur_throughput;

                            if (cur_max == null || cur_max < cur_throughput) {
                                table[waveSizes.get(waveSize)]
                                        [writeRatios.get(writeRatio)]
                                        [numberOfThreadss.get(numberOfThreads)]
                                        [al][1] = cur_throughput;
                            }

                            if (cur_min == null || cur_min > cur_throughput) {
                                table[waveSizes.get(waveSize)]
                                        [writeRatios.get(writeRatio)]
                                        [numberOfThreadss.get(numberOfThreads)]
                                        [al][2] = cur_throughput;
                            }

                        } catch (Exception e) {
                            throw e;
                        }
                    }
                }


            }

            try (BufferedWriter writer = Files.newBufferedWriter(
//                        Paths.get((folderName + "\\tables\\TempOperLeafsHandshakeTableREP_-i-" + neededSize + ".txt"))
                    Paths.get((folderName + "\\tables\\CreakersAndWaveShuffle.txt"))
            )) {

                writer.write("\t" + shortAlgoNames[0] + "\t" + shortAlgoNames[1] + "\t"
                        + shortAlgoNames[2] + "\t" + shortAlgoNames[3] + "\t");

                writer.write(shortAlgoNames[0] + "_max\t" + shortAlgoNames[1] + "_max\t"
                        + shortAlgoNames[2] + "_max\t" + shortAlgoNames[3] + "_max\t");

                writer.write(shortAlgoNames[0] + "_min\t" + shortAlgoNames[1] + "_min\t"
                        + shortAlgoNames[2] + "_min\t" + shortAlgoNames[3] + "_min\n");


                for (int waveSizeIndex = 0; waveSizeIndex < waveSizes.size(); waveSizeIndex++) {
                    String waveSize = waveSizeRev.get(waveSizeIndex);

                    for (int writeRatioIndex = 0; writeRatioIndex < writeRatios.size(); writeRatioIndex++) {
                        String writeRatio = eraseRatiosRev.get(writeRatioIndex);

                            writer.write(workload + " -ws " + waveSize
                                    + " u-" + writeRatio + "\n");

                            for (int numberOfThreadsIndex = 0; numberOfThreadsIndex < numberOfThreadss.size(); numberOfThreadsIndex++) {
                                String numberOfThreads = numberOfThreadssRev.get(numberOfThreadsIndex);
                                writer.write(numberOfThreads);

                                for (int al = 0; al < algoNames.length; al++) {
                                    writer.write("\t"
                                            + Double.toString(table[waveSizeIndex][writeRatioIndex][numberOfThreadsIndex][al][0] / 5)
                                            .replace(".", ","));
                                }

                                for (int al = 0; al < algoNames.length; al++) {
                                    writer.write("\t"
                                            + Double.toString(table[waveSizeIndex][writeRatioIndex][numberOfThreadsIndex][al][1])
                                            .replace(".", ","));
                                }

                                for (int al = 0; al < algoNames.length; al++) {
                                    writer.write("\t"
                                            + Double.toString(table[waveSizeIndex][writeRatioIndex][numberOfThreadsIndex][al][2])
                                            .replace(".", ","));
                                }

                                writer.write("\n");
                        }
                    }
                    writer.write("\n");
                }
            }
        }
    }

}

