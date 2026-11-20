#ifndef GA_UTILITY_RANDOM_HPP
#define GA_UTILITY_RANDOM_HPP

#include <algorithm>
#include <cassert>
#include <chrono>
#include <concepts>
#include <memory>
#include <random>
#include <ranges>
#include <vector>


namespace ga
{

/**
 * @brief A utility class that provides random number generation functionality for the genetic algorithm framework.
 * 
 *  The `Random` class encapsulates a Mersenne Twister random number generator engine.
 *  
 *  A `Random` object can be constructed with a default seed, a `std::mt19937::result_type seed`, or a `std::seed_seq seedSeq`.
 *  The default constructor initializes the engine with a 256-bit seed sequence generated from the current time and random device.
 */
class Random
{
public:
    Random()
        : m_engine { generateMtEngine() }
    { }

    /// @brief Constructs a `Random` object with a given seed for the Mersenne Twister engine.
    Random(
        std::mt19937::result_type seed
    )
        : m_engine { std::mt19937 { seed } }
    { }

    /// @brief Constructs a `Random` object with a given seed sequence for the Mersenne Twister engine.
    Random(
        std::seed_seq& seedSeq
    )
        : m_engine { std::mt19937 { seedSeq } }
    { }

    /**
     * @brief Generates a random integer in the range `[min, max)`.
     * 
     * @tparam T A `std::integral` type, defaults to `int`.
     * 
     * @param min The minimum value (inclusive).
     * @param max The maximum value (exclusive).
     * @return A random integer in the range `[min, max)`.
     * 
     * @pre `min < max`
     */
    template <std::integral T = int>
    T get(T min, T max)
    {
        assert(min < max && "Max value must be greater than min value");

        return std::uniform_int_distribution<T>{min, static_cast<T>(max - 1)}(m_engine);
    }

    /**
     * @brief Generates a random real number in the range `[min, max)`.
     * 
     * @tparam T A `std::floating_point` type, defaults to `double`.
     * 
     * @param min The minimum value (inclusive).
     * @param max The maximum value (exclusive).
     * @return A random real number in the range `[min, max)`.
     * 
     * @pre `min < max`
     */
    template <std::floating_point T = double>
    T get(T min, T max)
    {
        assert(min < max && "Max value must be greater than min value");

        return std::uniform_real_distribution<T>{min, max}(m_engine);
    }

    
    /**
     * @brief Generates a random integer between in the range `[min, max)`, excluding a specific value.
     * 
     * @details
     *  Performs exclusion via index remapping.
     *  
     *  Time complexity: O(1)   
     *  Space complexity: O(1)
     * 
     * @tparam T A `std::integral` type, defaults to `int`.
     * 
     * @param min The minimum value (inclusive).
     * @param max The maximum value (exclusive).
     * @param exclude The value to exclude.
     * @return A random integer in the range `[min, max)` that is not equal to `exclude`.
     * 
     * @pre `min < max`
     * @pre `[min, max)` contains `exclude`
     * @pre If `max - min == 1` then `exclude != min`
     */
    template <std::integral T = int>
    T get(T min, T max, T exclude)
    {
        assert(min < max && "Max value must be greater than min value");
        assert((exclude >= min && exclude < max) && "Excluded value not in requested range");
        assert((min != max - 1 || min != exclude) && "Impossible request: Excluded only possible value");

        T num { get<T>(min, max - 1) };
        
        if (num >= exclude)
            return num + 1;
        else
            return num;
    }
    
    /**
     * @brief Generates a random integer between in the range `[min, max)`, excluding a vector of values.
     * 
     * @details
     *  Performs exclusion via index remapping.
     *  
     *  Time complexity: O(k), for k = size of `exclude`   
     *  Space complexity: O(1)
     * 
     * @tparam T A `std::integral` type, defaults to `int`.
     * 
     * @param min The minimum value (inclusive).
     * @param max The maximum value (exclusive).
     * @param exclude The values to exclude.
     * @return A random integer in the range `[min, max)` that is not in `exclude`.
     * 
     * @pre `min < max`
     * @pre `exclude` is in sorted order
     * @pre `exclude` has no duplicates
     * @pre For all n in `exclude`, `[min, max)` contains `n`
     * @pre `[min, max) != exclude`
     * 
     * @note To keep `get()` efficient, the responsibility for setting up `exclude` correctly (sorted, no duplicates) is placed on the caller.
     *  This is to prevent `get()` from needing to attempt to sort and dedupe every call.
     */
    template <std::integral T = int>
    T get(T min, T max, const std::vector<T>& exclude)
    {
        assert(min < max && "Max value must be greater than min value");
        assert(std::ranges::is_sorted(exclude) && "Exclude list must be in sorted order");
        assert(std::ranges::adjacent_find(exclude) == exclude.end() && "Exclude list must have no duplicates");
        assert((exclude.front() >= min && exclude.back() < max) && "Exclude list must be within [min, max)");

        T validCount { (max - min) - static_cast<T>(exclude.size()) };

        assert(validCount > 0 && "Impossible request: Exclude list excludes all values in given range");

        std::uniform_int_distribution<T> dist {0, validCount - 1};
        T val { min + dist(m_engine) };

        // Add offset value to val
        auto it { std::ranges::lower_bound(exclude, val) };
        val + static_cast<T>(it - exclude.begin());

        // Ensure offset + val is not in exclude
        for (auto n : std::ranges::subrange(it, exclude.end()))
        {
            if (val == n)
                ++val;
            else
                break;
        }

        return val;
    }

private:
    /// @brief Generates a Mersenne Twister engine initialized with a 256 bit seed sequence.
    static constexpr std::mt19937 generateMtEngine()
    {
        std::random_device rd {};
        std::seed_seq ss { 
            static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
            rd(), rd(), rd(), rd(), rd(), rd(), rd()
        };

        return std::mt19937 { ss };
    }

    std::mt19937 m_engine {};

public:
    /// @brief Returns a reference of the underlying uniform random bit generator engine.
    std::mt19937& getEngine() 
    {
        return m_engine;
    }

};

}

#endif
