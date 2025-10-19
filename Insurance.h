#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <random>
#include <cmath>

using namespace std;

class CustomerManager;

class Insurance {
public:
    Insurance() = default;
    Insurance(int fee, int fee_period, int duration, int max_compensation, int franchise) {
        fee_ = fee;
        fee_period_ = fee_period;
        duration_ = duration;
        max_compensation_ = max_compensation;
        franchise_ = franchise;
    };
    void Set_Fee(int fee, int fee_period) {
        fee_ = fee;
        fee_period_ = fee_period;
    }
    void Set_Duration(int duration) {
        duration_ = duration;
    }
    void Set_Max_Compensation(int max_compensation) {
        max_compensation_ = max_compensation;
    }
    void Set_Franchise(int franchise) {
        franchise_ = franchise;
    }
    double demand() {
        return double(max_compensation_) / (double(duration_) / (double(fee_period_) * double(fee_)));
    }
    void add_customers(int n){
        customers += n;
    }
    int get_customers(){
        return customers;
    }
    void new_customers(std::default_random_engine& gen, int order, CustomerManager& manager);
protected:
    int fee_;
    int fee_period_;
    int duration_;
    int max_compensation_;
    int franchise_;
    int customers = 0;
};

class Life : public Insurance {
public:
    using Insurance::Insurance;
};

class Estate : public Insurance {
public:
    using Insurance::Insurance;
};

class Vehicle : public Insurance {
public:
    using Insurance::Insurance;
};