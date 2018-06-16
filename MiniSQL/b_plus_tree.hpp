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

// KeyType should be Int, Char or Float
template <typename KeyType>
class BPlusTree {
public:
    BPlusTree() = delete;
    BPlusTree(std::function<KeyType(Record)>, const std::string &);
    void Insert(Record);
    void Erase(KeyType);
    
private:
    const std::function<KeyType(Record)> get_key;
    const std::string index_path;
    BufferManager<kBlockNumber> &buffer_manager = BufferManager<kBlockNumber>::shared;
    const FileManager &file_manager = FileManager::shared;
    void InsertRecurrsively(uint64_t, Record);
    
    uint64_t root_offset();
    uint64_t invalid_head_offset();
    void set_root_offset(uint64_t);
    void set_invalid_head_offset(uint64_t);
    
    uint64_t NewBlock();
    void DeleteBlock(uint64_t);
    
};

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
    memcpy(data, Uint64_t(invalid_head_offset()).raw_value(), sizeof(uint64_t));
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
BPlusTree<KeyType>::BPlusTree(std::function<KeyType(Record)> get_key, const std::string &index_path): get_key(get_key), index_path(index_path) { }

template <typename KeyType>
void BPlusTree<KeyType>::Insert(Record record) {
    if (root_offset() == 0) {
        
    } else {
        
    }
}

template <typename KeyType>
void BPlusTree<KeyType>::InsertRecurrsively(uint64_t offset, Record record) {
    
}
