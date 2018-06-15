//
//  catalog_manager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>

#include "file_manager.hpp"
#include "table_item.hpp"

class CatalogManager {
public:
    static CatalogManager shared;
    CatalogManager();
    void set_root_path(std::string);
    bool opened();
    void CreateTable(const Table &);
    
private:
    std::string root_path_;
    bool opened_;
    
};

CatalogManager CatalogManager::shared = CatalogManager();

CatalogManager::CatalogManager(): opened_(false) { }

bool CatalogManager::opened() {
    return opened_;
}

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
    opened_ = true;
}

void CatalogManager::CreateTable(const Table &table) {
    using namespace std;
    FileManager &file_manager = FileManager::shared;
    file_manager.CreateFileAt(root_path_ + "/" + table.title + ".data");
    file_manager.CreateFileAt(root_path_ + "/" + table.title + ".data.header");
    file_manager.CreateFileAt(root_path_ + "/" + table.title + ".index");
    file_manager.CreateFileAt(root_path_ + "/" + table.title + ".index.header");
}
