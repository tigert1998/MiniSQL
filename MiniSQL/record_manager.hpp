//
//  record_manager.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/15.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include "table_item.hpp"
#include "buffer_manager.hpp"
#include "constant.hpp"

class RecordManager {
public:
    RecordManager() = delete;
    RecordManager(const std::string &);
    void Insert(const Record &);
    
private:
    BufferManager<kBlockNumber, kBlockSize> buffer_manager;
    
};

RecordManager::RecordManager(const std::string &data_path): buffer_manager(data_path) { }

void RecordManager::Insert(const Record &record) {
    
}
