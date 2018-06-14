//
//  main.cpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include "BufferManager.hpp"

BufferManager<20> buffer_manager;

int main() {
    buffer_manager.Open("test");
    buffer_manager.Read(432);
    return 0;
}
