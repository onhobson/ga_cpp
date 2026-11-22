# Operator Guide {#operators}

#### What is an Operator?

An operator is a callable used to generate new individuals from existing ones. Operators are a key part of the genetic algorithm process because they let the search explore the solution space by combining or altering existing solutions.

This library uses two main operator categories:
- @ref ga::Crossover "ga::Crossover": combines two parent sequences into a child sequence
- @ref ga::Mutate "ga::Mutate": transforms a single parent sequence into a child sequence

When the algorithm invokes a crossover or mutation step, the relevant selector object randomly chooses one of the provided operations according to the configured weights.

#### What is an Operator Set?

An operator set, under the namespace @ref ga::op_set, groups related crossover and mutation operators into a single reusable callable. These sets return a ready-to-use @ref ga::Operators object initialized with the built-in operators and equal weights.

@code
// Using op_set directly within GeneticAlgorithm constructor
ga::GeneticAlgorithm<Sequence, Fitness> ga {
    evalFunc,
    ga::op_set::permutationOperations<Sequence>(),
    config
};
@endcode

#### Using Operators

Operators are supplied to the genetic algorithm by passing a @ref ga::Operators<Sequence> instance to the @ref ga::GeneticAlgorithm constructor.

The @ref ga::Operators struct contains two members:
- `crossover`, a @ref ga::Crossover<Sequence>
- `mutate`, a @ref ga::Mutate<Sequence>

Each member may be constructed in one of three ways (examples show @ref ga::Crossover<Sequence>, but applies to @ref ga::Mutate<Sequence> as well):

1. Single operator   
The provided operation is always selected whenever that step is invoked.
@code
ga::Crossover<Sequence> crossOps {
    crossoverOperation
};
@endcode

2. Vector of operators   
One operator is selected randomly with equal weights when the step is invoked.
@code
ga::Crossover<Sequence> crossOps {
    { crossoverOperation1, crossoverOperation2, ...}
};
@endcode

3. Vector of operators with a vector of weights   
One operator is selected randomly according to the supplied weighted probabilities.
@code
ga::Crossover<Sequence> crossOps {
    { crossoverOperation1, crossoverOperation2, ...},
    { 0.5, 1.0, ...}
};
@endcode

A @ref ga::Operators<Sequence> object must then be instantiated with the @ref ga::Crossover<Sequence> and @ref ga::Mutate<Sequence> objects:

@code
ga::Operators<Sequence> operators {
    crossOps,
    mutateOps
};
@endcode

Alternatively, @ref ga::Operators<Sequence> accepts temporary objects as well:

@code
ga::Operators<Sequence> operators {
    ga::Crossover<Sequence> {
        { crossoverOperation1, crossoverOperation2 }
    },
    ga::Mutate<Sequence> {
        { mutateOperation1, mutateOperation2 }
    }
};
@endcode

### Built-in Operators

The library provides built-in permutation operators for immediate use.

Permutation crossover operators (in `ga::crossover_op`):
- `alternatingEdge()`
- `order`
- `edgeRecombination()`

Permutation mutation operators (in `ga::mutate_op`):
- `insert()`
- `inversion()`
- `scramble()`
- `swap()`

Operator set containing the built-in permutation operators:
- `ga::op_set::permutationOperators()`

All built-in permutation operators require the sequence used by the algorithm to satisfy @ref ga::constraints::Permutable.

### Custom Operators

A crossover operator accepts two parent sequences and returns one child sequence.

@code
Sequence crossover(const Sequence& parent1, const Sequence& parent2, ga::Random& rng);
@endcode

A mutation operator accepts one parent sequence and returns one child sequence.

@code
Sequence mutate(const Sequence& parent, ga::Random& rng);
@endcode

Both custom operators can be passed into the relevant fields of a @ref ga::Operators object.

#### Example

The following code creates a @ref ga::Operators instance with two custom crossover operations weighted `0.25` and `0.75`, and two custom mutation operations assigned default equal weights.

@code
ga::Operators<Sequence> operators {
    ga::Crossover<Sequence> {
        { customCrossover1, customCrossover2 },
        { 0.25, 0.75 }
    },
    ga::Mutate<Sequence> {
        { customMutate1, customMutate2 }
    }
};
@endcode

This pattern allows a custom implementation to reuse the library's weighted operator selection behavior without requiring a custom selection system.

