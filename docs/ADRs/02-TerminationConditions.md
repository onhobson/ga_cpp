# ADR 02: Termination Conditions

## Context

The GA currently runs for the number of generations specified in `Config`. While this provides a straightforward stopping rule, users cannot stop early or prioritize other aspects of the result. Supporting configurable termination conditions makes the GA more useful across different optimization tasks.

## Decision

Add a `TerminationCondition` callable to the `run()` call. It will receive a `TerminationContext` containing GA state information and return whether the GA should terminate.

Provide predefined conditions, including individual conditions and aggregate conditions such as `AnyOf` and `AllOf`, so users can compose stopping rules. Implement the aggregates as variadic templates to allow combining multiple conditions.

Rework `Config::generationCount` as a termination condition, and make a generation-count condition the default. To prevent a GA run from continuing indefinitely when a user supplies an unreachable condition, wrap user-supplied terminators in a `TerminationPolicy` that requires `GenerationLimit` as the outermost condition.

## Consequences

- Users can stop runs based on GA state, including combinations of multiple conditions.
- A generation-count condition remains the default stopping behavior.
- `run()` needs to pass a `TerminationContext` to the condition and honor its result.
- The outermost `GenerationLimit` ensures a run has a finite bound even when its supplied condition is unreachable.
- The `TerminationPolicy`, `TerminationContext`, predefined conditions, and aggregate interfaces must be defined and integrated with GA execution.

## Alternatives considered

- **Specify termination thresholds in `Config`:** The GA or one of its components would need to track the values required by the thresholds. This couples condition tracking to the GA rather than keeping termination logic in a callable, and adds more settings to `Config`.
