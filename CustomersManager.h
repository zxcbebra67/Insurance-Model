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
    Customer(Insurance cond, int num) {
        conditions = cond;
        number = num;
        next = nullptr;
        month = -1;
    }

    Insurance conditions;
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

    void AddCustomer(Insurance cond, int num) {
        Customer* newCustomer = new Customer(cond, num);
        if (head == nullptr) {
            head = tail = newCustomer;
        } else {
            tail->next = newCustomer;
            tail = newCustomer;
        }
    }

    void PrintQueue() {
        Customer* cur = head;
        while (cur) {
            cout << "Customer month = " << cur->month << endl;
            cur = cur->next;
        }
        cout << '\n';
    }

    void UpdateInsurance(Customer* cust, Insurance& insurance){
        if(head == cust){
            head = head->next;
        }
        else{
            Customer* cur = head;
            while (cur->next != cust) {
                cur = cur->next;
            }
            cur->next = cust->next;
        }
        tail->next = cust;
        tail = cust;
        cust->next = nullptr;
        cust->conditions = insurance;
        int old_customers = cust->number;
        insurance.add_customers(-old_customers);
        int n = insurance.new_customers(old_customers);
        cust->number = n;
        insurance.add_customers(n);
        cust->month = -1;
    };

    void MoneyLender(long long& capital, Insurance& insurance) {
        Customer* cur = head;
        while (cur) {
            cur->month++;
            if(cur->month % cur->conditions.get_fee_period_() == 0){
                capital+=(cur->conditions.get_fee() * cur->number);
            }
            if(cur->month >= cur->conditions.get_duration()){
                UpdateInsurance(cur, insurance);
            };
            cur = cur->next;
        }
    }

private:
    Customer* head;
    Customer* tail;
};
