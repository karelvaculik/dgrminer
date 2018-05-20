//
// Created by karbal on 27.2.18.
//

#ifndef DGRMINER_HELPERPRINTFUNCTIONS_H
#define DGRMINER_HELPERPRINTFUNCTIONS_H


#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

using namespace std;

inline void print();
inline void println();
template <typename T> inline void print(const T& t);
template <typename T> inline void println(const T& t);
template <typename First, typename... Rest> inline void print(const First& first, const Rest&... rest);
template <typename First, typename... Rest> inline void println(const First& first, const Rest&... rest);
template <typename T1, typename T2> inline void print_pair(const std::pair<T1, T2> & t);
template <typename T1, typename T2> inline void print(const std::pair<T1, T2> & t);
template <typename T1, typename T2> inline void println(const std::pair<T1, T2> & t);
template <typename T> inline void print_vector(const std::vector<T> & t);
template <typename T> inline void print(const std::vector<T> & t);
template <typename T> inline void println(const std::vector<T> & t);
template<typename T, std::size_t size> inline void print(const std::array<T, size> & t);
template<typename T, std::size_t size> inline void println(const std::array<T, size> & t);
template <typename T> inline void print_set(const std::set<T> &t);
template <typename T> inline void print(const std::set<T> & t);
template <typename T> inline void println(const std::set<T> & t);
template <typename T1, typename T2> void print_unordered_map(const std::unordered_map<T1, T2> &t);
template <typename T1, typename T2> void print(const std::unordered_map<T1, T2> &t);
template <typename T1, typename T2> void println(const std::unordered_map<T1, T2> &t);
template <typename... Rest> inline void debug_print(bool verbose, const Rest &... rest);
template <typename... Rest> inline void debug_println(bool verbose, const Rest &... rest);


#include "HelperPrintFunctions.cpp"


#endif //DGRMINER_HELPERPRINTFUNCTIONS_H
