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

struct Column {
    std::string title;
    bool is_primary, is_indexed, is_unique;
    DataTypeIdentifier type;
    
};

struct Table {
    std::string title;
    std::vector<Column> columns;
    
};

class Record {
public:
    Record() = delete;
    Record(const Table &);
    
private:
    const Table &schema;
    std::vector<char> raw_value();
    
};

Record::Record(const Table &schema): schema(schema) { }
