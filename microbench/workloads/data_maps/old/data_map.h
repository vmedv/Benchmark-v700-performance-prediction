//
// Created by Ravil Galiev on 19.05.2023.
//

#ifndef SETBENCH_DATA_MAP_H
#define SETBENCH_DATA_MAP_H

template<typename K>
class DataMap {
private:
    K * data = nullptr;
    bool isNonShuffle = true;

public:
    DataMap(Parameters * _parameters)
            : KeyGeneratorData(_parameters->MAXKEY, _parameters->isNonShuffle) {
    }

    DataMap(int maxKey, bool _isNonShuffle) : isNonShuffle(_isNonShuffle) {
        if (!isNonShuffle) {
            data = new K[maxKey];
            for (size_t i = 0; i < maxKey; i++) {
                data[i] = i + 1;
            }

            std::random_shuffle(data, data + maxKey - 1);
        }
    }

    K get(size_t index) {
        return !isNonShuffle ? data[index] : index + 1;
    }

    virtual ~KeyGeneratorData() {
        delete[] data;
    }

};

#endif //SETBENCH_DATA_MAP_H
