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
    void Erase(KeyType);
    
private:
    const std::string index_path;
    const int key_size;
    BufferManager<kBlockNumber> &buffer_manager = BufferManager<kBlockNumber>::shared;
    const FileManager &file_manager = FileManager::shared;
    void InsertRecurrsively(uint64_t, KeyType, uint64_t);
    
    int degree() const;
    
    uint64_t root_offset();
    uint64_t invalid_head_offset();
    void set_root_offset(uint64_t);
    void set_invalid_head_offset(uint64_t);
    
    uint64_t NewBlock();
    void DeleteBlock(uint64_t);
    
};

template <typename KeyType>
int BPlusTree<KeyType>::degree() const {
    return (kBlockSize + key_size - 9) / (sizeof(uint64_t) + key_size) - 1;
}

template <typename KeyType>
uint64_t BPlusTree<KeyType>::NewBlock() {
    auto offset = invalid_head_offset();
    if (offset == 0) {
        return file_manager.FileSizeAt(index_path);
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
    buffer_manager.Open(index_path);
    if (root_offset() == 0) {
        Node<KeyType> node(key_size, degree());
        node.is_internal = false;
        node.children.resize(1);
        node.keys[0] = key;
        node.children[0] = offset;
        auto address = NewBlock();
        buffer_manager.Write(address, node.raw_value());
        set_root_offset(address);
    } else {
        InsertRecurrsively(root_offset(), key, offset);
    }
}

template <typename KeyType>
void BPlusTree<KeyType>::InsertRecurrsively(uint64_t address, KeyType key, uint64_t offset) {
    auto node = Node<Int>(key_size, buffer_manager.Read(address));
}
