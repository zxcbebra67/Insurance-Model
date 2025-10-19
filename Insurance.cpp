#include "Insurance.h"
#include "CustomersManager.h"

void Insurance::new_customers(std::default_random_engine& gen, int order, CustomerManager& manager){
        double baseProbability = 1.0 / (1.0 + std::exp(-0.1 * demand()));
        std::normal_distribution<double> dist(1.0, 0.1);
        double randomFactor = dist(gen);
        double probability = baseProbability * randomFactor;
        probability = std::max(0.0, std::min(1.0, probability));
        std::binomial_distribution<int> binom(order, probability);
        int n = binom(gen);
        add_customers(n);
        manager.AddCustomer(duration_, n);
    }