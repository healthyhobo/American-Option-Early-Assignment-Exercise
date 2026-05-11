#pragma once

#include "OptionTypes.hpp"

#include <vector>

// Returns max(S-K,0) for calls and max(K-S,0) for puts.
double payoff(OptionType type, double stock_price, double strike);

// Price a European or American option with a Cox-Ross-Rubinstein binomial tree.
// The function also reports where early exercise was optimal.
PriceResult price_binomial(const OptionContract& option,
                           const MarketData& market,
                           int steps);

// A tiny helper used by the walkthrough and tests.
// It constructs the stock prices at a given time step i in the CRR tree.
std::vector<double> stock_prices_at_step(double spot,
                                         double volatility,
                                         double maturity_years,
                                         int steps,
                                         int step_index);
