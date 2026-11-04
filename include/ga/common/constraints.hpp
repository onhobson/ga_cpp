#ifndef GA_CONSTRAINTS_HPP
#define GA_CONSTRAINTS_HPP

#include "ga/common/hash.hpp"

#include <concepts>
#include <ranges>
#include <type_traits>


namespace ga::constraints
{
    /**
     * @brief A concept that checks if a type is a permutable sequence.
     * 
     * @returns `true` if the type is copy constructible, a random access range, a sized range, and permutable; otherwise, returns `false`.
     */
    template <typename Sequence>
    concept Permutable =
        std::copy_constructible<Sequence> &&
        std::ranges::random_access_range<Sequence> &&
        std::ranges::sized_range<Sequence> &&
        std::permutable<std::ranges::iterator_t<Sequence>>;
        

    /**
     * @brief A concept that checks if a type is hashable.
     * 
     * @returns `true` if the type is equality comparable and can be hashed using `ga::Hash<T>`; otherwise, returns `false`.
     */
    template <typename T>
    concept Hashable = 
        std::equality_comparable<T> &&
        requires(const T& v)
        {
            { ga::Hash<T>{}(v) } -> std::convertible_to<std::size_t>;
        }; 

    /**
     * @brief A concept that checks if a type has a `value_type` member.
     * 
     * @returns `true` if the type has a `value_type` member; otherwise, returns `false`.
     */
    template <typename T>
    concept HasValueType =
        requires()
        {
            typename T::value_type;
        };

}

#endif
