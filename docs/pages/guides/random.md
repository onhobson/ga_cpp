# Random Guide {#random}

#### What is @ref ga::Random "ga::Random"?

@ref ga::Random is the library's common interface for random number generation. It accepts any engine that satisfies `std::uniform_random_bit_generator` and stores it behind a type-erased adapter. This lets library components use the same non-templated random interface without depending on the concrete engine type.

When constructed, @ref ga::Random takes ownership of the supplied engine. Internally, it adapts the engine to a uniform 32-bit generator; callers can use @ref ga::Random::get to draw values.

#### Generating Values

@ref ga::Random::get is the overload set for generating values of type `T`.
Each overload accepts minimum and maximum values of type `T` and returns a value in the half-open interval `[min, max)`.

Overloads are provided for types satisfying `std::integral` and `std::floating_point`.
Calls with an integral type may additionally pass an `exclude` value or `std::vector<T>` of excluded values.
In these cases, the returned value is in `[min, max)` and is not equal to the single excluded value or is not present in the excluded value list.

Callers must satisfy the preconditions documented for the corresponding @ref ga::Random::get overload; violating them results in undefined behavior.

@code
// Create Random object
ga::Random rng { ga::Random::generateMtEngine() };

// Generate random int between 0 and 10 (exclusive)
int val = rng.get<int>(0, 10);

// Generate random double between 0.0 and 1.0 (exclusive)
double val = rng.get<double>(0.0, 1.0);

// Generate random int between 0 and 10, omitting 6
int val = rng.get<int>(0, 10, 6);

// Generate random int between 0 and 10, omitting 3 through 8
int val = rng.get<int>(0, 10, { 3, 4, 5, 6, 7, 8 });
@endcode

#### Accessing the Engine Adapter

@ref ga::Random::getEngine returns a lightweight adapter that satisfies the uniform random bit generator interface. It does not return a copy of the concrete engine; the adapter refers to the engine owned by the @ref ga::Random object. Keep that object alive while using the adapter.

This is useful with standard-library algorithms that accept a uniform random bit generator:

@code
ga::Random rng { std::mt19937 { 123 } };
auto engine = rng.getEngine();
std::ranges::shuffle(sequence, engine);
@endcode

#### Using a Custom Engine

Any engine that satisfies `std::uniform_random_bit_generator` can be used as with @ref ga::Random.

Because the @ref ga::GeneticAlgorithm initializes and owns the @ref ga::Random copy used throughout the algorithm, a custom engine must be passed through the @ref ga::GeneticAlgorithm constructor's `randomEngine` argument. A supplied engine is forwarded to the @ref ga::Random constructor.

For example, initializing @ref ga::GeneticAlgorithm with a seeded engine to make runs reproducible:

@code
// Define evalFunc, operators, config
ga::GeneticAlgorithm<Sequence, Fitness> ga {
    evalFunc,
    operators,
    config,
    std::mt19937 { 123 }
};
@endcode

The @ref ga::Random constructor takes its engine by value, so the wrapper owns its own engine instance.

#### Default Engine

@ref ga::Random::generateMtEngine creates a seeded `std::mt19937` engine. The @ref ga::GeneticAlgorithm constructor uses this generator by default when no `randomEngine` argument is provided.
