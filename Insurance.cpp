#include "Insurance.h"
#include "CustomersManager.h"

double probabilityFromDemand(double d) {
    if (d <= 1.0) return 0.0;

    const double k = 1.2;
    const double c = 3.0;

    auto logistic = [&](double x) {
        return 1.0 / (1.0 + std::exp(-k * (x - c)));
    };

    double minVal = logistic(1.0);
    double p = (logistic(d) - minVal) / (1.0 - minVal);
    return std::clamp(p, 0.0, 1.0);
}

int Insurance::new_customers(int order) {
    double d = demand();
    double baseProbability = probabilityFromDemand(d);
    std::uniform_real_distribution<double> noise(-0.1, 0.1);
    double randomFactor = 1.0 + noise(gen);

    double probability = std::clamp(baseProbability * randomFactor, 0.0, 1.0);

    std::binomial_distribution<int> binom(order, probability);
    int n = binom(gen);
    return n;
}
