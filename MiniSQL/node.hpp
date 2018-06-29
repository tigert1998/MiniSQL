//
//  node.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/16.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <vector>
#include <iostream>

template <typename KeyType>
class Node {
public:
    Node() = delete;
    Node(int, int);
    Node(int, const char *);

    bool is_internal;
    const char *raw_value() const;
    
    std::vector<KeyType> keys;
    std::vector<uint64_t> children;
    int total() const;
    bool need_split() const;
    bool need_merge() const;
    std::vector<typename KeyType::OriginalType> key_values() const;
    uint64_t left_sibling, right_sibling;
    
    const Node<KeyType> &operator=(const Node<KeyType> &);
    void Print();
    
private:
    static char data[kBlockSize];
    const int key_size;
    int degree_;
    
};

template <typename KeyType>
void Node<KeyType>::Print() {
    using namespace std;
    printf("is_internal = %s\n", is_internal ? "true" : "false");
    printf("degree = %d\n", degree_);
    printf("total = %d\n", total());
    printf("left_sibling = %lld\n", left_sibling);
    printf("right_sibling = %lld\n", right_sibling);
    for (int i = 0; i < keys.size(); i++) {
        cout << "keys[" << i << "] = " << keys[i].value() << endl;
    }
    for (int i = 0; i < children.size(); i++) {
        cout << "children[" << i << "] = " << children[i] << endl;
    }
}

template <typename KeyType>
const Node<KeyType> &Node<KeyType>::operator=(const Node<KeyType> &obj) {
    is_internal = obj.is_internal;
    keys = obj.keys;
    children = obj.children;
    left_sibling = obj.left_sibling;
    right_sibling = obj.right_sibling;
    degree_ = obj.degree_;
    return obj;
}

template <typename KeyType>
std::vector<typename KeyType::OriginalType> Node<KeyType>::key_values() const {
    std::vector<typename KeyType::OriginalType> ans;
    for (auto i : keys)
        ans.push_back(i.value());
    return ans;
}

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
Node<KeyType>::Node(int key_size, int degree): key_size(key_size), degree_(degree), left_sibling(0), right_sibling(0) { }

template <>
Node<Char>::Node(int key_size, const char *data): key_size(key_size) {
    is_internal = (data[0] != 0);
    degree_ = Int(data + 1).value();
    auto total_ = Int(data + 5).value();
    for (auto _ = 0; _ < (is_internal ? total_ - 1 : total_); _++) keys.push_back(Char(key_size - 1));
    children.resize(total_);
    left_sibling = Uint64_t(data + 9).value();
    right_sibling = Uint64_t(data + 17).value();
    data += 25;
    for (int i = 0; i <= total() - 2; i++) {
        children[i] = Uint64_t(data).value();
        keys[i] = Char(key_size, data + sizeof(uint64_t));
        data += sizeof(uint64_t) + key_size;
    }
    if (total_ >= 1)
        children.back() = Uint64_t(data).value();
    if (!is_internal && total_ >= 1)
        keys.back() = Char(key_size, data + sizeof(uint64_t));
}

template <typename KeyType>
Node<KeyType>::Node(int key_size, const char *data): key_size(key_size) {
    is_internal = (data[0] != 0);
    degree_ = Int(data + 1).value();
    auto total_ = Int(data + 5).value();
    keys.resize(is_internal ? total_ - 1 : total_);
    children.resize(total_);
    left_sibling = Uint64_t(data + 9).value();
    right_sibling = Uint64_t(data + 17).value();
    data += 25;
    for (int i = 0; i <= total() - 2; i++) {
        children[i] = Uint64_t(data).value();
        keys[i] = KeyType(data + sizeof(uint64_t));
        data += sizeof(uint64_t) + key_size;
    }
    if (total_ >= 1)
        children.back() = Uint64_t(data).value();
    if (!is_internal && total_ >= 1)
        keys.back() = KeyType(data + sizeof(uint64_t));
}

template <typename KeyType>
const char *Node<KeyType>::raw_value() const {
    data[0] = (char) is_internal;
    memcpy(data + 1, Int(degree_).raw_value(), sizeof(int));
    memcpy(data + 5, Int(total()).raw_value(), sizeof(int));
    memcpy(data + 9, Uint64_t(left_sibling).raw_value(), sizeof(uint64_t));
    memcpy(data + 17, Uint64_t(right_sibling).raw_value(), sizeof(uint64_t));
    char *_data = data + 25;
    for (int i = 0; i <= total() - 2; i++) {
        memcpy(_data, Uint64_t(children[i]).raw_value(), sizeof(Uint64_t));
        memcpy(_data + sizeof(uint64_t), keys[i].raw_value(), key_size);
        _data += sizeof(uint64_t) + key_size;
    }
    memcpy(_data, Uint64_t(children.back()).raw_value(), sizeof(uint64_t));
    if (!is_internal && !keys.empty())
        memcpy(_data + sizeof(uint64_t), keys.back().raw_value(), key_size);
    return data;
}

