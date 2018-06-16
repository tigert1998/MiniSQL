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
