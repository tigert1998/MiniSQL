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


// T should be Int Char or Float
class Record {
public:
    Record() = delete;
    Record(const Table &);
    Record(const Table &, const char *);
    const char *raw_value();
    void Reset();
    template <typename T> void Feed(const T &);
    template <typename T> T Get(int);
    uint64_t size() const;
    const Table &schema;
    
private:
    std::vector<char> raw_value_;
    int column_id;
    
};

Record::Record(const Table &schema): schema(schema), column_id(0) {}

Record::Record(const Table &schema, const char *s): schema(schema), column_id((int) schema.columns.size()) {
    raw_value_.resize(size());
    memcpy(raw_value_.data(), s, size());
}

uint64_t Record::size() const {
    uint64_t ans = 0;
    for (auto c : schema.columns) ans += c.size;
    return ans;
}

template <>
Char Record::Get(int column_id) {
    uint64_t offset = 0;
    for (int i = 0; i < column_id; i++) offset += schema.columns[i].size;
    return Char((int) schema.columns[column_id].size, raw_value_.data() + offset);
}

template <typename T>
T Record::Get(int column_id) {
    uint64_t offset = 0;
    for (int i = 0; i < column_id; i++) offset += schema.columns[i].size;
    return T(raw_value_.data() + offset);
}

void Record::Reset() {
    column_id = 0;
    raw_value_.clear();
}

template <typename T>
void Record::Feed(const T &value) {
    const auto &c = schema.columns[column_id];
    if (value.GetType() != c.type || value.size() != c.size) {
        throw TypeError();
    }
    for (int i = 0; i < value.size(); i++) {
        char tmp = value.raw_value()[i];
        raw_value_.push_back(tmp);
    }
    column_id++;
}

const char *Record::raw_value() {
    return raw_value_.data();
}
