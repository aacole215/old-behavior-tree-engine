# Behavior Tree Engine (C++)

An old, minimal behavior tree implementation written in C++ to model scalable NPC decision-making systems.

## Overview

This project implements:

- Composite nodes (Selector, Sequence)
- Leaf nodes (Condition, Action)
- Stateful execution with Running status
- Shared blackboard for cross-node state management
- Multi-tick simulation loop

The goal was to explore behavior tree architecture as an alternative to nested conditional AI logic.

## Design Notes

- Composite nodes track `activeChild` to preserve execution state across ticks.
- `Status::Running` allows actions (e.g., chasing) to span multiple updates.
- Blackboard provides shared mutable state between nodes.
- Smart pointers (`std::shared_ptr`) are used for safe ownership of node graphs.

Build

```bash
g++ -std=c++17 main.cpp -o bt
./bt
