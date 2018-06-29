//
//  main.cpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include <iostream>
#include <set>

#include "api.hpp"
using namespace std;

const std::string root_path = "/Users/tigertang/Desktop/database";

void PutRecord(const Record &record) {
    cout << "[Record]" << endl;
    cout << "height = " << record.Get<Float>(0).value() << endl;
    cout << "pid = " << record.Get<Int>(1).value() << endl;
    cout << "name = " << record.Get<Char>(2).value() << endl;
    cout << "identity = " << record.Get<Char>(3).value() << endl;
    cout << "age = " << record.Get<Int>(4).value() << endl;
}

int main() {
    API::root_path = root_path;
    
    vector<string> column_names = {"height", "pid", "name", "identity", "age"};
    vector<DataTypeIdentifier> types = {
        DataTypeIdentifier::Float,
        DataTypeIdentifier::Int,
        DataTypeIdentifier::Char,
        DataTypeIdentifier::Char,
        DataTypeIdentifier::Int
    };
    vector<int> sizes = {4, 4, 33, 129, 4};
    vector<bool> is_uniques = {true, false, false, true, true};

    API::CreateTable("person", column_names, types, sizes, is_uniques, "pid");

    for (int i = 1; i <= 20; i++) {
        string height = to_string(i) + ".0";
        string pid = to_string(i - 1300);
        string name = "\"Person" + to_string(i) + "\"";
        string identity = "\"0000" + to_string(i) + "\"";
        string age = to_string(i);
        API::Insert("person", {height, pid, name, identity, age});
    }

    API::CreateIndex("idx_height", "person", "height");
    API::CreateIndex("idx_identity", "person", "identity");
    API::CreateIndex("idx_age", "person", "age");

    API::Delete("person", {{"age", PredicateIdentifier::LESS_OR_EQUAL, "14"}});
    
    API::Select("person", {{"age", PredicateIdentifier::GREATER, "9"}}, PutRecord);
    API::Select("person", {{"name", PredicateIdentifier::EQUAL, "\"Person15\""}}, PutRecord);
    API::Select("person", {{"height", PredicateIdentifier::LESS_OR_EQUAL, "10"}}, PutRecord);

    API::DropIndex("idx_height");
    API::DropIndex("idx_identity");
    API::DropIndex("idx_age");

    API::DropTable("person");
    return 0;
}
