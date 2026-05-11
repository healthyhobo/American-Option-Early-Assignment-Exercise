#include "BinomialPricer.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace
{
void validate_inputs(const OptionContract& option, const MarketData& market, int steps)
{
    if (steps <= 0) {
        throw std::invalid_argument("steps must be positive");
    }
    if (option.strike <= 0.0 || option.maturity_years <= 0.0) {
        throw std::invalid_argument("strike and maturity must be positive");
    }
    if (market.spot <= 0.0 || market.volatility <= 0.0) {
        throw std::invalid_argument("spot and volatility must be positive");
    }
}
}

double payoff(OptionType type, double stock_price, double strike)
{
    if (type == OptionType::Call) {
        return std::max(stock_price - strike, 0.0);
    }
    return std::max(strike - stock_price, 0.0);
}

std::vector<double> stock_prices_at_step(double spot,
                                         double volatility,
                                         double maturity_years,
                                         int steps,
                                         int step_index)
{
    if (steps <= 0 || step_index < 0 || step_index > steps) {
        throw std::invalid_argument("invalid step inputs");
    }

    const double dt = maturity_years / static_cast<double>(steps);
    const double up = std::exp(volatility * std::sqrt(dt));
    const double down = 1.0 / up;

    std::vector<double> prices;
    prices.reserve(static_cast<std::size_t>(step_index + 1));

    // j = number of up moves. step_index - j = number of down moves.
    for (int j = 0; j <= step_index; ++j) {
        const double price = spot * std::pow(up, j) * std::pow(down, step_index - j);
        prices.push_back(price);
    }
    return prices;
}

PriceResult price_binomial(const OptionContract& option,
                           const MarketData& market,
                           int steps)
{
    validate_inputs(option, market, steps);

    const double dt = option.maturity_years / static_cast<double>(steps);
    const double up = std::exp(market.volatility * std::sqrt(dt));
    const double down = 1.0 / up;
    const double growth = std::exp((market.risk_free_rate - market.dividend_yield) * dt);
    const double risk_neutral_probability = (growth - down) / (up - down);
    const double discount = std::exp(-market.risk_free_rate * dt);

    if (risk_neutral_probability <= 0.0 || risk_neutral_probability >= 1.0) {
        throw std::runtime_error("Risk-neutral probability is outside (0,1). Try more steps or different inputs.");
    }

    // Terminal option values at expiry.
    std::vector<double> values(static_cast<std::size_t>(steps + 1));
    for (int j = 0; j <= steps; ++j) {
        const double stock_price = market.spot * std::pow(up, j) * std::pow(down, steps - j);
        values[static_cast<std::size_t>(j)] = payoff(option.type, stock_price, option.strike);
    }

    int early_exercise_nodes = 0;
    double max_exercise_advantage = 0.0;
    constexpr double tolerance = 1e-10;

    // Backward induction: move from the expiry layer back to today.
    for (int i = steps - 1; i >= 0; --i) {
        for (int j = 0; j <= i; ++j) {
            const double continuation_value = discount *
                (risk_neutral_probability * values[static_cast<std::size_t>(j + 1)] +
                 (1.0 - risk_neutral_probability) * values[static_cast<std::size_t>(j)]);

            double chosen_value = continuation_value;

            if (option.style == ExerciseStyle::American) {
                const double stock_price = market.spot * std::pow(up, j) * std::pow(down, i - j);
                const double exercise_value = payoff(option.type, stock_price, option.strike);
                const double exercise_advantage = exercise_value - continuation_value;

                if (exercise_advantage > tolerance) {
                    chosen_value = exercise_value;
                    ++early_exercise_nodes;
                    max_exercise_advantage = std::max(max_exercise_advantage, exercise_advantage);
                }
            }

            values[static_cast<std::size_t>(j)] = chosen_value;
        }
    }

    return PriceResult{values.front(), early_exercise_nodes, max_exercise_advantage};
}
