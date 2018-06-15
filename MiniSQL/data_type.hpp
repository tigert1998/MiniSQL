//
//  data_type.hpp
//  MiniSQL
//
//  Created by tigertang on 2018/6/14.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <vector>
#include <string>

enum class DataTypeIdentifier {
    Int, Char, Float
};

class DataType {
public:
    const char *raw_value() {
        return raw_value_.data();
    }
    unsigned long size() {
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
    Char(const char *value) {
        set_value(value);
    }
    void set_value(const char *value) {
        value_ = value;
    }
    const char *value() {
        return value_.c_str();
    }
    const char *raw_value() {
        return value();
    }
    unsigned long size() {
        return value_.size() + 1;
    }
private:
    std::string value_;
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
