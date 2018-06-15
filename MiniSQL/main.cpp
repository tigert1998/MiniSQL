//
//  main.cpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include "buffer_manager.hpp"
#include "catalog_manager.hpp"
#include "table_item.hpp"
#include "debug_util.hpp"

#include <iostream>

CatalogManager catalog_manager;

using namespace std;

const string root_path = "/Users/tigertang/Desktop/database";

int main() {

//    catalog_manager.set_root_path(root_path);
//    catalog_manager.RemoveTable("sample_table");
    ShowTableSchemaInFile(root_path);
    
//    ShowTableSchemaInFile(root_path);
//    catalog_manager.set_root_path(root_path);
//
//
//    Table table;
//    table.title = "another_table";
//    auto column = Column();
//    column.is_primary = column.is_indexed = column.is_unique = true;
//    column.title = "student_id";
//    column.type = DataTypeIdentifier::Int;
//    table.columns.push_back(column);
//
//    column = Column();
//    column.is_primary = column.is_indexed = false;
//    column.is_unique = true;
//    column.type = DataTypeIdentifier::Char;
//    column.title = "student_age";
//    table.columns.push_back(column);
//
//
//    catalog_manager.CreateTable(table);
    return 0;
}
