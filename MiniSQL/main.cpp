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
#include <set>

CatalogManager &catalog_manager = CatalogManager::shared;
IndexManager &index_manager = IndexManager::shared;

using namespace std;

const string root_path = "/Users/tigertang/Desktop/database";
const string table_name = "fuck_table";


int main() {
    index_manager.set_root_path(root_path);
    index_manager.CreatePrimaryIndex(table_name);
    BPlusTree<Int> t(4, root_path + "/" + table_name + ".index");
    set<int> s;
    for (int i = 0; i < 1000; i++) {
        int x;
        do {
            x = rand() % 10086;
        } while (s.count(x));
        t.Insert(Int(x), i + 1);
        // cout << "======================" << endl;
        // t.PrintTree();
    }
    t.Print();
    return 0;
}
