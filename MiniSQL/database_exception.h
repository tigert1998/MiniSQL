//
//  database_exception.h
//  MiniSQL
//
//  Created by tigertang on 2018/6/15.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <exception>

class RootPathError: public std::exception {
public:
    const char *what() const noexcept {
        return "Database root path is not appropriately seted.";
    }
};

class TableNotExistError: public std::exception {
public:
    const char *what() const noexcept {
        return "Table not exists.";
    }
};