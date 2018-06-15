//
//  catalog_manager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>
#include <exception>
#include <tuple>

#include "constant.hpp"
#include "file_manager.hpp"
#include "table_item.hpp"
#include "buffer_manager.hpp"
#include "database_exception.hpp"

class CatalogManager {
public:
    static CatalogManager shared;
    CatalogManager();
    void set_root_path(const std::string &);
    bool opened();
    void CreateTable(Table &);
    void RemoveTable(const std::string &);
    std::tuple<bool, uint32_t, int> LocateTable(const std::string &);
    
private:
    BufferManager<kBlockNumber, kBlockSize> &buffer_manager = BufferManager<kBlockNumber, kBlockSize>::shared;
    FileManager &file_manager = FileManager::shared;
    
    std::string root_path_;
    bool opened_;
    void WriteHeaderAt(const std::string &, int, uint32_t);
    uint32_t GetHeaderAt(const std::string &, int);

};

CatalogManager CatalogManager::shared = CatalogManager();

CatalogManager::CatalogManager(): opened_(false) { }

bool CatalogManager::opened() {
    return opened_;
}

void CatalogManager::WriteHeaderAt(const std::string &identifier, int id, uint32_t value) {
    using namespace std;
    if (!opened_) throw RootPathError();
    string path = root_path_ + "/" + identifier + ".header";
    fstream fs;
    fs.open(path, ios::out | ios::binary | ios::in);
    fs.seekp(id * 4);
    fs.write(Int((int) value).raw_value(), 4);
}

uint32_t CatalogManager::GetHeaderAt(const std::string &identifier, int id) {
    using namespace std;
    if (!opened_) throw RootPathError();
    string path = root_path_ + "/" + identifier + ".header";
    fstream fs;
    fs.open(path, ios::in | ios::binary);
    fs.seekg(id * 4);
    char data[4];
    fs.read(data, 4);
    return Int(data).value();
}

void CatalogManager::set_root_path(const std::string &path) {
    using namespace std;
    opened_ = true;
    root_path_ = path;
    if (!file_manager.FileExistsAt(path + "/catalog") || !file_manager.FileExistsAt(path + "/catalog.header")) {
        file_manager.CreateFileAt(path + "/catalog");
        file_manager.CreateFileAt(path + "/catalog.header");
        WriteHeaderAt("catalog", 0, -1);
        WriteHeaderAt("catalog", 1, 0);
    }
}

std::tuple<bool, uint32_t, int> CatalogManager::LocateTable(const std::string &title) {
    if (!opened_) throw RootPathError();
    
    auto valid_head = GetHeaderAt("catalog", 0);
    auto fail_tuple = std::tuple<bool, uint32_t, int>(false, 0, 0);
    buffer_manager.Open(root_path_ + "/catalog");
    for (auto i = valid_head; i != -1; i = Int(buffer_manager.Read(i)).value()) {
        const char *start = buffer_manager.Read(i), *s = start;
        auto table_total = Int(s + 4).value();
        s += 8;
        for (int j = 0; j < table_total; j++) {
            auto table = Table(s);
            if (table.title == title) {
                return std::tuple<bool, uint32_t, int>(true, i, s - start);
            }
            s += table.size();
        }
    }
    return fail_tuple;
}

void CatalogManager::CreateTable(Table &table) {
    using namespace std;
    if (!opened_) throw RootPathError();
    
    bool exists;
    tie(exists, ignore, ignore) = LocateTable(table.title);
    if (exists) throw TableAlreadyExistsError();
    
    FileManager &file_manager = FileManager::shared;
    file_manager.CreateFileAt(root_path_ + "/" + table.title + ".data");
    file_manager.CreateFileAt(root_path_ + "/" + table.title + ".data.header");
    WriteHeaderAt(table.title + ".data", 0, -1);
    WriteHeaderAt(table.title + ".data", 1, 0);
    
    string catalog_file_path = root_path_ + "/catalog";
    
    uint32_t valid_head = GetHeaderAt("catalog", 0), invalid_head = GetHeaderAt("catalog", 1);
    
    static char data[kBlockSize];
    
    uint32_t i;
    for (i = valid_head; i != -1; i = Int(buffer_manager.Read(i)).value()) {
        const char *start = buffer_manager.Read(i);
        const char *s = start;
        int total_tables = Int(s + 4).value();
        s += 8;
        for (int j = 0; j < total_tables; j++)
            s += Table(s).size();
        if (kBlockSize - (s - start) >= table.size()) break;
    }
    if (i == -1) {
        // there's no valid block that satisfies the requirments
        memcpy(data, Int(valid_head).raw_value(), 4);
        memcpy(data + 4, Int(1).raw_value(), 4);
        memcpy(data + 8, table.raw_value().data(), table.raw_value().size());
        valid_head = invalid_head;
        if (invalid_head == file_manager.FileSizeAt(catalog_file_path))
            invalid_head += kBlockSize;
        else
            invalid_head = Int(buffer_manager.Read(invalid_head)).value();
        buffer_manager.Write(valid_head, data);
    } else {
        // append one schema to block whose offset is i
        int total_blocks = Int(data + 4).value() + 1;
        memcpy(data, buffer_manager.Read(i), kBlockSize);
        memcpy(data + 4, Int(total_blocks).raw_value(), 4);
        char *s = data + 8;
        for (int j = 1; j <= total_blocks - 1; j++) {
            s += Table(s).size();
        }
        memcpy(s, table.raw_value().data(), table.size());
        buffer_manager.Write(i, data);
    }
    
    WriteHeaderAt("catalog", 0, valid_head);
    WriteHeaderAt("catalog", 1, invalid_head);
}

void CatalogManager::RemoveTable(const std::string &table_name) {
    using namespace std;
    if (!opened_) throw RootPathError();
    
    bool exists;
    int offset_in_file, offset_in_block;
    tie(exists, offset_in_file, offset_in_block) = LocateTable(table_name);
    if (!exists) throw TableNotExistsError();
    
    static char data[kBlockSize];
    memcpy(data, buffer_manager.Read(offset_in_file), kBlockSize);
    auto total = Int(data + 4).value();
    memcpy(data + 4, Int(total - 1).raw_value(), 4);
    buffer_manager.Write(offset_in_file, data);
    
    remove((root_path_ + "/" + table_name + ".data").c_str());
    remove((root_path_ + "/" + table_name + ".data.header").c_str());
}
