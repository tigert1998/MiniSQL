//
//  data_type.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <vector>

class DataType {
public:
    const char *raw_value() {
        return raw_value_.data();
    }
    uint64_t size() {
        return raw_value_.size();
    }
    
protected:
    std::vector<char> raw_value_;
};

class Int: public DataType {
public:
    Int() = default;
    Int(int value) {
        set_value(value);
    }
    Int(const char *bits) {
        int temp;
        memcpy((char *) &temp, bits, sizeof(int));
        set_value(temp);
    }
    void set_value(int value) {
        value_ = value;
        raw_value_.resize(sizeof(int));
        for (int i = 0; i < sizeof(int); i++) raw_value_[i] = ((char *) &value)[i];
    }
    int value() {
        return value_;
    }
    
private:
    int value_;
};

class Char: public DataType {
public:
    Char() = default;
    void set_value(const char *value, int length) {
        raw_value_.resize(length);
        for (int i = 0; i < length; i++)
            raw_value_[i] = value[i];
    }
    const char *value() {
        return raw_value_.data();
    }
};

class Float: public DataType {
public:
    Float() = default;
    Float(float value) {
        set_value(value);
    }
    Float(const char *bits) {
        float temp;
        memcpy((char *) &temp, bits, sizeof(float));
        set_value(temp);
    }
    void set_value(float value) {
        value_ = value;
        raw_value_.resize(sizeof(float));
        for (int i = 0; i < sizeof(float); i++) raw_value_[i] = ((char *) &value)[i];
    }
    float value() {
        return value_;
    }
    
private:
    float value_;
};
