#ifndef GA_UTILITY_DEBUG_CHECKS
#define GA_UTILITY_DEBUG_CHECKS

#include <algorithm>

namespace ga::debug
{
    
template <typename T>
bool hasDuplicates(T seq)
{
    std::ranges::sort(seq);
    return std::ranges::adjacent_find(seq) != seq.end();
}

template <typename T>
bool arePermutations(T seq1, T seq2)
{
    std::ranges::sort(seq1);
    std::ranges::sort(seq2);
    return std::ranges::equal(seq1, seq2);
}

}

#endif
