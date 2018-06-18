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


string RandomString() {
    string ans = "";
    for (int i = 0; i < 7; i++) {
        ans.push_back('a' + rand() % 26);
    }
    return ans;
}

int main() {
    index_manager.set_root_path(root_path);
    index_manager.CreatePrimaryIndex(table_name);
    BPlusTree<Char> t(8, root_path + "/" + table_name + ".index");
    set<string> s;
    auto start = clock();
    for (int i = 0; i < 100; i++) {
        string x = RandomString();
        if (s.count(x) != t.Count(Char(8, x))) {
            cout << "Wrong Answer!" << endl;
            return 0;
        }
        if (s.count(x)) {
            s.erase(x);
            t.Erase(Char(8, x));
        } else {
            s.insert(x);
            t.Insert(Char(8, x), i + 1);
        }
    }
    auto end = clock();
    cout << "Consumes " << (end - start) / CLOCKS_PER_SEC << "s" << endl;
    t.Print();
    Predicate<Char> predicate("column_name", PredicateIdentifier::LESS, Char(8, "aaaaa"));
    t.Query(predicate, [](uint64_t address) {
        cout << "address = " << address << endl;
    });
    return 0;
}
