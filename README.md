# CDBoost Experiments

Reproduction of the tick-counter study from VDW14 using the CDBoost C++11 PDEVS simulator.

## What this repository is about

CDBoost implements the sequential PDEVS architecture (VNWD15): a single-threaded
call/return simulation loop with a call stack linear in the model hierarchy depth.
These experiments use CDBoost to demonstrate how the choice of TIME type
(floating-point vs. exact rational) affects the correctness of a coupled simulation.

The reference experiment is the tick-counter from:

> Vicino, Dalle, Wainer. *A Data Type for Discretized Time Representation in DEVS.*
> SIMUTOOLS 2014. hal-01055555.

## Structure

```
cdboost-impl-notes.tex   Shared implementation notes (simulator conventions,
                         MSG type, TIME type, log format) included in the paper.
docs/
  main.tex               Root LaTeX document — compiles to main.pdf.
  main.pdf               Built paper.
vdw14/
  spec.tex               Model definition and expected observations for the
                         tick-counter experiment under PDEVS.
  main.cpp               Simulation driver (double and rational TIME runs).
  tick_gen.hpp           Periodic tick generator (period 1/10 s).
  reset_gen.hpp          Periodic reset generator (period 1 s).
  k_counter.hpp          Counter model (wraps infinite_counter).
  test_models.cpp        Unit tests for the atomic models.
  CMakeLists.txt         Build rules for the experiment.
vcpkg.json               vcpkg dependency manifest (Boost).
CMakeLists.txt           Root build configuration.
```

## Building

Requires CMake, vcpkg, and a C++11-capable compiler.

```sh
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
cmake --build build -j2
ctest --test-dir build
```

The simulation executables in `build/vdw14/` write NDJSON logs to stdout.
See `simulators/cdboost/docs/log-format.md` for the log schema.
