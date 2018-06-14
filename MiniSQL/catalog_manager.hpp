//
//  CatalogManager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>

#include "file_manager.hpp"
#include "data_type.hpp"

class CatalogManager {
public:
    static CatalogManager shared;
    void set_root_path(std::string);
    
private:
    std::string root_path_;
    
};

CatalogManager CatalogManager::shared = CatalogManager();

void CatalogManager::set_root_path(std::string path) {
    using namespace std;
    FileManager &file_manager = FileManager::shared;
    if (!file_manager.FileExistsAt(path + "/catalog") || !file_manager.FileExistsAt(path + "/catalog.header")) {
        file_manager.CreateFileAt(path + "/catalog");
        file_manager.CreateFileAt(path + "/catalog.header");
        fstream fs;
        fs.open(path + "/catalog.header", ios::out | ios::binary);
        fs.write(Int(0).raw_value(), 4);
    }
    root_path_ = path;
}
