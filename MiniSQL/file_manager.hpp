//
//  FileManager.hpp
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
    static FileManager shared;
    bool FileExistsAt(const char *);
    void CreateFileAt(const char *);
    bool FileExistsAt(const std::string &);
    void CreateFileAt(const std::string &);
};

FileManager FileManager::shared = FileManager();

bool FileManager::FileExistsAt(const char *path) {
    std::ifstream is(path);
    return is.is_open();
}

void FileManager::CreateFileAt(const char *path) {
    std::ofstream os(path);
}

bool FileManager::FileExistsAt(const std::string &path) {
    return FileExistsAt(path.c_str());
}

void FileManager::CreateFileAt(const std::string &path) {
    CreateFileAt(path.c_str());
}
