# Random Guide {#random}

#### What is @ref ga::Random "ga::Random"?

@ref ga::Random is the library's common interface for random number generation. It accepts any engine that satisfies `std::uniform_random_bit_generator` and stores it behind a type-erased adapter. This lets library components use the same non-templated random interface without depending on the concrete engine type.

When constructed, @ref ga::Random takes ownership of the supplied engine. Internally, it adapts the engine to a uniform 32-bit generator; callers can use @ref ga::Random::getInteger, @ref ga::Random::getIntegerExclude, and @ref ga::Random::getReal to draw values.

#### Generating Values

- @ref ga::Random::getInteger "ga::Random::getInteger()": Generates an integer in the inclusive range `[min, max]`. Supports integral types and throws `std::invalid_argument` if `min > max`.
- @ref ga::Random::getIntegerExclude "ga::Random::getIntegerExclude()": Generates an integer in the inclusive range `[min, max]`, excluding either one value or a `std::vector` of values. Values outside the range are ignored for the vector overload. Throws `std::invalid_argument` if the bounds are invalid or no permitted value remains.
- @ref ga::Random::getReal "ga::Random::getReal()": Generates a floating-point value in the range `[min, max)`. Supports floating-point types and throws `std::invalid_argument` if `min > max`.

#### Accessing the Engine Adapter

@ref ga::Random::getEngine returns a lightweight adapter that satisfies the uniform random bit generator interface. It does not return a copy of the concrete engine; the adapter refers to the engine owned by the @ref ga::Random object. Keep that object alive while using the adapter.

This is useful with standard-library algorithms that accept a uniform random bit generator:

@code
ga::Random random { std::mt19937 { 123 } };
auto engine = random.getEngine();
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
