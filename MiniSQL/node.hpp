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
    int total() const;
    bool need_split() const;
    bool need_merge() const;
    
private:
    static char data[kBlockSize];
    const int key_size;
    int degree_;
    
};

template <typename KeyType>
int Node<KeyType>::total() const {
    return (int) children.size();
}

template <typename KeyType>
bool Node<KeyType>::need_split() const {
    if (is_internal) {
        return total() > degree_;
    } else {
        return total() > degree_ - 1;
    }
}

template <typename KeyType>
bool Node<KeyType>::need_merge() const {
    if (is_internal) {
        return total() < (degree_ + 1) / 2;
    } else {
        return total() < degree_ / 2;
    }
}

template <typename KeyType>
char Node<KeyType>::data[kBlockSize] = {};

template <typename KeyType>
Node<KeyType>::Node(int key_size, int degree): key_size(key_size), degree_(degree) { }

template <>
Node<Char>::Node(int key_size, const char *data): key_size(key_size) {
    is_internal = (data[0] != 0);
    degree_ = Int(data + 1).value();
    auto total_ = Int(data + 5).value();
    for (auto _ = 0; _ < (is_internal ? total_ - 1 : total_); _++) keys.push_back(Char(key_size - 1));
    children.resize(total_);
    data += 9;
    for (int i = 0; i <= total() - 2; i++) {
        children[i] = Uint64_t(data).value();
        keys[i] = Char(key_size, data + sizeof(uint64_t));
        data += sizeof(uint64_t) + key_size;
    }
    children.back() = Uint64_t(data).value();
    if (!is_internal)
        keys.back() = Char(key_size, data + sizeof(uint64_t));
}

template <typename KeyType>
Node<KeyType>::Node(int key_size, const char *data): key_size(key_size) {
    is_internal = (data[0] != 0);
    degree_ = Int(data + 1).value();
    auto total_ = Int(data + 5).value();
    keys.resize(is_internal ? total_ - 1 : total_);
    children.resize(total_);
    data += 9;
    for (int i = 0; i <= total() - 2; i++) {
        children[i] = Uint64_t(data).value();
        keys[i] = KeyType(data + sizeof(uint64_t));
        data += sizeof(uint64_t) + key_size;
    }
    children.back() = Uint64_t(data).value();
    if (!is_internal)
        keys.back() = KeyType(data + sizeof(uint64_t));
}

template <typename KeyType>
const char *Node<KeyType>::raw_value() {
    data[0] = (char) is_internal;
    memcpy(data + 1, Int(degree_).raw_value(), sizeof(int));
    memcpy(data + 5, Int(total()).raw_value(), sizeof(int));
    char *_data = data + 9;
    for (int i = 0; i <= total() - 2; i++) {
        memcpy(_data, Uint64_t(children[i]).raw_value(), sizeof(Uint64_t));
        memcpy(_data + sizeof(uint64_t), keys[i].raw_value(), key_size);
        _data += sizeof(uint64_t) + key_size;
    }
    memcpy(_data, Uint64_t(children.back()).raw_value(), sizeof(uint64_t));
    if (!is_internal)
        memcpy(_data + sizeof(uint64_t), keys.back().raw_value(), key_size);
    return data;
}

