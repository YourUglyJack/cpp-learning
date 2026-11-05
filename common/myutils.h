#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>


void print_title(std::string title) {
    std::cout << "============= " << title << " ======================\n";
}

template<typename T>
void print_vector(const std::vector<T>& v, const std::string& name="") {
    if (!name.empty()) {
        std::cout << name << ": ";
    }
    std::cout << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i];
        if (i + 1 < v.size()) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
}

template<typename K, typename V>
void print_map(const std::map<K, V>& m, const std::string& name="") {
    if (!name.empty()) {
        std::cout << name << ": ";
    }
    std::cout << "{";
    bool first = true;
    for (const auto& [k, v] : m) {
        if(!first) std::cout << ", ";
        std::cout << k << ": " << v;
        first = false;
    }
    std::cout << "}\n";
}

template<typename T>
void print_set(const std::set<T>& s, const std::string& name="") {
    if (!name.empty()) {
        std::cout << name << ": ";
    }
    std::cout << "{";
    bool first = true;
    for (const auto& elem : s) {
        if (!first) std::cout << ", ";
        std::cout << elem;
        first = false;
    }
    std::cout << "}\n";
}