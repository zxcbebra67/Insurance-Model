#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <random>
#include <cmath>
#include <algorithm>

using namespace std;

static std::default_random_engine gen;
static std::random_device rd;

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
    double demand() { //функция рассчета спроса, возвращает какое-то число
        return double(max_compensation_) / ((double(duration_) / (double(fee_period_)) * double(fee_)));
    }
    void add_customers(int n){
        customers += n;
    }
    int get_customers(){
        return customers;
    }
    int get_duration(){
        return duration_;
    }
    int get_fee(){
        return fee_;
    }
    int get_fee_period_(){
        return fee_period_;
    }
    int get_max_compenstation(){
        return max_compensation_;
    }
    int get_franchise(){
        return franchise_;
    }
    int new_customers(int order);

    virtual std::string get_type_name() const {
        return "Basic Insurance";
    }
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

    std::string get_type_name() const override {
        return "Life Insurance";
    }
};

class Estate : public Insurance {
public:
    using Insurance::Insurance;

    std::string get_type_name() const override {
        return "Estate Insurance";
    }
};

class Vehicle : public Insurance {
public:
    using Insurance::Insurance;

    std::string get_type_name() const override {
        return "Vehicle Insurance";
    }
};

inline double probabilityFromDemand(double d) {
    if(d <= 4){
        return max(0.0, (d-3)/3);
    }
    else if (d <= 10) return (d-4)/15 + 1.0/3.0;
    else return min((d-10)/20 + 11.0/15.0, 1.0);
}