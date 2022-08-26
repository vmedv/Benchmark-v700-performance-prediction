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
#include "parameters/skewed_sets_parameters.h"

template<typename K>
class KeyGenerator {
public:
    KeyGenerator() = default;

    virtual K next_read() = 0;

    virtual K next_erase() = 0;

    virtual K next_insert() = 0;

    virtual K next_prefill() = 0;
};

class KeyGeneratorData {

};

template<typename K>
class SimpleKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    Distribution<K> *distribution;
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


class KeyGeneratorSkewedSetsData : public KeyGeneratorData {
public:
    PAD;
    size_t maxKey;
    size_t readSetLength;
    size_t writeSetLength;

    size_t writeSetBegin;
    size_t writeSetEnd;

    int *data;
    PAD;

    KeyGeneratorSkewedSetsData(const size_t _maxKey,
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
class KeyGeneratorSkewedSets : public KeyGenerator<K> {
private:
    PAD;
    KeyGeneratorSkewedSetsData *keygenData;
    Random64 *rng;
    Distribution<K> *readDist;
    Distribution<K> *writeDist;
    size_t prefillSize;
    PAD;
public:
    KeyGeneratorSkewedSets(KeyGeneratorSkewedSetsData *_keygenData, Random64 *_rng,
                           Distribution<K> *_readDist, Distribution<K> *_writeDist)
            : keygenData(_keygenData), rng(_rng), readDist(_readDist), writeDist(_writeDist) {}

    K next_read() {
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
        return keygenData->data[readDist->next()];
    }

    K next_write() {
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
        return keygenData->data[(keygenData->writeSetBegin + writeDist->next()) % keygenData->maxKey];
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
            value = keygenData->data[keygenData->readSetLength +
                                     rng->next(keygenData->maxKey - keygenData->readSetLength)];
        }
        return value;
    }
};

class KeyGeneratorTemporarySkewedData : public KeyGeneratorData {
public:
    PAD;
    size_t maxKey;

    int *data;
    size_t *setLengths;
    size_t *setBegins;
    TemporarySkewedParameters *TSParm;
    PAD;

    KeyGeneratorTemporarySkewedData(const size_t _maxKey, TemporarySkewedParameters *_TSParm) {
        TSParm = _TSParm;
        maxKey = _maxKey;
        data = new int[maxKey];
        for (int i = 0; i < maxKey; i++) {
            data[i] = i + 1;
        }

        std::random_shuffle(data, data + maxKey - 1);

        setLengths = new size_t[TSParm->setCount + 1];
        setBegins = new size_t[TSParm->setCount + 1];
        setBegins[0] = 0;
        for (int i = 0; i < TSParm->setCount; i++) {
            setLengths[i] = (size_t) (maxKey * TSParm->setSizes[i]) + 1;
            setBegins[i + 1] = setBegins[i] + setLengths[i];
        }
    }

    ~KeyGeneratorTemporarySkewedData() {
        delete[] data;
        delete[] setLengths;
        delete[] setBegins;
    }
};

template<typename K>
class KeyGeneratorTemporarySkewed : public KeyGenerator<K> {
private:
    PAD;
    KeyGeneratorTemporarySkewedData *keygenData;
    Distribution<K> **dists;
    long long time;
    size_t pointer;
    PAD;

    void update_pointer() {
        if (time > keygenData->TSParm->hotTimes[pointer]) {
            time = 0;
            ++pointer;
            if (pointer >= keygenData->TSParm->setCount) {
                pointer = 0;
            }
        } else {
            ++time;
        }
    }

public:
    KeyGeneratorTemporarySkewed(KeyGeneratorTemporarySkewedData *_keygenData, Distribution<K> **_dists)
            : keygenData(_keygenData), dists(_dists) {
        time = 0;
        pointer = 0;
    }

    K next_read() {
        update_pointer();
        return keygenData->data[keygenData->setBegins[pointer] + dists[pointer]->next()];
    }

    K next_write() {
        update_pointer();
        return keygenData->data[keygenData->setBegins[pointer] + dists[pointer]->next()];
    }

    K next_erase() {
        return this->next_write();
    }

    K next_insert() {
        return this->next_write();
    }

    K next_prefill() {
        update_pointer();
        return keygenData->data[keygenData->setBegins[pointer] + dists[pointer]->next()];
    }
};

#endif /* KEYGEN_H */

