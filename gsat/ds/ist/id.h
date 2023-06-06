#pragma once

#include <cstdint>

#define BUILD_ID int j = 0;                                                                             \
for (int i = 0; i < this->id_size; ++i) {                                                               \
    Key cur = this->left + (i + 1) * (static_cast<int64_t>(this->right) - this->left) / this->id_size;  \
    while (j < this->rep_size && this->rep[j] < cur) {                                                  \
        ++j;                                                                                            \
    }                                                                                                   \
    this->id[i] = j;                                                                                    \
}                                                                                                       \

#define ID_LOCATE int j = (static_cast<int64_t>(key) - this->left) * this->id_size / (static_cast<int64_t>(this->right) - this->left);  \
l = j == 0 ? -1 : this->id[j - 1] - 1;                                                                                                  \
r = this->id[j];                                                                                                                        \
