#ifndef GA__OPERATORS_FUNCTIONS_BUNDLES_HPP
#define GA__OPERATORS_FUNCTIONS_BUNDLES_HPP

#include "ga/operations/operators.hpp"
#include "ga/common/constraints.hpp"
#include "ga/operations/functions/crossover/permutation.hpp"
#include "ga/operations/functions/mutate/permutation.hpp"



namespace ga::prebuilt
{

    /** @brief Builds Operator<Sequence> object initialized with prebuilt crossover and mutation functions for permutation sequences with equal probabilities.
     * 
     * The crossover functions included are alternating edge (AEX), order (OX), and edge recombination (ERX).
     * The mutation functions included are insert, inversion, scramble, and swap.
     * 
     * @tparam Sequence The type of the sequence to operate on. Must satisfy the `Permutable`, `HasValueType`, and `Hashable` concepts.
     */
    template <typename Sequence>
    requires(constraints::Permutable<Sequence> &&
             constraints::HasValueType<Sequence> &&
             constraints::Hashable<typename Sequence::value_type>)
    inline Operators<Sequence> permutationOperators()
    {
        return Operators<Sequence> {
            Crossover<Sequence> {
                {
                    functions::crossover::alternatingEdge<Sequence>,
                    functions::crossover::order<Sequence>,
                    functions::crossover::edgeRecombination<Sequence>
                },
                { 1.0, 1.0, 1.0, }
            },
            Mutate<Sequence> {
                {
                    functions::mutate::insert<Sequence>,
                    functions::mutate::inversion<Sequence>,
                    functions::mutate::scramble<Sequence>,
                    functions::mutate::swap<Sequence>,
                },
                { 1.0, 1.0, 1.0, 1.0 }
            }
        };
    }

}


#endif
