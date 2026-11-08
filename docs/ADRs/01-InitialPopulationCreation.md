# ADR 01: Initial Population Creation

## Context

Initial population creation is currently tied to the GA's shuffling logic and split between the GA and `Populator`. This limits customization and can exclude sequence types that do not support the assumptions made by the current shuffle implementation. Initialization should be customizable, independent of GA internals, and reproducible through the GA's random number generator.

## Decision

Provide an initialization function to `run()` alongside the base sequence. The GA will invoke it as part of the run, supplying access to its random number generator (and configuration where needed). Provide a default shuffle initializer to preserve convenient behavior for common sequence types.

## Consequences

- Users can select a predefined strategy or supply a custom initializer.
- The GA retains control of initialization and RNG access, supporting reproducible runs.
- Initialization is specified at the point it is used, avoiding a more crowded GA constructor.
- The initializer callable interface must be defined, and the GA must integrate it into `run()`.
- A default shuffle initializer preserves the familiar behavior, but custom strategies can support other sequence types.

## Alternatives considered

- **Create the population externally:** Fully decouples initialization and makes it easy to inspect or modify the population before running, but adds user work and makes use of the GA's RNG for reproducibility difficult.
- **Pass an initializer function to the GA:** Like the chosen approach, this lets the GA manage initialization and provide RNG access; it can also expose configuration and a default shuffle strategy. However, it adds initialization to the constructor and leaves the GA responsible for integrating the callable. Passing the function to `run()` offers these same benefits while keeping it closer to where initialization is used.
