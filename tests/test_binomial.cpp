#include "BinomialPricer.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

namespace
{
bool approximately_equal(double a, double b, double tolerance)
{
    return std::fabs(a - b) <= tolerance;
}
}

int main()
{
    const int steps = 700;
    const MarketData no_dividend_market{100.0, 0.05, 0.00, 0.20};
    const MarketData high_dividend_market{100.0, 0.05, 0.12, 0.20};

    // Test 1: payoff mechanics.
    assert(approximately_equal(payoff(OptionType::Call, 120.0, 100.0), 20.0, 1e-12));
    assert(approximately_equal(payoff(OptionType::Call, 80.0, 100.0), 0.0, 1e-12));
    assert(approximately_equal(payoff(OptionType::Put, 80.0, 100.0), 20.0, 1e-12));
    assert(approximately_equal(payoff(OptionType::Put, 120.0, 100.0), 0.0, 1e-12));

    // Test 2: A non-dividend-paying American call should not be exercised early.
    const OptionContract european_call{OptionType::Call, ExerciseStyle::European, 100.0, 1.0};
    const OptionContract american_call{OptionType::Call, ExerciseStyle::American, 100.0, 1.0};
    const PriceResult euro_call = price_binomial(european_call, no_dividend_market, steps);
    const PriceResult amer_call = price_binomial(american_call, no_dividend_market, steps);
    assert(amer_call.early_exercise_nodes == 0);
    assert(approximately_equal(amer_call.price, euro_call.price, 1e-8));

    // Test 3: A non-dividend-paying American put can be worth more than the European put.
    const OptionContract european_put{OptionType::Put, ExerciseStyle::European, 100.0, 1.0};
    const OptionContract american_put{OptionType::Put, ExerciseStyle::American, 100.0, 1.0};
    const PriceResult euro_put = price_binomial(european_put, no_dividend_market, steps);
    const PriceResult amer_put = price_binomial(american_put, no_dividend_market, steps);
    assert(amer_put.price > euro_put.price);
    assert(amer_put.early_exercise_nodes > 0);
    assert(amer_put.max_exercise_advantage > 0.0);

    // Test 4: With a high continuous dividend yield, American calls can be exercised early.
    const PriceResult dividend_call = price_binomial(american_call, high_dividend_market, steps);
    assert(dividend_call.early_exercise_nodes > 0);
    assert(dividend_call.max_exercise_advantage > 0.0);

    // Test 5: A tiny tree sanity check: step 0 should only contain the spot price.
    const auto root_prices = stock_prices_at_step(100.0, 0.20, 1.0, 3, 0);
    assert(root_prices.size() == 1);
    assert(approximately_equal(root_prices.front(), 100.0, 1e-12));

    std::cout << "All tests passed.\n";
    return 0;
}
