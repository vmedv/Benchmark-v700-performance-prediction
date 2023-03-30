#pragma once

template <typename K, typename V>
struct SplayNode {
    const K key;
    V value;
    SplayNode* left = nullptr;
    SplayNode* right = nullptr;
    SplayNode* parent = nullptr;

    SplayNode(const K& key, const V& value)
        : key(key)
        , value(value) {
    }
};
