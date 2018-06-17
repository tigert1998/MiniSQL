//
//  json_manager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/17.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>

#include "rapidjson/include/rapidjson/document.h"
#include "rapidjson/include/rapidjson/writer.h"
#include "rapidjson/include/rapidjson/stringbuffer.h"
#include "rapidjson/include/rapidjson/rapidjson.h"

class JSONManager {
public:
    static const JSONManager shared;
    void ExportJSON(const std::string &, const rapidjson::Document &) const;
    rapidjson::Document ImportJSON(const std::string &) const;
    
};

const JSONManager JSONManager::shared = JSONManager();

void JSONManager::ExportJSON(const std::string &path, const rapidjson::Document &doc) const {
    using namespace rapidjson;
    using namespace std;
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    fstream fs;
    fs.open(path, ios::out);
    fs << buffer.GetString() << endl;
}

rapidjson::Document JSONManager::ImportJSON(const std::string &path) const {
    using namespace rapidjson;
    using namespace std;
    ifstream in(path);
    istreambuf_iterator<char> beg(in), end;
    string result(beg, end);
    Document doc;
    doc.Parse(result.c_str());
    return doc;
}
