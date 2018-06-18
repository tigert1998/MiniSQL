//
//  main.cpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include <iostream>
#include <set>

#include "record_manager.hpp"

using namespace std;

//CatalogManager &catalog_manager = CatalogManager::shared;
//IndexManager &index_manager = IndexManager::shared;
//
//
//const string root_path = "/Users/tigertang/Desktop/database";
//const string table_name = "fuck_table";
//
//
//string RandomString() {
//    string ans = "";
//    for (int i = 0; i < 7; i++) {
//        ans.push_back('a' + rand() % 26);
//    }
//    return ans;
//}
//
//int main() {
//    index_manager.set_root_path(root_path);
//    index_manager.CreatePrimaryIndex(table_name);
//    BPlusTree<Char> t(8, root_path + "/" + table_name + ".index");
//    set<string> s;
//    auto start = clock();
//    for (int i = 0; i < 100; i++) {
//        string x = RandomString();
//        if (s.count(x) != t.Count(Char(8, x))) {
//            cout << "Wrong Answer!" << endl;
//            return 0;
//        }
//        if (s.count(x)) {
//            s.erase(x);
//            t.Erase(Char(8, x));
//        } else {
//            s.insert(x);
//            t.Insert(Char(8, x), i + 1);
//        }
//    }
//    auto end = clock();
//    cout << "Consumes " << (end - start) / CLOCKS_PER_SEC << "s" << endl;
//    t.Print();
//    Predicate<Char> predicate("column_name", PredicateIdentifier::LESS, Char(8, "aaaaa"));
//    t.Query(predicate, [](uint64_t address) {
//        cout << "address = " << address << endl;
//    });
//    return 0;
//}



int main() {
    Table table;
    Column c;
    c.type = DataTypeIdentifier::Int;
    c.size = 4;
    table.columns.push_back(c);
    c.type = DataTypeIdentifier::Float;
    c.size = 4;
    table.columns.push_back(c);
    c.type = DataTypeIdentifier::Char;
    c.size = 10;
    table.columns.push_back(c);
    c.type = DataTypeIdentifier::Int;
    c.size = 4;
    table.columns.push_back(c);
    Record x(table);
    x.Reset();
    x.Feed(Int(3));
    x.Feed(Float(2.3333));
    x.Feed(Char(10, "hello"));
    x.Feed(Int(10086));
    Record y(table, x.raw_value());
    cout << y.Get<Int>(0).value() << endl;
    cout << y.Get<Float>(1).value() << endl;
    cout << y.Get<Char>(2).value() << endl;
    cout << y.Get<Int>(3).value() << endl;
    return 0;
}
