//
//  api.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/25.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>
#include <vector>

#include "record_manager.hpp"
#include "catalog_manager.hpp"
#include "index_manager.hpp"

struct Predicate {
    std::string column_name;
    PredicateIdentifier type;
    std::string value;
    // [Explaination]
    // int should be like 123
    // char(n) should be like "tigertang" or 'tigertang'
    // float should be like 123 or 123.0
};

class API {
private:
    static bool Satisfy(const Record &record, const Predicate &predicate);
    static bool Satisfy(const Record &record, const std::vector<Predicate> &predicates);
    
    static void QueryWithIndexPredicate(const Table &table, const Predicate &index_predicate, const std::vector<Predicate> other_predicates, std::function<void(uint64_t, Record)> yield);
    static void RemoveWithOffsetsAndRecords(const std::vector<uint64_t> &offsets, const std::vector<Record> &records);
    
public:
    static std::string root_path;
    
    static void CreateTable(
        const std::string &table_name,
        const std::vector<std::string> &column_names,
        const std::vector<DataTypeIdentifier> &types,
        const std::vector<int> &sizes,
        const std::vector<bool> &is_uniques,
        const std::string &primary_key
    );
    
    static void DropTable(const std::string &table_name);
    
    static void CreateIndex(const std::string &index_name, const std::string &table_name, const std::string &column_name);
    
    static void DropIndex(const std::string &index_name);
    
    static void Insert(const std::string &table_name, const std::vector<std::string> &values);
    
    static void Select(const std::string &table_name, const std::vector<Predicate> &predicates, std::function<void(Record)> yield);
    
    static void Delete(const std::string &table_name, const std::vector<Predicate> &predicates);
};

std::string API::root_path;

void API::RemoveWithOffsetsAndRecords(const std::vector<uint64_t> &offsets, const std::vector<Record> &records) {
    if (offsets.size() == 0) return;
    IndexManager &index_manager = IndexManager::shared;
    index_manager.set_root_path(root_path);
    RecordManager record_manager(records.front().schema, root_path + "/" + records.front().schema.title + ".data");
    for (int i = 0; i < offsets.size(); i++) {
        auto record = records[i];
        auto offset = offsets[i];
        index_manager.RemoveRecordFromIndices(record);
        record_manager.Erase(offset, record);
    }
}

void API::Delete(const std::string &table_name, const std::vector<Predicate> &predicates) {
    using namespace std;
    CatalogManager &catalog_manager = CatalogManager::shared;
    IndexManager &index_manager = IndexManager::shared;
    catalog_manager.set_root_path(root_path);
    index_manager.set_root_path(root_path);
    auto table = catalog_manager.GetTable(table_name);
    RecordManager record_manager(table, root_path + "/" + table_name + ".data");
    
    static vector<uint64_t> offsets;
    static vector<Record> records;
    
    offsets.clear();
    records.clear();
    
    for (int i = 0; i < predicates.size(); i++) {
        auto predicate = predicates[i];
        const Column &column = table.GetColumn(predicate.column_name);
        if (predicate.type == PredicateIdentifier::EQUAL && column.is_indexed) {
            // select '=' firstly
            vector<Predicate> other_predicates = predicates;
            other_predicates.erase(other_predicates.begin() + i);
            QueryWithIndexPredicate(table, predicate, other_predicates, [&](uint64_t offset, Record record) {
                offsets.push_back(offset);
                records.push_back(record);
            });
            RemoveWithOffsetsAndRecords(offsets, records);
            return;
        }
    }
    
    for (int i = 0; i < predicates.size(); i++) {
        auto predicate = predicates[i];
        const Column &column = table.GetColumn(predicate.column_name);
        if (!column.is_indexed) continue;
        if (predicate.type == PredicateIdentifier::UNEQUAL) continue;
        
        vector<Predicate> other_predicates = predicates;
        other_predicates.erase(other_predicates.begin() + i);
        QueryWithIndexPredicate(table, predicate, other_predicates, [&](uint64_t offset, Record record) {
            offsets.push_back(offset);
            records.push_back(record);
        });
        RemoveWithOffsetsAndRecords(offsets, records);
        return;
    }
    
    record_manager.TraverseRecordsWithOffsets([&](uint64_t offset, Record record) {
        if (Satisfy(record, predicates)) {
            offsets.push_back(offset);
            records.push_back(record);
        }
    });
    RemoveWithOffsetsAndRecords(offsets, records);
}

void API::QueryWithIndexPredicate(const Table &table, const Predicate &index_predicate, const std::vector<Predicate> other_predicates, std::function<void(uint64_t, Record)> yield) {
    IndexManager &index_manager = IndexManager::shared;
    index_manager.set_root_path(root_path);
    RecordManager record_manager(table, root_path + "/" + table.title + ".data");
    
    const Column &column = table.GetColumn(index_predicate.column_name);
    int column_id = table.GetColumnID(index_predicate.column_name);
    int key_size = (int) column.size;
    
    if (column.type == DataTypeIdentifier::Int) {
        int value = atoi(index_predicate.value.c_str());
        index_manager.Query<Int>(table, Predicate_<Int>(column.title, index_predicate.type, Int(value)), [&](uint64_t offset, Int value) {
            auto record = record_manager.GetRecord<Int>(offset, value, [&](Int key, Record record) -> bool {
                return record.Get<Int>(column_id).value() == key.value();
            });
            if (Satisfy(record, other_predicates))
                yield(offset, record);
        });
    } else if (column.type == DataTypeIdentifier::Char) {
        std::string value = index_predicate.value.substr(1, index_predicate.value.length() - 2);
        index_manager.Query<Char>(table, Predicate_<Char>(column.title, index_predicate.type, Char(key_size, value)), [&](uint64_t offset, Char value) {
            auto record = record_manager.GetRecord<Char>(offset, value, [&](Char key, Record record) -> bool {
                return record.Get<Char>(column_id).value() == key.value();
            });
            if (Satisfy(record, other_predicates))
                yield(offset, record);
        });
    } else if (column.type == DataTypeIdentifier::Float) {
        float value = atof(index_predicate.value.c_str());
        index_manager.Query<Float>(table, Predicate_<Float>(column.title, index_predicate.type, Float(value)), [&](uint64_t offset, Float value) {
            auto record = record_manager.GetRecord<Float>(offset, Float(value), [&](Float key, Record record) -> bool {
                return record.Get<Float>(column_id).value() == key.value();
            });
            if (Satisfy(record, other_predicates))
                yield(offset, record);
        });
    }
}

bool API::Satisfy(const Record &record, const std::vector<Predicate> &predicates) {
    for (const Predicate &predicate: predicates) {
        if (!Satisfy(record, predicate)) return false;
    }
    return true;
}

bool API::Satisfy(const Record &record, const Predicate &predicate) {
    using namespace std;
    int column_id = record.schema.GetColumnID(predicate.column_name);
    auto type = record.schema.columns[column_id].type;
    bool ans = false;
    if (type == DataTypeIdentifier::Int) {
        int value = record.Get<Int>(column_id).value();
        int predicate_value = atoi(predicate.value.c_str());
        switch (predicate.type) {
            case PredicateIdentifier::EQUAL:
                ans = value == predicate_value;
                break;
            case PredicateIdentifier::LESS:
                ans = value < predicate_value;
                break;
            case PredicateIdentifier::LESS_OR_EQUAL:
                ans = value <= predicate_value;
                break;
            case PredicateIdentifier::GREATER:
                ans = value > predicate_value;
                break;
            case PredicateIdentifier::GREATER_OR_EQUAL:
                ans = value >= predicate_value;
                break;
            case PredicateIdentifier::UNEQUAL:
                ans = value != predicate_value;
                break;
        }
    } else if (type == DataTypeIdentifier::Char) {
        string value = record.Get<Char>(column_id).value();
        string predicate_value = predicate.value.substr(1, predicate.value.length() - 2);
        switch (predicate.type) {
            case PredicateIdentifier::EQUAL:
                ans = value == predicate_value;
                break;
            case PredicateIdentifier::LESS:
                ans = value < predicate_value;
                break;
            case PredicateIdentifier::LESS_OR_EQUAL:
                ans = value <= predicate_value;
                break;
            case PredicateIdentifier::GREATER:
                ans = value > predicate_value;
                break;
            case PredicateIdentifier::GREATER_OR_EQUAL:
                ans = value >= predicate_value;
                break;
            case PredicateIdentifier::UNEQUAL:
                ans = value != predicate_value;
                break;
        }
    } else if (type == DataTypeIdentifier::Float) {
        float value = record.Get<Float>(column_id).value();
        float predicate_value = atof(predicate.value.c_str());
        switch (predicate.type) {
            case PredicateIdentifier::EQUAL:
                ans = value == predicate_value;
                break;
            case PredicateIdentifier::LESS:
                ans = value < predicate_value;
                break;
            case PredicateIdentifier::LESS_OR_EQUAL:
                ans = value <= predicate_value;
                break;
            case PredicateIdentifier::GREATER:
                ans = value > predicate_value;
                break;
            case PredicateIdentifier::GREATER_OR_EQUAL:
                ans = value >= predicate_value;
                break;
            case PredicateIdentifier::UNEQUAL:
                ans = value != predicate_value;
                break;
        }
    }
    return ans;
}

void API::Select(const std::string &table_name, const std::vector<Predicate> &predicates, std::function<void(Record)> yield) {
    using namespace std;
    CatalogManager &catalog_manager = CatalogManager::shared;
    IndexManager &index_manager = IndexManager::shared;
    catalog_manager.set_root_path(root_path);
    index_manager.set_root_path(root_path);
    auto table = catalog_manager.GetTable(table_name);
    
    for (int i = 0; i < predicates.size(); i++) {
        auto predicate = predicates[i];
        const Column &column = table.GetColumn(predicate.column_name);
        if (predicate.type == PredicateIdentifier::EQUAL && column.is_indexed) {
            // select '=' firstly
            vector<Predicate> other_predicates = predicates;
            other_predicates.erase(other_predicates.begin() + i);
            QueryWithIndexPredicate(table, predicate, other_predicates, [&](uint64_t, Record record) { yield(record); });
            return;
        }
    }
    
    for (int i = 0; i < predicates.size(); i++) {
        auto predicate = predicates[i];
        const Column &column = table.GetColumn(predicate.column_name);
        if (!column.is_indexed) continue;
        if (predicate.type == PredicateIdentifier::UNEQUAL) continue;
        
        vector<Predicate> other_predicates = predicates;
        other_predicates.erase(other_predicates.begin() + i);
        QueryWithIndexPredicate(table, predicate, other_predicates, [&](uint64_t, Record record) { yield(record); });
        return;
    }
    
    RecordManager record_manager(table, root_path + "/" + table_name + ".data");
    record_manager.TraverseRecordsWithOffsets([&](uint64_t offset, Record record) {
        if (Satisfy(record, predicates)) yield(record);
    });
}

void API::CreateTable(const std::string &table_name, const std::vector<std::string> &column_names, const std::vector<DataTypeIdentifier> &types, const std::vector<int> &sizes, const std::vector<bool> &is_uniques, const std::string &primary_key) {
    Table table;
    table.title = table_name;
    bool primary_visited = false;
    for (int i = 0; i < column_names.size(); i++) {
        Column column;
        column.title = column_names[i];
        column.size = sizes[i];
        column.type = types[i];
        column.is_unique = is_uniques[i];
        column.is_indexed = column.is_primary = false;
        if (column.title == primary_key) {
            column.is_primary = column.is_unique = column.is_indexed = true;
            primary_visited = true;
        }
        table.columns.push_back(column);
    }
    if (!primary_visited) throw ColumnNotExistsError();
    CatalogManager::shared.set_root_path(root_path);
    CatalogManager::shared.CreateTable(table);
}

void API::DropTable(const std::string &table_name) {
    CatalogManager::shared.set_root_path(root_path);
    CatalogManager::shared.RemoveTable(table_name);
}


void API::CreateIndex(const std::string &index_name, const std::string &table_name, const std::string &column_name) {
    IndexManager::shared.set_root_path(root_path);
    IndexManager::shared.CreateIndex(index_name, table_name, column_name);
}

void API::DropIndex(const std::string &index_name) {
    IndexManager::shared.set_root_path(root_path);
    IndexManager::shared.RemoveIndex(index_name);
}

void API::Insert(const std::string &table_name, const std::vector<std::string> &values) {
    // int should be like 123
    // char(n) should be like "tigertang" or 'tigertang'
    // float should be like 123 or 123.0
    using namespace std;
    CatalogManager &catalog_manager = CatalogManager::shared;
    catalog_manager.set_root_path(root_path);
    IndexManager &index_manager = IndexManager::shared;
    index_manager.set_root_path(root_path);
    auto table = catalog_manager.GetTable(table_name);
    auto record = Record(table);
    record.Reset();
    for (int i = 0; i < table.columns.size(); i++) {
        string value = values[i];
        switch (table.columns[i].type) {
            case DataTypeIdentifier::Int:
                record.Feed<Int>(Int(atoi(value.c_str())));
                break;
            case DataTypeIdentifier::Char:
                record.Feed<Char>(Char((int) table.columns[i].size, value.substr(1, value.length() - 2)));
                break;
            case DataTypeIdentifier::Float:
                record.Feed<Float>(Float(atof(value.c_str())));
                break;
        }
    }
    RecordManager record_manager(table, root_path + "/" + table_name + ".data");
    auto offset = record_manager.Insert(record);
    index_manager.InsertRecordIntoIndices(record, offset);
}

