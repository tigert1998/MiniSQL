//
//  main.cpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include <iostream>
#include <set>

#include "record_manager.hpp"
#include "catalog_manager.hpp"
#include "index_manager.hpp"

using namespace std;

const std::string root_path = "/Users/tigertang/Desktop/database";

CatalogManager &catalog_manager = CatalogManager::shared;

int main() {
    Table table;
    table.title = "sample_table";
    
    Column c;
    c.title = "id";
    c.is_primary = c.is_indexed = c.is_unique = true;
    c.type = DataTypeIdentifier::Int;
    c.size = 4;
    table.columns.push_back(c);
    
    c.title = "weight";
    c.is_primary = c.is_indexed = c.is_unique = false;
    c.type = DataTypeIdentifier::Float;
    c.size = 4;
    table.columns.push_back(c);
    
    c.title = "name";
    c.is_primary = c.is_indexed = c.is_unique = false;
    c.type = DataTypeIdentifier::Char;
    c.size = 10;
    table.columns.push_back(c);
    
    c.title = "score";
    c.is_primary = c.is_indexed = c.is_unique = false;
    c.type = DataTypeIdentifier::Int;
    c.size = 4;
    table.columns.push_back(c);
    
    catalog_manager.set_root_path(root_path);
    catalog_manager.CreateTable(table);
    RecordManager record_manager(table, root_path + "/" + table.title + ".data");
    IndexManager &index_manager = IndexManager::shared;
    index_manager.set_root_path(root_path);
    
    Record x(table);
    
    for (int i = 1; i <= 1000; i++) {
        x.Reset();
        x.Feed(Int(i));
        x.Feed(Float(2.3333));
        x.Feed(Char(10, "hello"));
        x.Feed(Int(10086));
        auto offset = record_manager.Insert(x);
        index_manager.InsertRecordIntoIndices(x, offset);
    }
    
    for (int i = 1; i <= 1000; i += 10) {
        Predicate<Int> predicate("id", PredicateIdentifier::EQUAL, Int(i));
        uint64_t offset;
        index_manager.Query(table, predicate, [&](uint64_t ans) {
            offset = ans;
        });
        auto match = [&](Int key, Record record) -> bool {
            return record.Get<Int>(0).value() == key.value();
        };
        Record record = record_manager.GetRecord<Int>(offset, Int(i), match);
        index_manager.RemoveRecordFromIndices(record);
        record_manager.Erase<Int>(offset, Int(i), match);
    }
    
    record_manager.PrintFile([](Record record) {
        cout << "id = " << record.Get<Int>(0).value() << endl;
        cout << "weight = " << record.Get<Float>(1).value() << endl;
        cout << "name = " << record.Get<Char>(2).value() << endl;
        cout << "score = " << record.Get<Int>(3).value() << endl;
    });
    
    return 0;
}
