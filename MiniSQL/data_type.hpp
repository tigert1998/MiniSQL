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
protected:
    static std::vector<char> raw_value_;
};

std::vector<char> DataType::raw_value_ = {};

class Int: public DataType {
public:
    using OriginalType = int;
    Int() {}
    Int(int value): value_(value) { }
    Int(const char *bits) {
        int temp;
        memcpy((char *) &temp, bits, sizeof(int));
        value_ = temp;
    }
    void set_value(int value) {
        value_ = value;
    }
    int value() const {
        return value_;
    }
    static DataTypeIdentifier GetType() {
        return DataTypeIdentifier::Int;
    }
    const char *raw_value() const {
        raw_value_.resize(sizeof(int));
        for (int i = 0; i < sizeof(int); i++) raw_value_[i] = ((char *) &value_)[i];
        return raw_value_.data();
    }
    uint64_t size() const {
        return sizeof(int);
    }
    
private:
    int value_;
};

// char(n) occupies (n + 1) bytes in total
// and it can represent string whose length <= n
class Char: public DataType {
public:
    using OriginalType = std::string;
    Char() = delete;
    Char(int n): n(n) { }
    Char(int size, const std::string &value): n(size - 1), value_(value) {
        if (value_.length() >= n + 1)
            throw CharLengthExceededError();
    }
    Char(int size, const char *bits): n(size - 1), value_(bits) {
        if (value_.length() >= n + 1)
            throw CharLengthExceededError();
    }
    void set_value(const std::string &value){
        value_ = value;
        if (value_.length() >= n + 1)
            throw CharLengthExceededError();
    }
    std::string value() const {
        return value_;
    }
    static DataTypeIdentifier GetType() {
        return DataTypeIdentifier::Char;
    }
    const char *raw_value() const {
        raw_value_.resize(n + 1);
        strcpy(raw_value_.data(), value_.c_str());
        return raw_value_.data();
    }
    uint64_t size() const {
        return n + 1;
    }
    const Char &operator=(const Char &obj) {
        value_ = obj.value_;
        if (value_.length() >= n + 1)
            throw CharLengthExceededError();
        return obj;
    }
    
    
private:
    const int n;
    std::string value_;
};

class Float: public DataType {
public:
    using OriginalType = float;
    Float() = default;
    Float(float value): value_(value) { }
    Float(const char *bits) {
        float temp;
        memcpy((char *) &temp, bits, sizeof(float));
        value_ = temp;
    }
    void set_value(float value) {
        value_ = value;
    }
    float value() const {
        return value_;
    }
    static DataTypeIdentifier GetType() {
        return DataTypeIdentifier::Float;
    }
    const char *raw_value() const {
        raw_value_.resize(sizeof(float));
        for (int i = 0; i < sizeof(float); i++) raw_value_[i] = ((char *) &value_)[i];
        return raw_value_.data();
    }
    uint64_t size() const {
        return sizeof(float);
    }
    
private:
    float value_;
};

class Uint64_t: public DataType {
public:
    using OriginalType = Uint64_t;
    Uint64_t() = default;
    Uint64_t(uint64_t value): value_(value) { }
    Uint64_t(const char *bits) {
        uint64_t temp;
        memcpy((char *) &temp, bits, sizeof(uint64_t));
        value_ = temp;
    }
    void set_value(uint64_t value) {
        value_ = value;
    }
    uint64_t value() const {
        return value_;
    }
    static DataTypeIdentifier GetType() {
        return DataTypeIdentifier::Uint64_t;
    }
    const char *raw_value() const {
        raw_value_.resize(sizeof(uint64_t));
        for (int i = 0; i < sizeof(uint64_t); i++) raw_value_[i] = ((char *) &value_)[i];
        return raw_value_.data();
    }
    uint64_t size() const {
        return sizeof(uint64_t);
    }
    
private:
    uint64_t value_;
};
