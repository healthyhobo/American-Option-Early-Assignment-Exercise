# C++ Lab: Early Exercise of American Options

This lab turns the Hull Chapter 10 idea into a small C++ project:

- A non-dividend-paying American call should **not** be exercised early.
- A non-dividend-paying American put **can** be exercised early.
- When the stock pays dividends, early exercise can become optimal for calls and puts depending on parameters.

The lab uses a **Cox-Ross-Rubinstein binomial tree**, because the tree lets you inspect the decision at every node:

```text
American option value = max(exercise now, hold/continuation value)
European option value = hold/continuation value only
```

In this project, dividends are represented as a **continuous dividend yield `q`**. That is simpler than modelling a specific ex-dividend date, but it still demonstrates the key economic idea: dividends reduce the value of waiting to exercise calls because the call holder does not receive dividends before exercising.

---

## Project layout

```text
american_option_early_exercise_lab/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── BinomialPricer.hpp
│   └── OptionTypes.hpp
├── src/
│   ├── BinomialPricer.cpp
│   └── main.cpp
└── tests/
    └── test_binomial.cpp
```

---

## Prerequisites

You need:

- A C++17 compiler
- CMake 3.16 or newer

Examples:

### macOS

```bash
xcode-select --install
brew install cmake
```

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake
```

### Windows

Install Visual Studio with the **Desktop development with C++** workload, or use MSYS2/MinGW plus CMake.

---

## How to build and run

From the project folder:

```bash
cmake -S . -B build
cmake --build build
```

Run the demo:

```bash
./build/option_lab
```

On Windows, the executable may be here instead:

```bash
.\build\Debug\option_lab.exe
```

Run the tests:

```bash
ctest --test-dir build --output-on-failure
```

Or run the test executable directly:

```bash
./build/test_binomial
```

---

## The core option theory

### 1. Call option payoff

A call gives the right to buy the stock at strike `K`.

```text
call payoff = max(S - K, 0)
```

For a non-dividend-paying stock, exercising the call early is usually wasteful because:

1. You pay the strike `K` earlier than necessary.
2. You give up the remaining time value of the option.
3. You do not gain any dividend by owning the stock early, because there are no dividends.

Therefore, in the binomial tree, the continuation value should always be at least as good as immediate exercise for a non-dividend-paying American call.

### 2. Put option payoff

A put gives the right to sell the stock at strike `K`.

```text
put payoff = max(K - S, 0)
```

For a deep in-the-money put, early exercise can be rational. If the stock is very low, exercising gives you cash now by selling at `K`. Receiving cash earlier is valuable when interest rates are positive.

So for puts, the tree may find nodes where:

```text
exercise value > continuation value
```

### 3. Dividends

A call holder does not receive dividends until they exercise and become a stockholder. If the dividend benefit is large enough, early call exercise can become optimal.

This lab models dividends using a continuous dividend yield `q`. In the risk-neutral stock process, this changes the expected growth rate from `r` to `r - q`.

---

## The binomial tree algorithm

For each time step:

```text
dt = T / steps
u = exp(sigma * sqrt(dt))
d = 1 / u
p = (exp((r - q) * dt) - d) / (u - d)
discount = exp(-r * dt)
```

At expiry:

```text
option_value = payoff
```

Then walk backward through the tree:

```text
continuation = discount * (p * up_value + (1 - p) * down_value)
```

For a European option:

```text
value = continuation
```

For an American option:

```text
value = max(continuation, exercise_value)
```

The project counts how many nodes choose early exercise.

---

## Lab walkthrough

### Step 1: Read the data structures

Open `include/OptionTypes.hpp`.

Focus on these ideas:

- `enum class OptionType` restricts the option type to `Call` or `Put`.
- `enum class ExerciseStyle` restricts the exercise style to `European` or `American`.
- `struct OptionContract` groups contract-specific fields.
- `struct MarketData` groups market inputs.
- `struct PriceResult` returns more than just the price.

C++ concept practiced here: **strongly typed enums** and **plain structs**.

---

### Step 2: Understand the payoff function

Open `src/BinomialPricer.cpp` and find:

```cpp
double payoff(OptionType type, double stock_price, double strike)
```

This function is intentionally simple. It is a good first place to modify and test code.

Try temporarily breaking it, for example by returning `stock_price - strike` for calls without `max`. Then run the tests and see what fails.

C++ concepts practiced here:

- Functions
- `if` statements
- `std::max`
- Header declarations vs. source definitions

---

### Step 3: Understand stock tree construction

Find:

```cpp
std::vector<double> stock_prices_at_step(...)
```

This function creates all stock prices at one level of the tree.

At step `i`, node `j` has:

```text
j up moves
(i - j) down moves
```

So the stock price is:

```text
S0 * u^j * d^(i-j)
```

C++ concepts practiced here:

- `std::vector`
- `reserve`
- loops
- `std::pow`
- `static_cast`

---

### Step 4: Understand backward induction

Find:

```cpp
PriceResult price_binomial(...)
```

This is the core of the lab.

First, terminal option values are computed at expiry:

```cpp
for (int j = 0; j <= steps; ++j) {
    const double stock_price = ...;
    values[j] = payoff(...);
}
```

Then the tree walks backward:

```cpp
for (int i = steps - 1; i >= 0; --i) {
    for (int j = 0; j <= i; ++j) {
        ...
    }
}
```

Inside that loop, the code compares:

```text
continuation value vs. exercise value
```

C++ concepts practiced here:

- Nested loops
- Local constants with `const`
- Floating-point tolerance
- Reusing one vector for memory efficiency
- Throwing exceptions for invalid inputs

---

### Step 5: Run the qualitative experiment

Run:

```bash
./build/option_lab
```

You should see output similar to this:

```text
European call, no dividend                    price=10.44... earlyExerciseNodes=0
American call, no dividend                    price=10.44... earlyExerciseNodes=0
European put, no dividend                     price=5.57...  earlyExerciseNodes=0
American put, no dividend                     price=6.08...  earlyExerciseNodes=>0
American call, high dividend yield            price=...      earlyExerciseNodes=>0
```

The exact prices depend on the number of tree steps.

---

## Exercises

### Exercise A: Change moneyness

In `src/main.cpp`, change the strike from `100` to:

- `80`
- `100`
- `120`

Observe how early exercise changes for puts and calls.

Questions:

1. When is the put most likely to be exercised early?
2. Why does a deep in-the-money put behave differently from an at-the-money put?
3. Does the non-dividend-paying call ever show early exercise nodes?

---

### Exercise B: Change interest rates

In `src/main.cpp`, change:

```cpp
const MarketData no_dividend_market{100.0, 0.05, 0.00, 0.20};
```

to use these rates:

```text
r = 0.00
r = 0.01
r = 0.05
r = 0.10
```

Questions:

1. What happens to early exercise of puts as rates increase?
2. Why does receiving `K` earlier become more attractive when rates are higher?

---

### Exercise C: Change dividend yield

In `src/main.cpp`, change:

```cpp
const MarketData high_dividend_market{100.0, 0.05, 0.12, 0.20};
```

to use these dividend yields:

```text
q = 0.00
q = 0.02
q = 0.05
q = 0.12
```

Questions:

1. At what dividend yield do you first see early exercise nodes for calls?
2. Why does a higher dividend yield make waiting less attractive for call holders?

---

### Exercise D: Increase tree steps

Change:

```cpp
const int steps = 500;
```

to:

```text
50
100
500
1000
```

Questions:

1. Do prices stabilize as the number of steps increases?
2. Does the early exercise node count increase simply because there are more nodes?
3. Is node count alone a perfect measure of exercise likelihood?

Important: the count of early exercise nodes is useful for diagnostics, but it is not the same as real-world exercise probability.

---

### Exercise E: Add a trace function

Challenge: add a function that prints the first few early exercise nodes.

Suggested output:

```text
timeStep=123 node=4 stock=52.31 exercise=47.69 continuation=47.51 advantage=0.18
```

This teaches you how to debug numerical models and inspect the exercise boundary.

---

## Required tests

The project includes tests in `tests/test_binomial.cpp`.

They verify:

1. Call and put payoffs are correct.
2. A non-dividend-paying American call has zero early exercise nodes.
3. A non-dividend-paying American call has the same value as the European call.
4. A non-dividend-paying American put is more valuable than the European put.
5. A non-dividend-paying American put has early exercise nodes.
6. A high-dividend-yield American call has early exercise nodes.
7. The stock tree root is equal to the spot price.

Run them with:

```bash
ctest --test-dir build --output-on-failure
```

Expected result:

```text
100% tests passed
```

---

## C++ learning checklist

By completing this lab, you practice:

- `enum class`
- `struct`
- header files and source files
- include guards via `#pragma once`
- functions with typed inputs and outputs
- `std::vector`
- loops
- exceptions
- floating-point comparisons
- `const` correctness
- basic CMake
- simple tests with `assert`

---

## Finance learning checklist

By completing this lab, you should understand:

- European vs. American exercise rights
- intrinsic value vs. continuation value
- why non-dividend American calls are not exercised early
- why American puts can be exercised early
- how dividends can create early-exercise incentives for calls
- how a binomial model prices options by backward induction
- why an American option is at least as valuable as the matching European option

---

## Suggested extensions

Once the base lab works, extend it in this order:

1. Add Black-Scholes European call and put formulas for comparison.
2. Add put-call parity checks for European options.
3. Add a CSV output of exercise boundaries.
4. Plot exercise boundaries in Python.
5. Add discrete cash dividends instead of continuous dividend yield.
6. Add command-line arguments for spot, strike, rate, volatility, maturity, and steps.
7. Add a small Monte Carlo simulation to show why Monte Carlo is less natural for early exercise than trees.

