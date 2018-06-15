//
//  debug_util.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/15.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "constant.hpp"
#include "table_item.hpp"
#include "file_manager.hpp"

void PrintBlock(const char *s) {
    using namespace std;
    for (int i = 0; i < kBlockSize; i += 16) {
        cout << setw(5) << i << ": ";
        for (int j = 0; j < 16; j++) {
            char tmp = s[i + j];
            for (int k = 0; k < 8; k++) if (tmp & (1 << k)) cout << 1;
            else cout << 0;
            cout << " ";
        }
        cout << endl;
    }
}

std::string ShowDataTypeIdentifier(DataTypeIdentifier t) {
    switch (t) {
        case DataTypeIdentifier::Int:
            return "Int";
        case DataTypeIdentifier::Char:
            return "Char";
        case DataTypeIdentifier::Float:
            return "Float";
        default:
            return "WrongType";
    }
}

void ShowTableSchemaInFile(const std::string &root_path) {
    using namespace std;
    auto catalog_path = root_path + "/catalog";
    auto header_path = root_path + "/catalog.header";
    auto file_size = FileManager::shared.FileSizeAt(catalog_path);
    cout << "File size at " << catalog_path << " is " << file_size << " bytes" << endl;
    static char tmp[kBlockSize];
    
    fstream fs;
    fs.open(header_path, ios::in | ios::binary);
    fs.read(tmp, 8);
    auto valid_head = Int(tmp).value(), invalid_head = Int(tmp + 4).value();
    auto i = valid_head;
    
    fs.close();
    fs.open(catalog_path, ios::in | ios::binary);
    while (i != -1) {
        fs.seekg(i);
        cout << "[valid block at " << fs.tellg() << "]" << endl;
        fs.read(tmp, kBlockSize);
        i = Int(tmp).value();
        cout << "next valid block offset: " << i << endl;
        auto total = Int(tmp + 4).value();
        cout << "The number of tables in this block is: " << total << endl;
        char *s = tmp + 8;
        for (int j = 0; j < total; j++) {
            auto table = Table(s);
            cout << "   [table " << table.title << "]" << endl;
            cout << "   table size = " << table.size() << endl;
            for (const auto &c : table.columns) {
                cout << "       [column " << c.title << "]" << endl;
                cout << "       type = " << ShowDataTypeIdentifier(c.type) << endl;
                cout << "       is_primary = " << boolalpha << c.is_primary << endl;
                cout << "       is_indexed = " << boolalpha << c.is_indexed << endl;
                cout << "       is_unique = " << boolalpha << c.is_unique << endl;
            }
            s += table.size();
        }
    }
    
    i = invalid_head;
    while (i != file_size) {
        fs.seekg(i);
        cout << "[invalid block at " << i << "]" << endl;
        fs.read(tmp, kBlockSize);
        i = Int(tmp).value();
        cout << "next invalid block offset: " << i << endl;
    }
}
