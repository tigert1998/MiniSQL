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

string RandomName() {
    switch (rand() % 4) {
        case 0:
            return "\"Tom\"";
        case 1:
            return "\"David\"";
        case 2:
            return "\"Mike\"";
        default:
            return "\"Jerry\"";
    }
}

string RandomUniqueString() {
    auto random_string = []() -> string {
        string res;
        for (int i = 0; i < 36; i++) res += 'a' + rand() % 26;
        return res;
    };
    static set<string> s;
    string str;
    do {
        str = random_string();
    } while (s.count(str));
    s.insert(str);
    return "\"" + str + "\"";
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
    
    cout << "Output Inserted Content" << endl;
    
    for (int i = 1300; i <= 2299; i++) {
        string height = to_string(i) + ".0";
        string pid = to_string(i - 1300);
        string name = RandomName();
        string identity = RandomUniqueString();
        string age = pid;
        
        cout << "-----------" << endl;
        cout << "height = " << height << endl;
        cout << "pid = " << pid << endl;
        cout << "name = " << name << endl;
        cout << "identity = " << identity << endl;
        cout << "age = " << age << endl;
        
        API::Insert("person", {height, pid, name, identity, age});
    }
    
    API::CreateIndex("idx_height", "person", "height");
    API::CreateIndex("idx_identity", "person", "identity");
    API::CreateIndex("idx_age", "person", "age");

    cout << "Output Disk Content" << endl;
    
    API::Select("person", {}, [](const Record &record) {
        cout << "-----------" << endl;
        cout << "height = " << record.Get<Float>(0).value() << endl;
        cout << "pid = " << record.Get<Int>(1).value() << endl;
        cout << "name = " << record.Get<Char>(2).value() << endl;
        cout << "identity = " << record.Get<Char>(3).value() << endl;
        cout << "age = " << record.Get<Int>(4).value() << endl;
    });
    
    return 0;
}
