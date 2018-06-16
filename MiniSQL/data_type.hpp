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

#include "database_exception.hpp"

enum class DataTypeIdentifier {
    Int, Char, Float
};

class DataType {
public:
    const char *raw_value() {
        return raw_value_.data();
    }
    uint64_t size() {
        return raw_value_.size();
    }
    virtual DataTypeIdentifier GetType() = 0;
    
protected:
    std::vector<char> raw_value_;
};

class Int: public DataType {
public:
    Int() {
        raw_value_.resize(sizeof(int));
    }
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
    DataTypeIdentifier GetType() {
        return DataTypeIdentifier::Int;
    }
    
private:
    int value_;
};

// char(N) occupies (N + 1) bytes in total
// and it can represent string whose length <= N
template <int N>
class Char: public DataType {
public:
    Char() {
        raw_value_.resize(N + 1);
    }
    Char(const std::string &value) {
        if (value.length() >= N + 1)
            throw CharLengthExceededError();
        set_value(value);
    }
    Char(const char *bits) {
        raw_value_.resize(N + 1);
        strcpy(raw_value_.data(), bits);
    }
    void set_value(const std::string &value) {
        if (value.length() >= N + 1)
            throw CharLengthExceededError();
        value_ = value;
        raw_value_.resize(N + 1);
        strcpy(raw_value_.data(), value.c_str());
    }
    std::string value() {
        return value_;
    }
    DataTypeIdentifier GetType() {
        return DataTypeIdentifier::Char;
    }
    
private:
    std::string value_;
};

class Float: public DataType {
public:
    Float() {
        raw_value_.resize(sizeof(float));
    }
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
    DataTypeIdentifier GetType() {
        return DataTypeIdentifier::Float;
    }
    
private:
    float value_;
};
