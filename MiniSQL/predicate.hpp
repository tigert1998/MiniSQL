//
//  predicate.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/18.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>

enum class PredicateIdentifier {
    GREATER, GREATER_OR_EQUAL,
    LESS, LESS_OR_EQUAL,
    EQUAL, UNEQUAL
};

template <typename KeyType>
class Predicate_ {
public:
    std::string column_name;
    PredicateIdentifier type;
    KeyType key;

    Predicate_(const std::string column_name, PredicateIdentifier type, KeyType key): column_name(column_name), type(type), key(key) { }
};
