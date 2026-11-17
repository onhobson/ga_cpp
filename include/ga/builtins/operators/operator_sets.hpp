#ifndef GA_BUILTINS_OPERATOR_SETS_HPP
#define GA_BUILTINS_OPERATOR_SETS_HPP

#include "ga/builtins/operators/crossover_permutation.hpp"
#include "ga/builtins/operators/mutate_permutation.hpp"

#include "ga/operations/operators.hpp"

#include "ga/utility/constraints.hpp"


namespace ga::op_set
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
                    crossover_op::alternatingEdge<Sequence>,
                    crossover_op::order<Sequence>,
                    crossover_op::edgeRecombination<Sequence>
                },
                { 1.0, 1.0, 1.0, }
            },
            Mutate<Sequence> {
                {
                    mutate_op::insert<Sequence>,
                    mutate_op::inversion<Sequence>,
                    mutate_op::scramble<Sequence>,
                    mutate_op::swap<Sequence>,
                },
                { 1.0, 1.0, 1.0, 1.0 }
            }
        };
    }

}


#endif
