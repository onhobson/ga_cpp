# Random Guide {#random}

#### What is @ref ga::Random "ga::Random"?

@ref ga::Random is the library's common interface for random number generation. It uses a `std::mt19937` as the pseudo-random number engine.

By default, @ref ga::Random seeds its `std::mt19937` instance with a `std::seed_seq` constructed from a 256-bit sequence derived from `std::random_device` calls and clock time.

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

#### Accessing the Engine

@ref ga::Random::getEngine returns a reference to the engine used by the @ref ga::Random object. 

This is useful with standard-library algorithms that accept a uniform random bit generator:

@code
ga::Random rng { std::mt19937 { 123 } };
auto engine = rng.getEngine();
std::ranges::shuffle(sequence, engine);
@endcode
