# Library Definitions {#definitions}

### Chromosome {#chromosome}

An individual tracked by the genetic algorithm. A chromosome contains a @ref sequence "sequence" representing a candidate solution and its @ref fitness "fitness" value. The algorithm evaluates and ranks chromosomes as it evolves the @ref population "population".

### Sequence {#sequence}

The data representing one candidate solution to the optimization problem. The algorithm's crossover and mutation operators create new sequences, and the evaluation function uses a sequence to calculate its @ref fitness "fitness".

### Fitness {#fitness}

The value returned when the evaluation function evaluates a @ref sequence "sequence". The algorithm uses fitness values to rank @ref chromosome "chromosomes", select @ref elite "elites", and track the @ref bestSolution "best solution". The configured comparison type determines which fitness values rank ahead of others.

### Population {#population}

The collection of @ref chromosome "chromosomes" in the current generation. Each chromosome contains a candidate @ref sequence "sequence" and its evaluated @ref fitness "fitness".

### Elite {#elite}

An elite is one of the highest-ranked chromosomes in the current population, according to fitness and the configured comparison type. Elites are copied into the next generation unchanged. The number preserved is set by @ref ga::Config::eliteCount.

### Best Solution {#bestSolution}

The best chromosome found across all generations evaluated during a run, according to the configured fitness comparison. Unlike an elite, which is preserved from the current population into the next generation, the best solution is tracked separately, ensuring the best solution is not lost between generations, and returned in the run's @ref ga::Result.
