//
//  record_manager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/15.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include "table_item.hpp"
#include "buffer_manager.hpp"
#include "constant.hpp"

class RecordBlock {
public:
    RecordBlock() { }
    RecordBlock(const Table &schema, const char *data) {
        next = Uint64_t(data).value();
        int total_ = Int(data + sizeof(uint64_t)).value();
        data += sizeof(uint64_t) + sizeof(int);
        for (int i = 0; i < total_; i++) {
            records.emplace_back(schema, data);
            data += records.back().size();
        }
    }
    
    int total() const {
        return (int) records.size();
    }
    const char *raw_value() const {
        static char data[kBlockSize];
        memcpy(data, Uint64_t(next).raw_value(), sizeof(uint64_t));
        memcpy(data + sizeof(uint64_t), Int(total()).raw_value(), sizeof(int));
        char *s = data + sizeof(uint64_t) + sizeof(int);
        for (const Record &r : records) {
            memcpy(s, r.raw_value(), r.size());
            s += r.size();
        }
        return data;
    }
    
    uint64_t next;
    std::vector<Record> records;
    
};

class RecordManager {
public:
    RecordManager() = delete;
    RecordManager(const Table &, const std::string &);
    uint64_t Insert(const Record &);
    template <typename KeyType> void Erase(uint64_t, KeyType, std::function<bool(KeyType, Record)>);
    template <typename KeyType> Record GetRecord(uint64_t, KeyType, std::function<bool(KeyType, Record)>);
    void PrintFile(std::function<void(Record)>);
    
private:
    const FileManager &file_manager = FileManager::shared;
    BufferManager<kBlockNumber, kBlockSize> buffer_manager;
    
    const Table &schema;
    const std::string data_path;
    
    int maximum_record() const;
    uint64_t invalid_head_offset() const;
    uint64_t valid_head_offset() const;
    void set_invalid_head_offset(uint64_t) const;
    void set_valid_head_offset(uint64_t) const;
    
    uint64_t NewBlock();
    void DeleteBlock(uint64_t) const;

    RecordBlock GetBlockAt(uint64_t);
    void WriteBlockAt(uint64_t, const RecordBlock &);
    
};

RecordManager::RecordManager(const Table &schema, const std::string &data_path): schema(schema), buffer_manager(data_path), data_path(data_path) { }

void RecordManager::PrintFile(std::function<void(Record)> print) {
    using namespace std;
    cout << "valid head offset = " << valid_head_offset() << endl;
    cout << "invalid head offset = " << invalid_head_offset() << endl;
    auto address = valid_head_offset();
    while (address) {
        auto node = GetBlockAt(address);
        cout << "[valid address = " << address << "]" << endl;
        cout << "next = " << node.next << endl;
        cout << "total = " << node.total() << endl;
        for (auto record : node.records) print(record);
        address = node.next;
    }
    address = invalid_head_offset();
    while (address) {
        auto node = GetBlockAt(address);
        cout << "[invalid address = " << address << "]" << endl;
        cout << "next = " << node.next << endl;
        cout << "total = " << node.total() << endl;
        address = node.next;
    }
}

uint64_t RecordManager::Insert(const Record &record) {
    if (valid_head_offset() == 0) {
        auto address = NewBlock();
        RecordBlock node;
        node.next = 0;
        node.records.emplace_back(record);
        WriteBlockAt(address, node);
        set_valid_head_offset(address);
        return address;
    } else {
        auto address = valid_head_offset();
        auto node = GetBlockAt(address);
        if (node.records.size() < maximum_record()) {
            node.records.push_back(record);
            WriteBlockAt(address, node);
            return address;
        } else {
            auto new_address = NewBlock();
            RecordBlock new_node;
            new_node.next = address;
            new_node.records.emplace_back(record);
            WriteBlockAt(new_address, new_node);
            set_valid_head_offset(new_address);
            return new_address;
        }
    }
}

template <typename KeyType>
void RecordManager::Erase(uint64_t offset, KeyType key, std::function<bool(KeyType, Record)> match) {
    auto node = GetBlockAt(offset);
    int i;
    for (i = 0; i < node.total(); i++) if (match(key, node.records[i])) break;
    assert(i < node.total());
    node.records.erase(node.records.begin() + i);
    if (node.total() == 0) {
        node.next = invalid_head_offset();
        set_invalid_head_offset(offset);
        WriteBlockAt(offset, node);
    } else {
        WriteBlockAt(offset, node);
    }
}

template <typename KeyType>
Record RecordManager::GetRecord(uint64_t offset, KeyType key, std::function<bool(KeyType, Record)> match) {
    using namespace std;
    auto node = GetBlockAt(offset);
    int i;
    for (i = 0; i < node.total(); i++) {
        cout << "id = " << node.records[i].Get<Int>(0).value() << endl;
        cout << "weight = " << node.records[i].Get<Float>(1).value() << endl;
        cout << "name = " << node.records[i].Get<Char>(2).value() << endl;
        cout << "score = " << node.records[i].Get<Int>(3).value() << endl;
        if (match(key, node.records[i])) break;
    }
        
    assert(i < node.total());
    return node.records[i];
}

int RecordManager::maximum_record() const {
    return (kBlockSize - 12) / schema.size();
}

uint64_t RecordManager::invalid_head_offset() const {
    using namespace std;
    fstream fs;
    fs.open(data_path, ios::in | ios::binary);
    fs.seekg(sizeof(uint64_t));
    static char data[sizeof(uint64_t)];
    fs.read(data, sizeof(uint64_t));
    return Uint64_t(data).value();
}

uint64_t RecordManager::valid_head_offset() const {
    using namespace std;
    fstream fs;
    fs.open(data_path, ios::in | ios::binary);
    fs.seekg(0);
    static char data[sizeof(uint64_t)];
    fs.read(data, sizeof(uint64_t));
    return Uint64_t(data).value();
}

void RecordManager::set_invalid_head_offset(uint64_t offset) const {
    using namespace std;
    fstream fs;
    fs.open(data_path, ios::in | ios::out | ios::binary);
    fs.seekp(sizeof(uint64_t));
    fs.write(Uint64_t(offset).raw_value(), sizeof(uint64_t));
}

void RecordManager::set_valid_head_offset(uint64_t offset) const {
    using namespace std;
    fstream fs;
    fs.open(data_path, ios::in | ios::out | ios::binary);
    fs.seekp(0);
    fs.write(Uint64_t(offset).raw_value(), sizeof(uint64_t));
}

uint64_t RecordManager::NewBlock() {
    using namespace std;
    uint64_t ans = invalid_head_offset();
    if (ans == 0) {
        static char data[kBlockSize];
        fstream fs;
        fs.open(data_path, ios::in | ios::out | ios::binary);
        fs.seekp(0, ios::end);
        ans = fs.tellp();
        fs.write(data, kBlockSize);
        return ans;
    } else {
        set_invalid_head_offset(GetBlockAt(ans).next);
        return ans;
    }
}

RecordBlock RecordManager::GetBlockAt(uint64_t offset) {
    return RecordBlock(schema, buffer_manager.Read(offset));
}

void RecordManager::WriteBlockAt(uint64_t offset, const RecordBlock &node) {
    buffer_manager.Write(offset, node.raw_value());
}
