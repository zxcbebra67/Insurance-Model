#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

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
        return double(max_compensation_) / (double(duration_) / (double(fee_period_) * double(fee_))) - 1.0;
    }
protected:
    int fee_;
    int fee_period_;
    int duration_;
    int max_compensation_;
    int franchise_;
};

class Life : public Insurance {

};

class Estate : public Insurance {

};

class Vehicle : public Insurance {

};