//
//  main.cpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include "buffer_manager.hpp"
#include "catalog_manager.hpp"

#include <iostream>

using namespace std;

BufferManager<20, 100> buffer_manager;

const char *file_name = "/Users/tigertang/CodeSandBox/test";



int main() {
    buffer_manager.Open(file_name);
    auto s = buffer_manager.Read(0);
    for (int i = 0; i < 100; i++) cout << s[i] << endl;
    return 0;
}
