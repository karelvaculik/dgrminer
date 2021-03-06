//
// Created by karbal on 27.2.18.
//

#ifdef DGRMINER_HELPERPRINTFUNCTIONS_H


#include "HelperPrintFunctions.h"


inline void print()
{
    cout << endl;
}
inline void println()
{
    cout << endl;
}

template <typename T> void print(const T& t)
{
    cout << t;
}
template <typename T> void println(const T& t)
{
    cout << t << endl;
}


template <typename First, typename... Rest> void print(const First& first, const Rest&... rest)
{
    cout << first;
    print(rest...); // recursive call using pack expansion syntax
}

template <typename First, typename... Rest> void println(const First& first, const Rest&... rest)
{
    cout << first;
    println(rest...); // recursive call using pack expansion syntax
}


template <typename T1, typename T2> void print_pair(const std::pair<T1, T2> & t)
{
    print("(");
    print(t.first);
    print(", ");
    print(t.second);
    print(")");
}


template <typename T1, typename T2> void print(const std::pair<T1, T2> & t)
{
    print_pair(t);
}

template <typename T1, typename T2> void println(const std::pair<T1, T2> & t)
{
    print_pair(t);
    println("");
}


template <typename T> void print_vector(const std::vector<T> & t)
{
    print("[");
    for (int i = 0; i < t.size(); ++i) {
        print(t[i]);
        if (i < t.size() - 1)
        {
            print(", ");
        }

    }
    print("]");
}


template <typename T> void print(const std::vector<T> & t)
{
    print_vector(t);
}

template <typename T> void println(const std::vector<T> & t)
{
    print_vector(t);
    println("");
}


template <typename T> void print_set(const std::set<T> &t)
{
    print("{");
    int i = 0;
    for (auto & el : t)
    {
        print(el);
        if (i < t.size() - 1)
        {
            print(", ");
        }
        i++;
    }
    print("}");
}

template <typename T> void print(const std::set<T> & t)
{
    print_set(t);
}

template <typename T> void println(const std::set<T> & t)
{
    print_set(t);
    println("");
}

template <typename T1, typename T2> void print_unordered_map(const std::unordered_map<T1, T2> &t)
{
    print("{");
    int i = 0;
    for (auto & kv : t)
    {
        print(kv.first);
        print(": ");
        print(kv.second);
        if (i < t.size() - 1)
        {
            print(", ");
        }
        i++;
    }
    print("}");
}

template <typename T1, typename T2> void print(const std::unordered_map<T1, T2> &t)
{
    print_unordered_map(t);
}

template <typename T1, typename T2> void println(const std::unordered_map<T1, T2> &t)
{
    print_unordered_map(t);
    println("");
}


template<typename T, std::size_t size> inline void print_array(const std::array<T, size> & t)
{
    print("[");
    for (int i = 0; i < size; ++i) {
        print(t[i]);
        if (i < size - 1)
        {
            print(", ");
        }

    }
    print("]");
}


template<typename T, std::size_t size> inline void print(const std::array<T, size> & t)
{
    print_array(t);
}


template<typename T, std::size_t size> inline void println(const std::array<T, size> & t)
{
    print_array(t);
    println("");
}


template <typename... Rest> void debug_print(bool verbose, const Rest &... rest)
{
    if (verbose) print(rest...);
}

template <typename... Rest> void debug_println(bool verbose, const Rest &... rest)
{
    if (verbose) println(rest...);
}


#endif
