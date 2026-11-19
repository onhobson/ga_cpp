#ifndef GA_UTILITY_RANDOM_HPP
#define GA_UTILITY_RANDOM_HPP

#include <algorithm>
#include <cassert>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>


namespace ga
{
    
/** @brief A class that provides a uniform random number generator with an erased engine type. */
class Random
{
public:
    // Copying and moving are disallowed to ensure reliable seeding
    Random(const Random&) = delete;
    Random& operator==(const Random&) = delete;

    /**
     * @brief Constructs a Random object using the given uniform random bit generator engine.
     * @param URBG The uniform random bit generator engine. Must satisfy the requirements of a uniform random bit generator.
     */
    template <std::uniform_random_bit_generator URBG>
    explicit Random(URBG engine) 
        : m_engine { std::make_unique<EngineModel<URBG>>(std::move(engine)) }
        , m_impl { *m_engine }
    { }
 


    /**
     * @brief Generates a random integer in the range [min, max).
     * 
     * @tparam T A `std::integral` type, defaults to `int`.
     * 
     * @param min The minimum value (inclusive).
     * @param max The maximum value (exclusive).
     * @return A random integer in the range [min, max).
     * 
     * @pre `min < max`
     */
    template <std::integral T = int>
    T get(T min, T max)
    {
        assert(min < max && "Max value must be greater than min value");

        return std::uniform_int_distribution<T>{min, static_cast<T>(max - 1)}( m_impl );
    }

    /**
     * @brief Generates a random real number in the range [min, max).
     * 
     * @tparam T A `std::floating_point` type, defaults to `double`.
     * 
     * @param min The minimum value (inclusive).
     * @param max The maximum value (exclusive).
     * @return A random real number in the range [min, max).
     * 
     * @pre `min < max`
     */
    template <std::floating_point T = double>
    T get(T min, T max)
    {
        assert(min < max && "Max value must be greater than min value");

        return std::uniform_real_distribution<T>{min, max}(m_impl);
    }

    
    /**
     * @fn template <std::integral T = int> T get(T min, T max, T exclude)
     * @brief Generates a random integer between in the range [min, max), excluding a specific value.
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
     * @return A random integer in the range [min, max) that is not equal to `exclude`.
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
     * @fn template <std::integral T = int> T get(T min, T max, const std::vector<T>& exclude)
     * @brief Generates a random integer between in the range [min, max), excluding a vector of values.
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
     * @return A random integer in the range [min, max) that is not in `exclude`.
     * 
     * @pre `min < max`
     * @pre `exclude` is in sorted order
     * @pre `exclude` has no duplicates
     * @pre For all n in `exclude`, `[min, max)` contains `n`
     * @pre `[min, max) != exclude`
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

        std::uniform_int_distribution<T> dist {0, validCount};
        T val { min + dist(m_impl) };

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

    /**
     * @brief Generates a random integer between `min` and `max` (inclusive).
     * 
     * @tparam T The type of the real number, defaulting to `int`.
     * 
     * @param min The minimum value (inclusive).
     * @param max The maximum value (inclusive).
     * @return A random integer in the range [min, max].
     * 
     * @throws `std::invalid_argument` If `min` is greater than `max`.
     */
    template <std::integral T = int>
    T getInteger(T min, T max) {
        if (min > max)
            throw std::invalid_argument("min value greater than max value");

        return std::uniform_int_distribution<T>{min, max}( m_impl );
    }


    /**
     * @brief Generates a random integer between `min` and `max` (inclusive), excluding a specific value.
     * 
     * @tparam T The type of the real number, defaulting to `int`.
     * 
     * @param min The minimum value (inclusive).
     * @param max The maximum value (inclusive).
     * @param exclude The value to exclude.
     * @return A random integer in the range [min, max] that is not equal to `exclude`.
     * 
     * @throws `std::invalid_argument` If `min` is greater than `max`.
     * @throws `std::invalid_argument` If `min` equals `max` and both equal `exclude`.
     */
    template <std::integral T = int>
    T getIntegerExclude(T min, T max, T exclude) {
        if (min > max)
            throw std::invalid_argument("min value greater than max value");
        if (min == max && max == exclude)
            throw std::invalid_argument("Impossible request: min == max == exclude");

        T num;
        std::uniform_int_distribution<T> dist {min, max};

        do
        {
            num = dist(m_impl);
        } while (num == exclude);
        
        return num;
    }

    /**
     * @brief Generates a random integer between `min` and `max` (inclusive), excluding specific values.
     * @throws `std::invalid_argument` If `min` is greater than `max` or every value in the range is excluded.
     */
    template <std::integral T = int>
    T getIntegerExclude(T min, T max, const std::vector<T>& exclude) {
        if (min > max)
            throw std::invalid_argument("min value greater than max value");

        // Clean exclude: bound, sort & remove duplicates
        std::vector<T> excludedInRange;
        for (const T value : exclude) {
            if (value >= min && value <= max)
                excludedInRange.push_back(value);
        }
        std::ranges::sort(excludedInRange);
        excludedInRange.erase(std::unique(excludedInRange.begin(), excludedInRange.end()), excludedInRange.end());

        // Iterate each possible value until one is not contained in exclude list
        T expected = min;
        bool allExcluded = true;
        bool reachedMax = false;
        for (const T value : excludedInRange) {
            if (value != expected) {
                allExcluded = false;
                break;
            }
            if (value == max) {
                reachedMax = true;
                break;
            }
            ++expected;
        }
        if (allExcluded && reachedMax)
            throw std::invalid_argument("Impossible request: every value in range is excluded");

        std::uniform_int_distribution<T> dist {min, max};
        T num;
        do {
            num = dist(m_impl);
        } while (std::ranges::binary_search(excludedInRange, num));

        return num;
    }

    /**
     * @brief Generates a random real number between `min` and `max`.
     * 
     * @tparam T The type of the real number, defaulting to `double`.
     * 
     * @param min The minimum value (inclusive).
     * @param max The maximum value (exclusive).
     * @return A random real number in the range [min, max).
     * 
     * @throws `std::invalid_argument` If `min` is greater than `max`.
     */
    template <std::floating_point T = double>
    T getReal(T min, T max) {
        if (min > max)
            throw std::invalid_argument("min value greater than max value");

        return std::uniform_real_distribution<T>{min, max}(m_impl);
    }
    
    /// @brief Generates a Mersenne Twister engine initialized with a 256 bit seed sequence.
    static constexpr std::mt19937 generateMtEngine()
    {
        std::random_device rd {};
        std::seed_seq ss { 
            static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
            rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() 
        };

        return std::mt19937 { ss };
    }

private:
    // Abstract Interface 
    struct EngineConcept
    {
        using result_type = uint32_t;

        virtual ~EngineConcept() = default;

        virtual result_type next() = 0;
    };

    // Templated Implementation
    template <std::uniform_random_bit_generator URBG>
    class EngineModel : public EngineConcept
    {
    public:
        static constexpr std::size_t result_width = 32;

        EngineModel(URBG engine) 
            : m_model { std::move(engine) } 
        { }

        result_type next() override { return m_model(); }

    private:
        std::independent_bits_engine<URBG, result_width, result_type> m_model;
    };

    // Type Erasure Wrapper
    class ErasedURBG
    {
    public:
        using result_type = uint32_t;

        explicit ErasedURBG(EngineConcept& engine)
            : m_engine { engine }
        { }

        static constexpr result_type min() noexcept
        {
            return 0;
        }

        static constexpr result_type max() noexcept 
        {
            return std::numeric_limits<result_type>::max();
        }

        result_type operator()()
        {
            return m_engine.next();
        }
        
    private:
        EngineConcept& m_engine;
    };

    std::unique_ptr<EngineConcept> m_engine;
    ErasedURBG m_impl;


    
public:
    /// @brief Returns a copy of the underlying uniform random bit generator engine.
    ErasedURBG getEngine() 
    {
        return ErasedURBG { *m_engine };
    }

};

}

#endif
