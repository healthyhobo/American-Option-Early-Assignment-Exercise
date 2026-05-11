# Worksheet: Early Exercise Intuition

Use this worksheet while running the lab.

## Scenario 1: Non-dividend American call

Market:

```text
S0 = 100
K = 100
r = 5%
q = 0%
sigma = 20%
T = 1 year
```

Record:

```text
European call price:
American call price:
American call early exercise nodes:
```

Question: Why should the American call not be worth more than the European call here?

Answer:


## Scenario 2: Non-dividend American put

Record:

```text
European put price:
American put price:
American put early exercise nodes:
```

Question: Why can early exercise add value for a put?

Answer:


## Scenario 3: High-dividend American call

Market:

```text
S0 = 100
K = 100
r = 5%
q = 12%
sigma = 20%
T = 1 year
```

Record:

```text
American call price:
American call early exercise nodes:
Maximum exercise advantage:
```

Question: Why do dividends change the call exercise decision?

Answer:


## C++ reflection

1. What does `enum class` protect you from?


2. Why does `price_binomial` return a `PriceResult` struct instead of only a `double`?


3. Why does the implementation use one vector and overwrite values during backward induction?


4. Why do tests use approximate equality instead of exact equality for `double` values?

