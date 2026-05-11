#include "BinomialPricer.hpp"

#include <iomanip>
#include <iostream>
#include <vector>

void print_scenario(const std::string& label,
                    const OptionContract& option,
                    const MarketData& market,
                    int steps)
{
    const PriceResult result = price_binomial(option, market, steps);

    std::cout << std::left << std::setw(46) << label
              << " price=" << std::setw(10) << std::fixed << std::setprecision(4) << result.price
              << " earlyExerciseNodes=" << std::setw(6) << result.early_exercise_nodes
              << " maxExerciseAdvantage=" << result.max_exercise_advantage
              << '\n';
}

int main()
{
    const int steps = 500;

    const MarketData no_dividend_market{100.0, 0.05, 0.00, 0.20};
    const MarketData high_dividend_market{100.0, 0.05, 0.12, 0.20};

    const OptionContract european_call{OptionType::Call, ExerciseStyle::European, 100.0, 1.0};
    const OptionContract american_call{OptionType::Call, ExerciseStyle::American, 100.0, 1.0};
    const OptionContract european_put{OptionType::Put, ExerciseStyle::European, 100.0, 1.0};
    const OptionContract american_put{OptionType::Put, ExerciseStyle::American, 100.0, 1.0};

    std::cout << "Binomial early-exercise lab\n";
    std::cout << "Steps: " << steps << "\n\n";

    print_scenario("European call, no dividend", european_call, no_dividend_market, steps);
    print_scenario("American call, no dividend", american_call, no_dividend_market, steps);
    print_scenario("European put, no dividend", european_put, no_dividend_market, steps);
    print_scenario("American put, no dividend", american_put, no_dividend_market, steps);
    print_scenario("American call, high dividend yield", american_call, high_dividend_market, steps);
    print_scenario("American put, high dividend yield", american_put, high_dividend_market, steps);

    std::cout << "\nExpected qualitative result:\n";
    std::cout << "1. No-dividend American call should match European call and show zero early exercise nodes.\n";
    std::cout << "2. No-dividend American put can be more valuable than European put and can show early exercise nodes.\n";
    std::cout << "3. With a high dividend yield, American calls may show early exercise nodes.\n";

    return 0;
}
