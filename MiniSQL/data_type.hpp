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
    Int, Char, Float, Uint64_t
};

class DataType {
public:
    const char *raw_value() {
        return raw_value_.data();
    }
    uint64_t size() const {
        return raw_value_.size();
    }
    virtual DataTypeIdentifier GetType() const = 0;
    
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
    Int(int, const char *bits) {
        int temp;
        memcpy((char *) &temp, bits, sizeof(int));
        set_value(temp);
    }
    void set_value(int value) {
        value_ = value;
        raw_value_.resize(sizeof(int));
        for (int i = 0; i < sizeof(int); i++) raw_value_[i] = ((char *) &value)[i];
    }
    int value() const {
        return value_;
    }
    DataTypeIdentifier GetType() const {
        return DataTypeIdentifier::Int;
    }
    
private:
    int value_;
};

// char(n) occupies (n + 1) bytes in total
// and it can represent string whose length <= n
class Char: public DataType {
public:
    Char() = delete;
    Char(int n): n(n) {
        raw_value_.resize(n + 1);
    }
    Char(int n, const std::string &value): n(n) {
        if (value.length() >= n + 1)
            throw CharLengthExceededError();
        set_value(value);
    }
    Char(int n, const char *bits): n(n) {
        raw_value_.resize(n + 1);
        strcpy(raw_value_.data(), bits);
    }
    void set_value(const std::string &value) {
        if (value.length() >= n + 1)
            throw CharLengthExceededError();
        value_ = value;
        raw_value_.resize(n + 1);
        strcpy(raw_value_.data(), value.c_str());
    }
    std::string value() const {
        return value_;
    }
    DataTypeIdentifier GetType() const {
        return DataTypeIdentifier::Char;
    }
    
private:
    const int n;
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
    Float(int, const char *bits) {
        float temp;
        memcpy((char *) &temp, bits, sizeof(float));
        set_value(temp);
    }
    void set_value(float value) {
        value_ = value;
        raw_value_.resize(sizeof(float));
        for (int i = 0; i < sizeof(float); i++) raw_value_[i] = ((char *) &value)[i];
    }
    float value() const {
        return value_;
    }
    DataTypeIdentifier GetType() const {
        return DataTypeIdentifier::Float;
    }
    
private:
    float value_;
};

class Uint64_t: public DataType {
public:
    Uint64_t() {
        raw_value_.resize(sizeof(uint64_t));
    }
    Uint64_t(uint64_t value) {
        set_value(value);
    }
    Uint64_t(const char *bits) {
        uint64_t temp;
        memcpy((char *) &temp, bits, sizeof(uint64_t));
        set_value(temp);
    }
    void set_value(uint64_t value) {
        value_ = value;
        raw_value_.resize(sizeof(uint64_t));
        for (int i = 0; i < sizeof(uint64_t); i++) raw_value_[i] = ((char *) &value)[i];
    }
    uint64_t value() const {
        return value_;
    }
    DataTypeIdentifier GetType() const {
        return DataTypeIdentifier::Uint64_t;
    }
    
private:
    uint64_t value_;
};
