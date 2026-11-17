# Callback Guide {#callbacks}

#### What is a Callback?

A callback is an optional callable invoked by @ref ga::GeneticAlgorithm::run "ga::GeneticAlgorithm::run()" at a configured generation interval. Its purpose is to expose the current state of the search while the algorithm is running and to let user code capture execution data for later inspection.

The library passes read-only execution state through @ref ga::context::CallbackContext "ga::context::CallbackContext". That context contains the current population, the best chromosome found so far, and the current generation number.

#### Defining a Callback

A callback must have the following interface:

@code
void callback(ga::context::CallbackContext<Sequence, Fitness> context);
@endcode

It must satisfy @ref ga::constraints::CallbackFunction.

#### Using a Callback

Assign the callback to the `callback` field of a @ref ga::RunOptions object, then pass that options object into @ref ga::GeneticAlgorithm::run().

The invocation frequency is controlled by the `callbackInterval` field in @ref ga::Config. A value of `0` disables callbacks. The callback is invoked once every `callbackInterval` generations.

The @ref ga::RunOptions object stores the callback by reference. Deleting or invalidating the original callback while the run is in progress results in undefined behavior.
