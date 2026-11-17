#ifndef GA_UTILITY_HASH_HPP
#define GA_UTILITY_HASH_HPP

#include <functional>


namespace ga
{
    /**
     * @brief A struct that provides a hash function for a given type.
     * 
     * @note This struct inherits from `std::hash<T>`, allowing it to be used as a hash function object in standard containers and algorithms.
     */
    template <typename T>
    struct Hash : std::hash<T> {};
}


#endif
