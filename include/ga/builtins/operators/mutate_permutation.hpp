#ifndef GA_BUILTINS_OPERATORS_MUTATE_PERMUTATION_HPP
#define GA_BUILTINS_OPERATORS_MUTATE_PERMUTATION_HPP

#include "ga/utility/constraints.hpp"
#include "ga/utility/hash.hpp"
#include "ga/utility/random.hpp"
#include "ga/utility/debug_checks.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <unordered_set>
#include <vector>


namespace ga::mutate_op
{
    /**
     * @brief Randomly selects an element from the sequence and inserts it at a different position, shifting the other elements accordingly.
     * 
     * @tparam Sequence The type of the sequence to mutate. Must satisfy the `Permutable` concept.
     * @param parent The sequence to mutate.
     * @param rng The random number generator.
     * @return The mutated sequence.
     * 
     * @pre `parent.size() >= 3`
     */
    template <typename Sequence>
    requires(constraints::Permutable<Sequence>)
    Sequence insert(const Sequence& parent, Random& rng)
    {
        const std::ptrdiff_t size { std::ranges::ssize(parent) };
        
        assert(size >= 3 && "Insert mutation requires at least 3 elements in parent");

        std::ptrdiff_t i1 { rng.get<std::ptrdiff_t>(0, size) };
        std::ptrdiff_t i2;

        // Select second index not adjacent to first if size allows
        if (size > 4)
        {
            std::vector<std::ptrdiff_t> excludes;

            // Build excludes of vals adjacent to i1, bound by [0, size)
            if (i1 > 0) excludes.push_back(i1 - 1);
            excludes.push_back(i1);
            if (i1 < size - 1) excludes.push_back(i1 + 1);

            i2 = rng.get<std::ptrdiff_t>(0, size, excludes);
        }
        else
        {
            i2 = rng.get<std::ptrdiff_t>(0, size, i1);
        }

        if (i1 > i2)
            std::swap(i1, i2);

        ++i1; // So that view operations include i1 (i2 is inserted after i1)

        // Extract each section of parent
        const auto gene = parent
                    | std::views::drop(i2)
                    | std::views::take(1);
        
        const auto middle = parent
                    | std::views::drop(i1)
                    | std::views::take(i2 - i1);
        
        // Build child from sections
        Sequence child { parent };
        const auto childBegin { std::ranges::begin(child) };
        std::ptrdiff_t position { i1 };        

        std::ranges::copy(gene, childBegin + position);
        position += gene.size();

        std::ranges::copy(middle, childBegin + position);

        return child;             
    }


    /**
     * @brief Randomly selects a contiguous subsequence of the sequence and reverses its order.
     * 
     * @tparam Sequence The type of the sequence to mutate. Must satisfy the `Permutable` concept.
     * @param parent The sequence to mutate.
     * @param rng The random number generator.
     * @return The mutated sequence.
     * 
     * @pre `parent.size() >= 2`
     */
    template <typename Sequence>
    requires(constraints::Permutable<Sequence>)
    Sequence inversion(const Sequence& parent, Random& rng)
    {
        const std::ptrdiff_t size { std::ranges::ssize(parent) };

        assert(size >= 2 && "Inversion mutation requires at least 2 elements in parent");
        
        if (size < 2)
            throw std::invalid_argument("Invalid sequence size");

        // Select two random indexes
        auto i1 { rng.get<std::ptrdiff_t>(0, size) };
        auto i2 { rng.get<std::ptrdiff_t>(0, size, i1) };
        
        if (i1 > i2)
            std::swap(i1, i2);

        Sequence child { parent };
        const auto childBegin { std::ranges::begin(child) };

        // Invert subsequence between indexes (inclusive)
        std::ranges::reverse(std::ranges::subrange(childBegin + i1, childBegin + i2 + 1));

        return child;
    }

    /**
     * @brief Randomly selects a contiguous subsequence of the sequence and shuffles its elements.
     * 
     * @tparam Sequence The type of the sequence to mutate. Must satisfy the `Permutable` concept.
     * @param parent The sequence to mutate.
     * @param rng The random number generator.
     * @return The mutated sequence.
     * 
     * @pre `parent.size() >= 2`
     */
    template <typename Sequence>
    requires(constraints::Permutable<Sequence> &&
             std::equality_comparable<Sequence>)
    Sequence scramble(const Sequence& parent, Random& rng)
    {
        const std::ptrdiff_t size { std::ranges::ssize(parent) };

        assert(size >= 2 && "Scramble mutation requires at least 2 elements in parent");
        
        // Select two random indexes
        auto i1 { rng.get<std::ptrdiff_t>(0, size) };
        auto i2 { rng.get<std::ptrdiff_t>(0, size, i1) };
        
        if (i1 > i2)
            std::swap(i1, i2);

        Sequence child { parent };

        auto childView = child 
                       | std::views::drop(i1)
                       | std::views::take(i2 - i1 + 1);
                       
        const auto parentView = parent 
                       | std::views::drop(i1)
                       | std::views::take(i2 - i1 + 1);

        // Ensure that shuffle does not randomly place elements in original order
        do
        {
            std::ranges::shuffle(childView, rng.getEngine());
        } while (std::ranges::equal(childView, parentView));

        return child;
    }

    /**
     * @brief Randomly selects two elements from the sequence and swaps their positions.
     * 
     * @tparam Sequence The type of the sequence to mutate. Must satisfy the `Permutable` concept.
     * @param parent The sequence to mutate.
     * @param rng The random number generator.
     * @return The mutated sequence.
     * 
     * @pre `parent.size() >= 2`
     */
    template <typename Sequence>
    requires(constraints::Permutable<Sequence>)
    Sequence swap(const Sequence& parent, Random& rng)
    {
        const std::ptrdiff_t size { std::ranges::ssize(parent) };

        assert(size >= 2 && "Swap mutation requires at least 2 elements in parent");
            
        // Select two random indexes
        const auto i1 { rng.get<std::ptrdiff_t>(0, size) };
        const auto i2 { rng.get<std::ptrdiff_t>(0, size, i1) };

        Sequence child { parent };
        const auto childBegin { std::ranges::begin(child) };

        std::ranges::swap(childBegin[i1], childBegin[i2]);

        return child;
    }
}


#endif
