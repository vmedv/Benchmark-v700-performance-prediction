/* 
 * File:   zipf.h
 * Author: t35brown
 *
 * Created on August 5, 2019, 5:05 PM
 */

// adapted from Masoud Kazemi's stack overflow post
// https://stackoverflow.com/questions/9983239/how-to-generate-zipf-distributed-numbers-efficiently

#ifndef ZIPF_H
#define ZIPF_H

#include <cassert>
#include <limits>

// call once *before* threading (specifying n=maxkey), then can call freely with threading...

int rand_zipf(RandomFNV1A& rng, double alpha, int n) {
    static int first = 1; // Static first time flag
    static double c = 0; // Normalization constant
    static double *sum_probs; // Pre-calculated sum of probabilities
    double z; // Uniform random number (0 < z < 1)
    int zipf_value; // Computed exponential value to be returned
    int i; // Loop counter
    int low, high, mid; // Binary-search bounds

    // Compute normalization constant on first call only
    if (first == 1) {
        for (i = 1; i <= n; i++)
            c = c + (1.0 / pow((double) i, alpha));
        c = 1.0 / c;

        sum_probs = malloc((n + 1) * sizeof (*sum_probs));
        sum_probs[0] = 0;
        for (i = 1; i <= n; i++) {
            sum_probs[i] = sum_probs[i - 1] + c / pow((double) i, alpha);
        }
        first = 0;
    }

    // Pull a uniform random number (0 < z < 1)
    do {
        z = (double) (rng.next() / std::numeric_limits<uint64_t>::max());
    } while ((z == 0) || (z == 1));

    // Map z to the value
    low = 1, high = n, mid;
    do {
        mid = floor((low + high) / 2);
        if (sum_probs[mid] >= z && sum_probs[mid - 1] < z) {
            zipf_value = mid;
            break;
        } else if (sum_probs[mid] >= z) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    } while (low <= high);

    // Assert that zipf_value is between 1 and N
    assert((zipf_value >= 1) && (zipf_value <= n));

    return (zipf_value);
}

#endif /* ZIPF_H */

