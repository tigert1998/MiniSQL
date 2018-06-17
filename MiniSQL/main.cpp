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
#include "b_plus_tree.hpp"
#include "node.hpp"

#include <iostream>

CatalogManager catalog_manager;

using namespace std;

const string root_path = "/Users/tigertang/Desktop/database";

int main() {

    Node<Int> node(5, 5);
    node.is_internal = true;
    for (int i = 0; i < 5; i++)
        node.children.push_back(10086 - i);
    for (int i = 0; i < 4; i++) {
        string s = "key";
        s.push_back(i + 'c');
        node.keys.emplace_back(i + 1);
    }
    
    Node<Int> another(5, node.raw_value());

    return 0;
}
