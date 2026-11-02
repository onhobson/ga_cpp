#ifndef GA_COMMON_HASH_HPP
#define GA_COMMON_HASH_HPP


#include <functional>

namespace ga
{
    template <typename T>
    struct Hash : std::hash<T> {};
}


#endif
