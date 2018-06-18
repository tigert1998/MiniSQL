//
//  index_manager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>

#include "b_plus_tree.hpp"
#include "database_exception.hpp"
#include "file_manager.hpp"
#include "data_type.hpp"
#include "json_manager.hpp"

class IndexManager {
public:
    static IndexManager shared;
    IndexManager();
    IndexManager(const std::string &);
    void CreatePrimaryIndex(const std::string &) const;
    void CreateIndex(const std::string &, const std::string &, const std::string &) const;
    void RemoveIndex(const std::string &) const;
    void InsertRecordIntoIndices(const Record &, uint64_t) const;
    void RemoveRecordFromIndices(const Record &) const;
    template <typename KeyType> void Query(const Table &, const Predicate<KeyType> &, std::function<void(uint64_t)>) const;
    void set_root_path(const std::string &);
    
private:
    const FileManager &file_manager = FileManager::shared;
    const JSONManager &json_manager = JSONManager::shared;
    std::string root_path_;
    bool is_valid_;
    void InitIndexFile(const std::string &) const;
    const std::string json_path() const;
    const std::string GetIndexName(const std::string &, const std::string &) const;
    
};

IndexManager IndexManager::shared = IndexManager();

IndexManager::IndexManager(): is_valid_(false) { }

IndexManager::IndexManager(const std::string &root_path): is_valid_(true), root_path_(root_path) { }

template <typename KeyType>
void IndexManager::Query(const Table &table, const Predicate<KeyType> &predicate, std::function<void(uint64_t)> yield) const {
    using namespace std;
    if (!is_valid_) throw RootPathError();
    int i;
    for (i = 0; i < table.columns.size(); i++)
        if (table.columns[i].title == predicate.column_name) break;
    if (i == table.columns.size()) throw ColumnNotExistsError();
    if (!table.columns[i].is_indexed) throw IndexNotExistsError();
    if (table.columns[i].is_primary) {
        string index_path = root_path_ + "/" + table.title + ".index";
        BPlusTree<KeyType> t((int) table.columns[i].size, index_path);
        t.Query(predicate, yield);
    } else {
        string index_name = GetIndexName(table.title, predicate.column_name);
        string index_path = root_path_ + "/" + table.title + "." + index_name + ".index";
        BPlusTree<KeyType> t((int) table.columns[i].size, index_path);
        t.Query(predicate, yield);
    }
}

const std::string IndexManager::GetIndexName(const std::string &table_name, const std::string &column_name) const {
    // if there's no corresponding index, return ""
    using namespace std;
    using namespace rapidjson;
    auto doc = json_manager.ImportJSON(json_path());
    
    auto index_json = doc["index"].GetObject();
    string index_name = "";
    for (auto &m : index_json) {
        auto specific_json = m.value.GetObject();
        if (strcmp(table_name.c_str(), specific_json["table"].GetString()) != 0) continue;
        if (strcmp(column_name.c_str(), specific_json["column"].GetString()) != 0) continue;
        index_name = m.name.GetString();
    }
    return index_name;
}

void IndexManager::InsertRecordIntoIndices(const Record &record, uint64_t offset) const {
    using namespace std;
    if (!is_valid_) throw RootPathError();
    
    auto insert_into_single_index = [&](Column c, const std::string &index_path, int i) {
        if (c.type == DataTypeIdentifier::Int) {
            BPlusTree<Int> t(sizeof(int), index_path);
            auto key = record.Get<Int>(i);
            t.Insert(key, offset);
        } else if (c.type == DataTypeIdentifier::Char) {
            BPlusTree<Char> t((int) c.size, index_path);
            auto key = record.Get<Char>(i);
            t.Insert(key, offset);
        } else if (c.type == DataTypeIdentifier::Float) {
            BPlusTree<Float> t(sizeof(float), index_path);
            auto key = record.Get<Float>(i);
            t.Insert(key, offset);
        }
    };
    
    for (int i = 0; i < record.schema.columns.size(); i++) {
        Column c = record.schema.columns[i];
        if (!c.is_indexed) continue;
        if (c.is_primary) {
            string index_path = root_path_ + "/" + record.schema.title + ".index";
            insert_into_single_index(c, index_path, i);
        } else {
            string index_name = GetIndexName(record.schema.title, c.title);
            if (index_name == "") continue;
            string index_path = root_path_ + "/" + record.schema.title + "." + index_name + ".index";
            insert_into_single_index(c, index_path, i);
        }
    }
}

void IndexManager::RemoveRecordFromIndices(const Record &record) const {
    using namespace std;
    if (!is_valid_) throw RootPathError();
    
    auto remove_from_single_index = [&](Column c, const std::string &index_path, int i) {
        if (c.type == DataTypeIdentifier::Int) {
            BPlusTree<Int> t(sizeof(int), index_path);
            auto key = record.Get<Int>(i);
            t.Erase(key);
        } else if (c.type == DataTypeIdentifier::Char) {
            BPlusTree<Char> t((int) c.size, index_path);
            auto key = record.Get<Char>(i);
            t.Erase(key);
        } else if (c.type == DataTypeIdentifier::Float) {
            BPlusTree<Float> t(sizeof(float), index_path);
            auto key = record.Get<Float>(i);
            t.Erase(key);
        }
    };
    
    for (int i = 0; i < record.schema.columns.size(); i++) {
        Column c = record.schema.columns[i];
        if (c.is_primary) {
            string index_path = root_path_ + "/" + record.schema.title + ".index";
            remove_from_single_index(c, index_path, i);
        } else {
            string index_name = GetIndexName(record.schema.title, c.title);
            if (index_name == "") continue;
            string index_path = root_path_ + "/" + record.schema.title + "." + index_name + ".index";
            remove_from_single_index(c, index_path, i);
        }
    }
}

const std::string IndexManager::json_path() const {
    return root_path_ + "/catalog.json";
}

void IndexManager::InitIndexFile(const std::string &index_path) const {
    using namespace std;
    fstream fs;
    fs.open(index_path, ios::out | ios::binary);
    static char data[sizeof(uint64_t) * 2];
    memcpy(data, Uint64_t(static_cast<uint64_t>(0)).raw_value(), sizeof(uint64_t));
    memcpy(data + sizeof(uint64_t), Uint64_t(static_cast<uint64_t>(0)).raw_value(), sizeof(uint64_t));
    fs.write(data, sizeof(uint64_t) * 2);
}

void IndexManager::CreatePrimaryIndex(const std::string &table_name) const {
    // create table_name.index
    if (!is_valid_) throw RootPathError();
    auto index_path = root_path_ + "/" + table_name + ".index";
    InitIndexFile(index_path);
}

void IndexManager::CreateIndex(const std::string &index_name, const std::string &table_name, const std::string &column_name) const {
    // create table_name.index_name.index
    using namespace rapidjson;
    if (!is_valid_) throw RootPathError();
    auto index_path = root_path_ + "/" + table_name + "." + index_name + ".index";
    auto doc = json_manager.ImportJSON(json_path());
    if (doc["index"].HasMember(index_name.c_str())) throw IndexAlreadyExistsError();
    if (!doc["table"].HasMember(table_name.c_str())) throw TableNotExistsError();
    if (!doc["table"].GetObject()[table_name.c_str()].GetObject().HasMember(column_name.c_str())) throw ColumnNotExistsError();
    if (doc["table"].GetObject()[table_name.c_str()].GetObject()[column_name.c_str()].GetObject()["is_indexed"].GetBool()) throw ColumnAlreadyIndexedError();
    
    auto &alloc = doc.GetAllocator();
    Value index_json(kObjectType);
    index_json.AddMember("table", StringRef(table_name.c_str()), alloc);
    index_json.AddMember("column", StringRef(column_name.c_str()), alloc);
    doc["index"].GetObject().AddMember(StringRef(index_name.c_str()), index_json, alloc);
    
    auto column_json = doc["table"].GetObject()[table_name.c_str()].GetObject()[column_name.c_str()].GetObject();
    column_json.RemoveMember("is_indexed");
    column_json.AddMember("is_indexed", true, alloc);
    json_manager.ExportJSON(json_path(), doc);

    InitIndexFile(index_path);
}

void IndexManager::RemoveIndex(const std::string &index_name) const {
    // remove table_name.index_name.index
    using namespace std;
    using namespace rapidjson;
    if (!is_valid_) throw RootPathError();
    auto doc = json_manager.ImportJSON(json_path());
    auto &alloc = doc.GetAllocator();
    if (!doc["index"].GetObject().HasMember(index_name.c_str())) throw IndexNotExistsError();
    
    auto index_json = doc["index"].GetObject()[index_name.c_str()].GetObject();
    string table_name = index_json["table"].GetString();
    string column_name = index_json["column"].GetString();
    doc["index"].GetObject().RemoveMember(index_name.c_str());
    
    file_manager.RemoveFileAt(root_path_ + "/" + table_name + "." + index_name + ".index");
    auto column_json = doc["table"].GetObject()[table_name.c_str()].GetObject()[column_name.c_str()].GetObject();
    column_json.RemoveMember("is_indexed");
    column_json.AddMember("is_indexed", false, alloc);
    json_manager.ExportJSON(json_path(), doc);
}

void IndexManager::set_root_path(const std::string &root_path) {
    root_path_ = root_path;
    is_valid_ = true;
}


