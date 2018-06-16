//
//  file_manager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <fstream>
#include <string>

class FileManager {
public:
    const static FileManager shared;
    bool FileExistsAt(const std::string &) const;
    void CreateFileAt(const std::string &) const;
    uint32_t FileSizeAt(const std::string &) const;
    void RemoveFileAt(const std::string &) const;
    
};

const FileManager FileManager::shared = FileManager();

bool FileManager::FileExistsAt(const std::string &path) const {
    std::ifstream is(path);
    return is.is_open();
}

void FileManager::CreateFileAt(const std::string &path) const {
    std::ofstream os(path);
}

uint32_t FileManager::FileSizeAt(const std::string &path) const {
    using namespace std;
    fstream fs;
    fs.open(path, ios::in | ios::binary);
    fs.seekg(0, ios::end);
    return (uint32_t) fs.tellg();
}

void FileManager::RemoveFileAt(const std::string &path) const {
    remove(path.c_str());
}
