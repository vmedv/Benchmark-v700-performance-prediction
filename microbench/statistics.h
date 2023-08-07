//
// Created by Ravil Galiev on 03.08.2023.
//

#ifndef SETBENCH_STATISTICS_H
#define SETBENCH_STATISTICS_H

#include "define_global_statistics.h"
#include "gstats_global.h"
#include "debugprinting.h"
#include "json/single_include/nlohmann/json.hpp"
#include "globals_extern.h"

struct Statistic {
    long long totalAll = 0;

    long long totalGets;
    long long totalRQs;
    long long totalQueries;
    long long totalInserts;
    long long totalDeletes;
    long long totalUpdates;

    double SECONDS_TO_RUN;
    long long throughputSearches;
    long long throughputRQs;
    long long throughputQueries;
    long long throughputUpdates;
    long long throughputAll;

    Statistic(double _SECONDS_TO_RUN) {
        totalGets = GSTATS_GET_STAT_METRICS(num_searches, TOTAL)[0].sum;
        totalRQs = GSTATS_GET_STAT_METRICS(num_rq, TOTAL)[0].sum;
        totalQueries = totalGets + totalRQs;
        totalInserts = GSTATS_GET_STAT_METRICS(num_inserts, TOTAL)[0].sum;
        totalDeletes = GSTATS_GET_STAT_METRICS(num_deletes, TOTAL)[0].sum;
        totalUpdates = totalInserts + totalDeletes;

        SECONDS_TO_RUN = _SECONDS_TO_RUN; // (MILLIS_TO_RUN)/1000.;
        totalAll = totalUpdates + totalQueries;
        throughputSearches = (long long) (totalGets / SECONDS_TO_RUN);
        throughputRQs = (long long) (totalRQs / SECONDS_TO_RUN);
        throughputQueries = (long long) (totalQueries / SECONDS_TO_RUN);
        throughputUpdates = (long long) (totalUpdates / SECONDS_TO_RUN);
        throughputAll = (long long) (totalAll / SECONDS_TO_RUN);
    }

    void printTotalStatisticShort() const {
        COUTATOMIC(std::endl);
        COUTATOMIC("total_find=" << totalGets << std::endl)
        COUTATOMIC("total_rq=" << totalRQs << std::endl)
        COUTATOMIC("total_inserts=" << totalInserts << std::endl)
        COUTATOMIC("total_deletes=" << totalDeletes << std::endl)
        COUTATOMIC("total_updates=" << totalUpdates << std::endl)
        COUTATOMIC("total_queries=" << totalQueries << std::endl)
        COUTATOMIC("total_ops=" << totalAll << std::endl)
        COUTATOMIC("find_throughput=" << throughputSearches << std::endl)
        COUTATOMIC("rq_throughput=" << throughputRQs << std::endl)
        COUTATOMIC("update_throughput=" << throughputUpdates << std::endl)
        COUTATOMIC("query_throughput=" << throughputQueries << std::endl)
        COUTATOMIC("total_throughput=" << throughputAll << std::endl)
        COUTATOMIC(std::endl);
    }

    void printTotalStatistic() const {
//        COUTATOMIC(std::endl)
//        COUTATOMIC("total find                    : " << totalGets << std::endl)
//        COUTATOMIC("total rq                      : " << totalRQs << std::endl)
//        COUTATOMIC("total inserts                 : " << totalInserts << std::endl)
//        COUTATOMIC("total deletes                 : " << totalDeletes << std::endl)
//        COUTATOMIC("total updates                 : " << totalUpdates << std::endl)
//        COUTATOMIC("total queries                 : " << totalQueries << std::endl)
//        COUTATOMIC("total ops                     : " << totalAll << std::endl)
//        COUTATOMIC("find throughput               : " << throughputSearches << std::endl)
//        COUTATOMIC("rq throughput                 : " << throughputRQs << std::endl)
//        COUTATOMIC("update throughput             : " << throughputUpdates << std::endl)
//        COUTATOMIC("query throughput              : " << throughputQueries << std::endl)
//        COUTATOMIC("total throughput              : " << throughputAll << std::endl)
//        COUTATOMIC(std::endl)


        COUTATOMIC(std::endl)
        COUTATOMIC(indented_title_with_data("total find", totalGets))
        COUTATOMIC(indented_title_with_data("total rq", totalRQs))
        COUTATOMIC(indented_title_with_data("total inserts", totalInserts))
        COUTATOMIC(indented_title_with_data("total deletes", totalDeletes))
        COUTATOMIC(indented_title_with_data("total updates", totalUpdates))
        COUTATOMIC(indented_title_with_data("total queries", totalQueries))
        COUTATOMIC(indented_title_with_data("total ops", totalAll))
        COUTATOMIC(indented_title_with_data("find throughput", throughputSearches))
        COUTATOMIC(indented_title_with_data("rq throughput", throughputRQs))
        COUTATOMIC(indented_title_with_data("update throughput", throughputUpdates))
        COUTATOMIC(indented_title_with_data("query throughput", throughputQueries))
        COUTATOMIC(indented_title_with_data("total throughput", throughputAll))
        COUTATOMIC(std::endl)
    }

};

void to_json(nlohmann::json &json, const Statistic &s) {
    json["total_gets"] = s.totalGets;
    json["total_rq"] = s.totalRQs;
    json["total_inserts"] = s.totalInserts;
    json["total_deletes"] = s.totalDeletes;
    json["total_updates"] = s.totalUpdates;
    json["total_queries"] = s.totalQueries;
    json["total_ops"] = s.totalAll;
    json["find_throughput"] = s.throughputSearches;
    json["rq_throughput"] = s.throughputRQs;
    json["update_throughput"] = s.throughputUpdates;
    json["query_throughput"] = s.throughputQueries;
    json["total_throughput"] = s.throughputAll;
}

void from_json(const nlohmann::json &json, Statistic &s) {
    s.totalGets = json["total_gets"];
    s.totalRQs = json["total_rq"];
    s.totalInserts = json["total_inserts"];
    s.totalDeletes = json["total_deletes"];
    s.totalUpdates = json["total_updates"];
    s.totalQueries = json["total_queries"];
    s.totalAll = json["total_ops"];
    s.throughputSearches = json["find_throughput"];
    s.throughputRQs = json["rq_throughput"];
    s.throughputUpdates = json["update_throughput"];
    s.throughputQueries = json["query_throughput"];
    s.throughputAll = json["total_throughput"];
}




#endif //SETBENCH_STATISTICS_H
