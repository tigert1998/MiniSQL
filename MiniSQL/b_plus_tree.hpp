//
//  b_plus_tree.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/16.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <functional>
#include <string>
#include <fstream>
#include <iostream>

#include "table_item.hpp"
#include "constant.hpp"
#include "node.hpp"

// KeyType should be Int, Char or Float
template <typename KeyType>
class BPlusTree {
public:
    BPlusTree() = delete;
    BPlusTree(int, const std::string &);
    void Insert(KeyType, uint64_t);
    int Count(KeyType);
    void Erase(KeyType);
    void Print();
    void PrintTree();
    
private:
    const std::string index_path;
    const int key_size;
    BufferManager<kBlockNumber, kBlockSize> &buffer_manager = BufferManager<kBlockNumber, kBlockSize>::shared;
    const FileManager &file_manager = FileManager::shared;
    void InsertRecurrsively(uint64_t, KeyType, uint64_t);
    int CountRecurrsively(uint64_t, KeyType);
    void SplitAtIndex(long, uint64_t);
    void MergeAtIndex(long, uint64_t);
    
    int degree() const;
    
    uint64_t root_offset();
    uint64_t invalid_head_offset();
    void set_root_offset(uint64_t);
    void set_invalid_head_offset(uint64_t);
    
    uint64_t NewBlock();
    void DeleteBlock(uint64_t);
    
    Node<KeyType> GetNodeAt(uint64_t);
    void WriteNodeAt(uint64_t, const Node<KeyType> &);
    void PrintTreeRecurrsively(uint64_t);
};

template <typename KeyType>
void BPlusTree<KeyType>::PrintTreeRecurrsively(uint64_t address) {
    using namespace std;
    auto node = GetNodeAt(address);
    cout << "[node address = " << address << "]" << endl;
    node.Print();
    if (!node.is_internal) return;
    for (auto i : node.children) PrintTreeRecurrsively(i);
}

template <typename KeyType>
void BPlusTree<KeyType>::PrintTree() {
    using namespace std;
    cout << "root_offset = " << root_offset() << endl;
    cout << "invalid_head_offset = " << invalid_head_offset() << endl;
    cout << "file size = " << file_manager.FileSizeAt(index_path) << endl;
    auto i = invalid_head_offset();
    while (i) {
        cout << "[invalid address = " << i << "]" << endl;
        i = Uint64_t(buffer_manager.Read(i)).value();
    }
    PrintTreeRecurrsively(root_offset());
}

template <typename KeyType>
void BPlusTree<KeyType>::Print() {
    using namespace std;
    auto address = root_offset();
    if (address == 0) return;
    auto node = GetNodeAt(address);
    while (node.is_internal) {
        address = node.children.front();
        node = GetNodeAt(address);
    }
    do {
        for (int i = 0; i < node.total(); i++) {
            cout << "key = " << node.keys[i].value() << ", offset = " << node.children[i] << endl;
        }
        address = node.right_sibling;
        if (address) node = GetNodeAt(address);
    } while (address);
}

template <typename KeyType>
int BPlusTree<KeyType>::CountRecurrsively(uint64_t address, KeyType key) {
    auto node = GetNodeAt(address);
    auto key_values = node.key_values();
    auto i = lower_bound(key_values.begin(), key_values.end(), key.value()) - key_values.begin();
    if (node.is_internal) {
        return CountRecurrsively(node.children[i], key);
    } else {
        return key_values[i] == key.value();
    }
}

template <typename KeyType>
int BPlusTree<KeyType>::Count(KeyType key) {
    if (root_offset() == 0) return 0;
    return CountRecurrsively(root_offset(), key);
}

template <typename KeyType>
Node<KeyType> BPlusTree<KeyType>::GetNodeAt(uint64_t address) {
    return Node<KeyType>(key_size, buffer_manager.Read(address));
}

template <typename KeyType>
void BPlusTree<KeyType>::WriteNodeAt(uint64_t address, const Node<KeyType> &node) {
    buffer_manager.Write(address, node.raw_value());
}

template <typename KeyType>
void BPlusTree<KeyType>::SplitAtIndex(long i, uint64_t address) {
    auto node = GetNodeAt(address);
    auto splited_address = node.children[i];
    auto splited_node = GetNodeAt(splited_address);
    if (!splited_node.is_internal) {
        // leaf node
        auto left_node = Node<KeyType>(key_size, degree());
        auto right_node = Node<KeyType>(key_size, degree());
        left_node.is_internal = right_node.is_internal = false;
        auto left_address = NewBlock(), right_address = NewBlock();

        auto mid = splited_node.total() / 2;
        copy(splited_node.keys.begin(), splited_node.keys.begin() + mid, back_inserter(left_node.keys));
        copy(splited_node.keys.begin() + mid, splited_node.keys.end(), back_inserter(right_node.keys));
        copy(splited_node.children.begin(), splited_node.children.begin() + mid, back_inserter(left_node.children));
        copy(splited_node.children.begin() + mid, splited_node.children.end(), back_inserter(right_node.children));

        KeyType inserted_key = splited_node.keys[mid];
        node.keys.insert(node.keys.begin() + i, inserted_key);

        node.children.erase(node.children.begin() + i);
        node.children.insert(node.children.begin() + i, right_address);
        node.children.insert(node.children.begin() + i, left_address);

        if (splited_node.left_sibling != 0) {
            auto temp = GetNodeAt(splited_node.left_sibling);
            temp.right_sibling = left_address;
            WriteNodeAt(splited_node.left_sibling, temp);
        }
        left_node.right_sibling = right_address;
        right_node.right_sibling = splited_node.right_sibling;

        left_node.left_sibling = splited_node.left_sibling;
        right_node.left_sibling = left_address;
        if (splited_node.right_sibling != 0) {
            auto temp = GetNodeAt(splited_node.right_sibling);
            temp.left_sibling = right_address;
            WriteNodeAt(splited_node.right_sibling, temp);
        }
        
        DeleteBlock(splited_address);
        WriteNodeAt(left_address, left_node);
        WriteNodeAt(right_address, right_node);
        WriteNodeAt(address, node);
    } else {
        // internal node
        auto left_node = Node<KeyType>(key_size, degree());
        auto right_node = Node<KeyType>(key_size, degree());
        auto left_address = NewBlock(), right_address = NewBlock();
        left_node.is_internal = right_node.is_internal = true;
        
        auto mid = splited_node.total() / 2;
        copy(splited_node.keys.begin(), splited_node.keys.begin() + mid, back_inserter(left_node.keys));
        copy(splited_node.children.begin(), splited_node.children.begin() + mid + 1, back_inserter(left_node.children));
        copy(splited_node.keys.begin() + mid + 1, splited_node.keys.end(), back_inserter(right_node.keys));
        copy(splited_node.children.begin() + mid + 1, splited_node.children.end(), back_inserter(right_node.children));

        KeyType inserted_key = splited_node.keys[mid];
        node.keys.insert(node.keys.begin() + i, inserted_key);

        node.children.erase(node.children.begin() + i);
        node.children.insert(node.children.begin() + i, right_address);
        node.children.insert(node.children.begin() + i, left_address);

        DeleteBlock(splited_address);
        WriteNodeAt(left_address, left_node);
        WriteNodeAt(right_address, right_node);
        WriteNodeAt(address, node);
    }
}

template <typename KeyType>
int BPlusTree<KeyType>::degree() const {
    return (kBlockSize + key_size - 25) / (sizeof(uint64_t) + key_size) - 1;
}

template <typename KeyType>
uint64_t BPlusTree<KeyType>::NewBlock() {
    using namespace std;
    auto offset = invalid_head_offset();
    if (offset == 0) {
        static char temp[kBlockSize];
        auto ans = file_manager.FileSizeAt(index_path);
        fstream fs;
        fs.open(index_path, ios::in | ios::out | ios::binary);
        fs.seekp(ans);
        fs.write(temp, kBlockSize);
        return ans;
    } else {
        auto ans = offset;
        offset = Uint64_t(buffer_manager.Read(offset)).value();
        set_invalid_head_offset(offset);
        return ans;
    }
}

template <typename KeyType>
void BPlusTree<KeyType>::DeleteBlock(uint64_t offset) {
    static char data[kBlockSize];
    memcpy(data, buffer_manager.Read(offset), kBlockSize);
    auto next_block_offset = Uint64_t(invalid_head_offset());
    memcpy(data, next_block_offset.raw_value(), sizeof(uint64_t));
    buffer_manager.Write(offset, data);
    set_invalid_head_offset(offset);
}

template <typename KeyType>
uint64_t BPlusTree<KeyType>::root_offset() {
    std::fstream fs;
    fs.open(index_path, std::ios::in | std::ios::binary);
    fs.seekg(0);
    char data[sizeof(uint64_t)];
    fs.read(data, sizeof(uint64_t));
    return Uint64_t(data).value();
}

template <typename KeyType>
void BPlusTree<KeyType>::set_root_offset(uint64_t offset) {
    std::fstream fs;
    fs.open(index_path, std::ios::in | std::ios::binary | std::ios::out);
    fs.seekg(0);
    fs.write(Uint64_t(offset).raw_value(), sizeof(uint64_t));
}

template <typename KeyType>
uint64_t BPlusTree<KeyType>::invalid_head_offset() {
    std::fstream fs;
    fs.open(index_path, std::ios::in | std::ios::binary);
    fs.seekg(sizeof(uint64_t));
    char data[sizeof(uint64_t)];
    fs.read(data, sizeof(uint64_t));
    return Uint64_t(data).value();
}

template <typename KeyType>
void BPlusTree<KeyType>::set_invalid_head_offset(uint64_t offset) {
    std::fstream fs;
    fs.open(index_path, std::ios::in | std::ios::binary | std::ios::out);
    fs.seekg(sizeof(uint64_t));
    fs.write(Uint64_t(offset).raw_value(), sizeof(uint64_t));
}

template <typename KeyType>
BPlusTree<KeyType>::BPlusTree(int key_size, const std::string &index_path): key_size(key_size), index_path(index_path) { }

template <typename KeyType>
void BPlusTree<KeyType>::Insert(KeyType key, uint64_t offset) {
    using namespace std;
    buffer_manager.Open(index_path);
    auto root_offset_ = root_offset();
    if (root_offset_ == 0) {
        Node<KeyType> node(key_size, degree());
        node.is_internal = false;
        node.children.resize(1);
        node.keys.resize(1);
        node.keys[0] = key;
        node.children[0] = offset;
        auto address = NewBlock();
        WriteNodeAt(address, node);
        set_root_offset(address);
    } else {
        InsertRecurrsively(root_offset_, key, offset);
        auto root_node = GetNodeAt(root_offset_);
        // cout << "root_node total = " << root_node.total() << endl;
        if (root_node.need_split()) {
            auto address = NewBlock();
            Node<KeyType> node(key_size, degree());
            node.is_internal = true;
            node.children.resize(1);
            node.children[0] = root_offset();
            WriteNodeAt(address, node);
            SplitAtIndex(0, address);
            set_root_offset(address);
        }
    }
    buffer_manager.Flush();
    // cout << "file size = " << file_manager.FileSizeAt(index_path) << endl;
}

template <typename KeyType>
void BPlusTree<KeyType>::InsertRecurrsively(uint64_t address, KeyType key, uint64_t offset) {
    using namespace std;
    auto node = GetNodeAt(address);
    auto key_values = node.key_values();
    if (node.is_internal) {
        auto i = lower_bound(key_values.begin(), key_values.end(), key.value()) - key_values.begin();
        InsertRecurrsively(node.children[i], key, offset);
        if (GetNodeAt(node.children[i]).need_split()) {
            SplitAtIndex(i, address);
        }
    } else {
        auto i = lower_bound(key_values.begin(), key_values.end(), key.value()) - key_values.begin();
        node.keys.insert(node.keys.begin() + i, key);
        node.children.insert(node.children.begin() + i, offset);
        WriteNodeAt(address, node);
    }
}
