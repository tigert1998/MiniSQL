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
#include "index_manager.hpp"

#include <iostream>

CatalogManager &catalog_manager = CatalogManager::shared;
IndexManager &index_manager = IndexManager::shared;

using namespace std;

const string root_path = "/Users/tigertang/Desktop/database";
const string table_name = "fuck_table";


int main() {
    index_manager.set_root_path(root_path);
    index_manager.RemoveIndex("fuck_index");
    // index_manager.CreateIndex("fuck_index", "fuck_table", "fuck_age");
    // BPlusTree<Int> t(sizeof(int), root_path + "/" + table_name + ".index");
    // t.Insert(Int(3), 0);
    return 0;
}
