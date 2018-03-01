//
// Created by karbal on 22.2.18.
//

#ifndef DGRMINER_COMMON_UTILS_H
#define DGRMINER_COMMON_UTILS_H


template <typename T> bool equal(const T& t1, const T& t2)
{
    return t1 == t2;
}

template <typename T> bool equal(const std::vector<T> & t1, const std::vector<T> & t2)
{
    if (t1.size() == t2.size())
    {
        for (int i = 0; i < t1.size(); ++i)
        {
            bool result = equal(t1.at(i), t2.at(i));
            if (!result) return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}


#endif //DGRMINER_COMMON_UTILS_H


