//
//  node.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/16.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <vector>

template <typename KeyType>
class Node {
public:
    Node() = delete;
    Node(int, int);
    Node(int, const char *);

    bool is_internal;
    const char *raw_value();
    
    std::vector<KeyType> keys;
    std::vector<uint64_t> children;
    
private:
    static char data[kBlockSize];
    const int key_size;
    int degree_;
    
};

template <typename KeyType>
char Node<KeyType>::data[kBlockSize] = {};

template <>
Node<Char>::Node(int key_size, int degree): key_size(key_size), degree_(degree), children(degree) {
    for (auto _ = 0; _ < degree - 1; _++) {
        keys.push_back(Char(key_size - 1));
    }
}

template <typename KeyType>
Node<KeyType>::Node(int key_size, int degree): key_size(key_size), degree_(degree), children(degree), keys(degree - 1) { }

template <>
Node<Char>::Node(int key_size, const char *data): key_size(key_size) {
    is_internal = (data[0] != 0);
    degree_ = Int(data[1]).value();
    for (auto _ = 0; _ < degree_ - 1; _++) keys.push_back(Char(key_size - 1));
    children.resize(degree_);
    data += 5;
    for (int i = 0; i <= degree_ - 2; i++) {
        children[i] = Uint64_t(data).value();
        keys[i] = Char(key_size, data + sizeof(uint64_t));
        data += sizeof(uint64_t) + key_size;
    }
    children.back() = Uint64_t(data).value();
}

template <typename KeyType>
Node<KeyType>::Node(int key_size, const char *data): key_size(key_size) {
    is_internal = (data[0] != 0);
    degree_ = Int(data[1]).value();
    keys.resize(degree_ - 1);
    children.resize(degree_);
    data += 5;
    for (int i = 0; i <= degree_ - 2; i++) {
        children[i] = Uint64_t(data).value();
        keys[i] = KeyType(data + sizeof(uint64_t));
        data += sizeof(uint64_t) + key_size;
    }
    children.back() = Uint64_t(data).value();
}

template <typename KeyType>
const char *Node<KeyType>::raw_value() {
    data[0] = (char) is_internal;
    memcpy(data + 1, Int(degree_).raw_value(), sizeof(int));
    char *_data = data + 5;
    for (int i = 0; i <= degree_ - 2; i++) {
        memcpy(_data, Uint64_t(children[i]).raw_value(), sizeof(Uint64_t));
        memcpy(_data + sizeof(uint64_t), keys[i].raw_value(), key_size);
        _data += sizeof(uint64_t) + key_size;
    }
    memcpy(_data, Uint64_t(children.back()).raw_value(), sizeof(uint64_t));
    return data;
}

