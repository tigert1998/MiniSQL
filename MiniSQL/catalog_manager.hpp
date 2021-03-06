//
//  catalog_manager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>
#include <fstream>
#include <exception>
#include <vector>
#include <cstring>

#include "file_manager.hpp"
#include "table_item.hpp"
#include "database_exception.hpp"
#include "json_manager.hpp"

class CatalogManager {
public:
    static CatalogManager shared;
    void set_root_path(const std::string &);
    void CreateTable(const Table &) const;
    void RemoveTable(const std::string &) const;
    Table GetTable(const std::string &) const;
    
private:
    const FileManager &file_manager = FileManager::shared;
    const JSONManager &json_manager = JSONManager::shared;
    std::string root_path_;
    bool is_valid_;
    
    bool is_valid() const;
    const std::string json_path() const;
    void ExportJSON(const rapidjson::Document &) const;
    rapidjson::Document ImportJSON() const;
};

CatalogManager CatalogManager::shared = CatalogManager();

void CatalogManager::ExportJSON(const rapidjson::Document &doc) const {
    json_manager.ExportJSON(json_path(), doc);
}

rapidjson::Document CatalogManager::ImportJSON() const {
    return json_manager.ImportJSON(json_path());
}

const std::string CatalogManager::json_path() const {
    return root_path_ + "/catalog.json";
}

void CatalogManager::set_root_path(const std::string &root_path) {
    using namespace rapidjson;
    root_path_ = root_path;
    is_valid_ = true;
    if (!file_manager.FileExistsAt(json_path())) {
        const char *json = "{ \"table\": {}, \"index\": {} }";
        Document doc;
        doc.Parse(json);
        ExportJSON(doc);
    }
}

Table CatalogManager::GetTable(const std::string &title) const {
    using namespace rapidjson;
    if (!is_valid())
        throw RootPathError();
    Document doc = ImportJSON();
    if (!doc["table"].HasMember(title.c_str()))
        throw TableNotExistsError();
    auto obj = doc["table"].GetObject()[title.c_str()].GetObject();
    Table result;
    result.title = title;
    for (auto &it : obj) {
        Column c;
        c.title = it.name.GetString();
        auto column_obj = it.value.GetObject();
        c.is_primary = column_obj["is_primary"].GetBool();
        c.is_indexed = column_obj["is_indexed"].GetBool();
        c.is_unique = column_obj["is_unique"].GetBool();
        c.type = static_cast<DataTypeIdentifier>(column_obj["type"].GetInt());
        c.size = column_obj["size"].GetInt();
        result.columns.push_back(c);
    }
    return result;
}

void CatalogManager::CreateTable(const Table &table) const {
    using namespace rapidjson;
    using namespace std;
    if (!is_valid())
        throw RootPathError();
    Document doc = ImportJSON();
    auto &alloc = doc.GetAllocator();
    if (doc["table"].HasMember(table.title.c_str()))
        throw TableAlreadyExistsError();
    
    Value new_table(kObjectType);
    for (const auto &c : table.columns) {
        Value new_column(kObjectType);
        new_column.AddMember("is_primary", c.is_primary, alloc);
        new_column.AddMember("is_indexed", c.is_indexed, alloc);
        new_column.AddMember("is_unique", c.is_unique, alloc);
        new_column.AddMember("type", static_cast<int>(c.type), alloc);
        new_column.AddMember("size", c.size, alloc);
        new_table.AddMember(StringRef(c.title.c_str()), new_column, alloc);
    }
    doc["table"].AddMember(StringRef(table.title.c_str()), new_table, alloc);
    
    ExportJSON(doc);
    
    static char data[2 * sizeof(uint64_t)];
    memset(data, 0, 2 * sizeof(uint64_t));
    fstream fs;
    fs.open(root_path_ + "/" + table.title + ".data", ios::out | ios::binary);
    fs.write(data, 2 * sizeof(uint64_t));
    fs.close();
    fs.open(root_path_ + "/" + table.title + ".index", ios::out | ios::binary);
    fs.write(data, 2 * sizeof(uint64_t));
}

void CatalogManager::RemoveTable(const std::string &title) const {
    using namespace rapidjson;
    using namespace std;
    if (!is_valid())
        throw RootPathError();
    auto doc = ImportJSON();
    if (!doc["table"].HasMember(title.c_str()))
        throw TableNotExistsError();
    
    doc["table"].RemoveMember(StringRef(title.c_str()));
    vector<string> to_delete_index;
    for (auto iter = doc["index"].MemberBegin(); iter != doc["index"].MemberEnd(); iter++) {
        if (strcmp(iter->value["table"].GetString(), title.c_str()) == 0) {
            to_delete_index.push_back(iter->name.GetString());
        }
    }
    for (const auto &str : to_delete_index) {
        doc["index"].RemoveMember(StringRef(str.c_str()));
        file_manager.RemoveFileAt(root_path_ + "/" + title + "." + str + ".index");
    }
    
    file_manager.RemoveFileAt(root_path_ + "/" + title + ".data");
    file_manager.RemoveFileAt(root_path_ + "/" + title + ".index");
    ExportJSON(doc);
}

bool CatalogManager::is_valid() const {
    return is_valid_;
}
