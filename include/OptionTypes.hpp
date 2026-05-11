#pragma once

#include <string>

// This header contains the small vocabulary of the lab.
// Keeping these simple makes the rest of the code easier to read.

enum class OptionType
{
    Call,
    Put
};

enum class ExerciseStyle
{
    European,
    American
};

struct OptionContract
{
    OptionType type{};
    ExerciseStyle style{};
    double strike{};
    double maturity_years{};
};

struct MarketData
{
    double spot{};            // Current stock price S0
    double risk_free_rate{};  // Continuously compounded annual rate r
    double dividend_yield{};  // Continuous dividend yield q
    double volatility{};      // Annual volatility sigma
};

struct PriceResult
{
    double price{};
    int early_exercise_nodes{};          // How many tree nodes chose exercise over continuation
    double max_exercise_advantage{};     // Largest intrinsic - continuation value found
};

inline std::string to_string(OptionType type)
{
    return type == OptionType::Call ? "Call" : "Put";
}

inline std::string to_string(ExerciseStyle style)
{
    return style == ExerciseStyle::American ? "American" : "European";
}
