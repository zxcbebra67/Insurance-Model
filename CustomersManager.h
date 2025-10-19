#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <random>
#include <cmath>
#include <map>
#include "Insurance.h"

using namespace std;

class Customer {
public:
    Customer(int dur, int num) {
        duration = dur;
        number = num;
        month = dur;
        next = nullptr;
    }

    int duration;
    int number;
    int month;
    Customer* next;
};

class CustomerManager {
public:
    CustomerManager() {
        head = nullptr;
        tail = nullptr;
    }

    void AddCustomer(int dur, int num) {
        Customer* newCustomer = new Customer(dur, num);
        if (head == nullptr) {
            head = tail = newCustomer;
        } else {
            tail->next = newCustomer;
            tail = newCustomer;
        }
    }

private:
    Customer* head;
    Customer* tail;
};
