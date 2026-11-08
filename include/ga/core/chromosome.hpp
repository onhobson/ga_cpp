#ifndef GA_CHROMOSOME_HPP
#define GA_CHROMOSOME_HPP

namespace ga
{
    
template <typename Sequence, typename Fitness>
struct Chromosome
{
    Sequence sequence {};
    Fitness fitness {};
};

template <typename Sequence, typename Fitness>
inline bool operator==(const Chromosome<Sequence, Fitness>& lhs, const Chromosome<Sequence, Fitness>& rhs)
{
    return (lhs.sequence == rhs.sequence) && (lhs.fitness == rhs.fitness);
}

}

#endif
