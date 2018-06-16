//
//  buffer_manager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <algorithm>
#include <unordered_map>
#include <list>
#include <string>

#include "file_manager.hpp"

// H is the total number of blocks in Buffer
// S is the size (count in bytes) of one block
template <int H, int S = 8192>
class BufferManager {
public:
    static BufferManager<H, S> shared;
    
    BufferManager();
    ~BufferManager();
    void Open(const std::string &);
    const char *Read(uint32_t);
    void Write(uint32_t, const char *);
    
private:
    void FlushBufferAtIndex(int);
    void ListPullIndex(int);
    void ListPushFront(int);
    void ListRemoveIndex(int);
    
    char buffer[H][S];
    bool modified[H];
    uint32_t offset[H];
    std::fstream fs;
    std::unordered_map<uint32_t, int> position;
    
    int valid_total, left[H + 1], right[H + 1];
};

template <int H, int S>
BufferManager<H, S> BufferManager<H, S>::shared = BufferManager<H, S>();

template <int H, int S>
BufferManager<H, S>::~BufferManager() {
    if (fs.is_open()) {
        for (int i = 0; i < valid_total; i++)
            if (modified[i]) {
                FlushBufferAtIndex(i);
            }
    }
}

template <int H, int S>
void BufferManager<H, S>::ListPullIndex(int h) {
    ListRemoveIndex(h);
    ListPushFront(h);
}

template <int H, int S>
void BufferManager<H, S>::ListPushFront(int h) {
    left[h] = H;
    right[h] = right[H];
    left[right[H]] = h;
    right[H] = h;
}

template <int H, int S>
void BufferManager<H, S>::ListRemoveIndex(int h) {
    right[left[h]] = right[h];
    left[right[h]] = left[h];
}

template <int H, int S>
BufferManager<H, S>::BufferManager() : valid_total(0) { }

template <int H, int S>
void BufferManager<H, S>::FlushBufferAtIndex(int h) {
    fs.seekp(offset[h]);
    fs.write(buffer[h], S);
    fs.flush();
    modified[h] = false;
}

template <int H, int S>
void BufferManager<H, S>::Open(const std::string &file_name) {
    using namespace std;
    if (fs.is_open()) {
        for (int i = 0; i < valid_total; i++)
            if (modified[i])
                FlushBufferAtIndex(i);
        position.clear();
        valid_total = 0;
        left[H] = right[H] = H;
        fs.close();
    }
    
    const FileManager &file_manager = FileManager::shared;
    if (!file_manager.FileExistsAt(file_name)) {
        file_manager.CreateFileAt(file_name);
    }
    fs.open(file_name, ios::in | ios::out | ios::binary);
}

template <int H, int S>
const char *BufferManager<H, S>::Read(uint32_t offset_in_file) {
    using namespace std;
    auto read_into_buffer = [&](uint32_t offset_in_file, int h) {
        fs.seekg(offset_in_file);
        fs.read(buffer[h], S);
        modified[h] = false;
        offset[h] = offset_in_file;
        position[offset_in_file] = h;
        ListPushFront(h);
    };
    
    if (position.count(offset_in_file)) {
        int h = position[offset_in_file];
        ListPullIndex(h);
        return buffer[h];
    }
    if (valid_total < H) {
        int h = valid_total++;
        read_into_buffer(offset_in_file, h);
        return buffer[h];
    }
    int h = left[H];
    if (modified[h])
        FlushBufferAtIndex(h);
    position.erase(offset[h]);
    
    read_into_buffer(offset_in_file, h);
    return buffer[h];
}

template <int H, int S>
void BufferManager<H, S>::Write(uint32_t offset_in_file, const char *data) {
    using namespace std;
    int h;
    if (position.count(offset_in_file)) {
        h = position[offset_in_file];
        ListPullIndex(h);
    } else if (valid_total < H) {
        h = valid_total++;
        ListPullIndex(h);
    } else {
        h = left[H];
        ListPullIndex(h);
        if (modified[h])
            FlushBufferAtIndex(h);
        position.erase(offset[h]);
        position[offset_in_file] = h;
    }
    offset[h] = offset_in_file;
    copy(data, data + S, buffer[h]);
    modified[h] = true;
}
