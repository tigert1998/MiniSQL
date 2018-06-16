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
#include "database_exception.hpp"

struct Column {
    std::string title;
    bool is_primary, is_indexed, is_unique;
    DataTypeIdentifier type;
    uint64_t size;
    
};

struct Table {
    std::string title;
    std::vector<Column> columns;
    
};

class Record {
public:
    Record() = delete;
    Record(const Table &);
    void Reset();
    template <typename T> void Feed(const T &);
    
private:
    const Table &schema;
    std::vector<char> raw_value_;
    int column_id;
    
};

Record::Record(const Table &schema): schema(schema), column_id(0) {}

void Record::Reset() {
    column_id = 0;
    raw_value_.clear();
}

template <typename T>
void Record::Feed(const T &value) {
    const auto &c = schema.columns[column_id];
    if (value->GetType() != c.type || value->size() != c.size) {
        throw TypeError();
    }
    for (int i = 0; i < value->size(); i++) {
        char tmp = value->raw_value()[i];
        raw_value_.push_back(tmp);
    }
    column_id++;
}
