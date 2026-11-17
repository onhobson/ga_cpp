# Termination Guide {#termination}

#### What is a Terminator?

A terminator is a callable invoked every generation to decide whether the genetic algorithm should stop. It must satisfy @ref ga::constraints::TerminationCondition and accept a @ref ga::context::TerminationContext "TerminationContext". It returns `bool`: `false` continues the run and `true` signals termination.

A single terminator may represent one stop condition or a combination of many. Terminators can be combined into aggregate checks such as @ref ga::terminate::AnyOf and @ref ga::terminate::AllOf.

#### What is a @ref ga::terminate::TerminationPolicy "TerminationPolicy"?

A @ref ga::terminate::TerminationPolicy is the object that contains the termination conditions. It is passed through the `terminator` field of @ref ga::RunOptions.

A policy has two pieces of state:
- a `generationLimit`, which acts as a hard cap on the run loop
- a `condition`, which may be a single terminator or an aggregate of several terminators

The library requires termination logic to be passed through a termination policy so the algorithm cannot continue indefinitely.

#### Using a @ref ga::terminate::TerminationPolicy "TerminationPolicy"

A @ref ga::terminate::TerminationPolicy is supplied to @ref ga::GeneticAlgorithm::run "ga::GeneticAlgorithm::run()" through a @ref ga::RunOptions object.

Assign the created policy to the `terminator` field of @ref ga::RunOptions. If no custom policy is defined, the algorithm defaults to a generation limit of 100.

### Built-in Terminators

Aggregates:
- @ref ga::terminate::AnyOf
- @ref ga::terminate::AllOf

Compositions:
- @ref ga::terminate::GenerationLimit
- @ref ga::terminate::StagnationLimit
- @ref ga::terminate::FitnessThreshold

Other:
- @ref ga::terminate::NoOpTerminator

`ga::terminate::NoOpTerminator` is the default used by a @ref ga::terminate::TerminationPolicy when no termination condition is specified. It does not effect termination behavior on its own. This allows a simple @ref ga::terminate::TerminationPolicy to terminate solely using a `generationLimit`.

### Custom Terminators

Define a callable with the following interface:

@code
bool terminator(const ga::context::TerminationContext<Fitness>& context);
@endcode

The callable must satisfy @ref ga::constraints::TerminationCondition. Returing `true` signals termination.

Assign the custom terminator to the `condition` field of @ref ga::terminate::TerminationPolicy, or use it as a sub-condition inside an aggregate such as @ref ga::terminate::AnyOf or @ref ga::terminate::AllOf.

Terminators may hold state, but they are copied by value into @ref ga::RunOptions. As a result, they should not be used to track execution. Use a @ref callbacks "callback" for tracked execution data instead.
