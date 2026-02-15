# Behavior Tree Engine 

Very minimal behavior tree implementation written in C++ for the intent of modeling scalable
NPC decision-making systems.

## Overview

This project has:
-   Composite nodes (Selector, Sequence)
-   Leaf nodes (Condition, Action)
-   Stateful execution with Running status
-   Shared blackboard for cross-node state management
-   Multi-tick simulation loop

The goal here was to explore behavior trees as a viable alternative to
nested conditional AI logic.

## Design Notes

-   Composite nodes track `activeChild` to preserve execution state
    across ticks.
-   `Status::Running` allows actions (like chasing) to persist across multiple
    updates.
-   Blackboard provides shared mutable state between nodes.
-   Smart pointers (std::shared_ptr) are used for safe ownership of
    any node graphs.

## Build

```bash
g++ -std=c++17 main.cpp -o bt
./bt
```

## Example Behavior

The simulation demonstrates:

-   NPC chasing over multiple ticks
-   Transition from chase to attack once in range
-   Idle fallback behavior

