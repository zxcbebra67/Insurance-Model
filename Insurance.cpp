#include "Insurance.h"
#include "CustomersManager.h"

int Insurance::new_customers(int order) {
    double d = demand();
    double baseProbability = probabilityFromDemand(d);
    std::uniform_real_distribution<double> noise(-0.5, 0.5);
    double p = max(0.0, baseProbability + noise(gen));
    cout << p << '\n';
    int n = p * order;
    return n;
}
