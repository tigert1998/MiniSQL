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
    static FileManager shared;
    bool FileExistsAt(const char *);
    bool FileExistsAt(const std::string &);
    
    void CreateFileAt(const char *);
    void CreateFileAt(const std::string &);
    
    unsigned long FileSizeAt(const char *);
    unsigned long FileSizeAt(const std::string &);
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

unsigned long FileManager::FileSizeAt(const char *path) {
    using namespace std;
    fstream fs;
    fs.open(path, ios::in | ios::binary);
    fs.seekg(0, ios::end);
    return fs.tellg();
}

unsigned long FileManager::FileSizeAt(const std::string &path) {
    return FileSizeAt(path.c_str());
}

