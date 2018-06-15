//
//  table_item.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>
#include <vector>

#include "data_type.hpp"

// represent table data in binary format:
// | 32(table title) | 4(number of columns) | 32(column title) | 1(primary, indexed, unique, ignore...) | 4(TypeIdentifier) | ... |

class Column {
public:
    std::string title;
    bool is_primary, is_indexed, is_unique;
    DataTypeIdentifier type;
    
    static unsigned long size() {
        return 37;
    }
    const std::vector<char> &raw_value() {
        raw_value_.resize(size());
        char *s = raw_value_.data();
        memcpy(s, title.c_str(), title.size() + 1);
        s[32] = 0;
        if (is_primary) s[32] |= 1;
        if (is_indexed) s[32] |= 1 << 1;
        if (is_unique) s[32] |= 1 << 2;
        memcpy(s + 33, Int(static_cast<int>(type)).raw_value(), 4);
        return raw_value_;
    }
    
private:
    std::vector<char> raw_value_;
};

class Table {
public:
    std::string title;
    std::vector<Column> columns;
    
    unsigned long size() {
        return 36 + Column::size() * columns.size();
    }
    
    Table() = default;
    Table(const char *s) {
        title = Char(s).value();
        columns.resize(Int(s + 32).value());
        s += 36;
        for (Column &c : columns) {
            c.title = Char(s).value();
            c.is_primary = s[32] & 1;
            c.is_indexed = (s[32] >> 1) & 1;
            c.is_unique = (s[32] >> 2) & 1;
            c.type = static_cast<DataTypeIdentifier>(Int(s + 33).value());
            s += Column::size();
        }
    }
    
    const std::vector<char> &raw_value() {
        raw_value_.resize(size());
        char *s = raw_value_.data();
        memcpy(s, title.c_str(), title.size() + 1);
        memcpy(s + 32, Int((int) columns.size()).raw_value(), 4);
        s += 36;
        for (Column &c : columns) {
            memcpy(s, c.raw_value().data(), c.size());
            s += Column::size();
        }
        
        return raw_value_;
    }
private:
    std::vector<char> raw_value_;
};
