/*
 * File:   keygen.h
 * Author: t35brown
 *
 * Created on August 5, 2019, 7:24 PM
 */

#ifndef KEYGEN_H
#define KEYGEN_H

#include <algorithm>

#include <cassert>
#include "plaf.h"
#include "distribution.h"

template<typename K>
class KeyGenerator {
public:
    KeyGenerator() = default;

    virtual K next_read() = 0;

    virtual K next_erase() = 0;

    virtual K next_insert() = 0;

    virtual K next_prefill() = 0;
};

template<typename K>
class SimpleKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    Distribution<K> * distribution;
    PAD;
public:
    SimpleKeyGenerator(Distribution<K> *_distribution) : distribution(_distribution) {}

    K next_read() {
        return distribution->next();
    }

    K next_erase() {
        return distribution->next();
    }

    K next_insert() {
        return distribution->next();
    }

    K next_prefill() {
        return distribution->next();
    }
};


class KeyGeneratorSkewedSetsData {
public:
    PAD;
    int maxKey;
    size_t readSetLength;
    size_t writeSetLength;

    size_t writeSetBegin;
    size_t writeSetEnd;

    int *data;
    PAD;

    KeyGeneratorSkewedSetsData(const int _maxKey,
                               const double _readSetSize, const double _writeSetSize, const double _interSetSize) {
        maxKey = _maxKey;
        data = new int[maxKey];
        for (int i = 0; i < maxKey; i++) {
            data[i] = i + 1;
        }

        std::random_shuffle(data, data + maxKey - 1);

        readSetLength = (size_t) (maxKey * _readSetSize) + 1;
        writeSetBegin = readSetLength - (size_t) (maxKey * _interSetSize);
        writeSetEnd = writeSetBegin + (size_t) (maxKey * _writeSetSize) + 1;
        writeSetLength = writeSetEnd - writeSetBegin;
    }

    ~KeyGeneratorSkewedSetsData() {
        delete[] data;
    }
};

template<typename K>
class KeyGeneratorSkewedSets: public KeyGenerator<K> {
private:
    PAD;
    KeyGeneratorSkewedSetsData *keygenData;
    Random64 *rng;
    Distribution<K> *readDist;
    Distribution<K> *writeDist;
    double readProb;
    double writeProb;
    size_t prefillSize;
    PAD;
public:
    KeyGeneratorSkewedSets(KeyGeneratorSkewedSetsData *_keygenData, Random64 *_rng,
                           Distribution<K> *_readDist, Distribution<K> *_writeDist,
                           const double _readProb, const double _writeProb)
            : keygenData(_keygenData), rng(_rng), readDist(_readDist), writeDist(_writeDist),
              readProb(_readProb), writeProb(_writeProb) {}

    K next_read() {
        return keygenData->data[readDist->next()];
//        int value = 0;
//        double z; // Uniform random number (0 < z < 1)
//        // Pull a uniform random number (0 < z < 1)
//        do {
//            z = (rng->next() / (double) std::numeric_limits<uint64_t>::max());
//        } while ((z == 0) || (z == 1));
//        if (z < readProb) {
//            value = data->data[rng->next(data->readSetLength)];
//        } else {
//            value = data->data[data->readSetLength + rng->next(data->maxKey - data->readSetLength)];
//        }
//        return value;
    }

    K next_write() {
        return keygenData->data[(keygenData->writeSetBegin + writeDist->next()) % keygenData->maxKey];
//        int value = 0;
//        double z; // Uniform random number (0 < z < 1)
//        // Pull a uniform random number (0 < z < 1)
//        do {
//            z = (rng->next() / (double) std::numeric_limits<uint64_t>::max());
//        } while ((z == 0) || (z == 1));
//        if (z < writeProb) {
//            value = data->data[data->writeSetBegin + rng->next(data->writeSetLength)];
//        } else {
//            value = data->data[(data->writeSetEnd + rng->next(data->maxKey - data->writeSetLength)) % data->maxKey];
//        }
//        return value;
    }

    K next_erase() {
        return this->next_write();
    }

    K next_insert() {
        return this->next_write();
    }

    K next_prefill() {
        K value = 0;
        if (prefillSize < keygenData->readSetLength) {
            value = keygenData->data[prefillSize++];
        } else {
            value = keygenData->data[keygenData->readSetLength + rng->next(keygenData->maxKey - keygenData->readSetLength)];
        }
        return value;
    }
};

#endif /* KEYGEN_H */

