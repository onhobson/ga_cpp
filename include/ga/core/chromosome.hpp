#ifndef GA_CHROMOSOME_HPP
#define GA_CHROMOSOME_HPP

namespace ga
{
    
template <typename Sequence, typename Fitness>
struct Chromosome
{
    Sequence sequence {};
    Fitness fitness { 0 };
};

}

#endif
