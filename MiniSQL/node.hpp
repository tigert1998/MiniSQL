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
    Node(int, const char *);

    bool is_internal();
    const char *raw_value();
    
    std::vector<KeyType> keys;
    std::vector<uint64_t> children;
    
private:
    const int key_size;
    bool is_internal_;
    int degree;
    
};

template <typename KeyType>
Node<KeyType>::Node(int key_size, const char *data): key_size(key_size) {
    is_internal_ = (data[0] != 0);
    degree = Int(data[1]).value();
    keys.resize(degree - 1);
    children.resize(degree);
    data += 5;
    for (int i = 0; i <= degree - 2; i++) {
        children[i] = Uint64_t(data).value();
        keys[i] = KeyType(key_size, data + sizeof(uint64_t)).value();
        data += sizeof(uint64_t) + key_size;
    }
    children[degree - 1] = Uint64_t(data).value();
}

template <typename KeyType>
const char *Node<KeyType>::raw_value() {
    static char data[kBlockSize];
    data[0] = (char) is_internal_;
}

template <typename KeyType>
bool Node<KeyType>::is_internal() {
    return is_internal_;
}
