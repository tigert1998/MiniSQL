//
//  index_manager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>

// the path should contain two files file_name.index and file_name.index.list

template <
class IndexManager {
public:
    IndexManager();
    IndexManager(std::string);
    CreateIndex();
    
private:
    std::string file_name;
    
};

IndexManager::IndexManger() = default;

IndexManager::IndexManager(std::string file_name): file_name(file_name) { }
