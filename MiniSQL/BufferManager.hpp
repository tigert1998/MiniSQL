//
//  BufferManager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <list>

// H is the total number of blocks in Buffer
// S is the size (count in bytes) of one block
template <int H, int S = 4096>
class BufferManager {
public:
    BufferManager();
    void Open(const char *);
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
    fs.seekg(offset[h]);
    fs.write(buffer[h], S);
    modified[h] = false;
}

template <int H, int S>
void BufferManager<H, S>::Open(const char *file_name) {
    using std::ios;
    for (int i = 0; i < valid_total; i++)
        if (modified[i]) {
            FlushBufferAtIndex(i);
        }
    position.clear();
    valid_total = 0;
    left[H] = right[H] = H;
    
    fs.open(file_name, ios::in);
    if (!fs.is_open()) {
        fs.open(file_name, ios::out);
    }
    fs.open(file_name, ios::in | ios::out | ios::binary);
}

template <int H, int S>
const char *BufferManager<H, S>::Read(uint32_t offset_in_file) {
    auto read_into_buffer = [&](uint32_t offset_in_file, int h) {
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
    Read(offset_in_file);
    int h = position[offset_in_file];
    copy(data, data + S, buffer[h]);
    modified[h] = true;
}
