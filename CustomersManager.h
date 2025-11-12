#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <random>
#include <cmath>
#include <map>
#include "Insurance.h"

/*тут находится структура в которой хранятся данные о покупателях. Сама структура представляет собой список,
узлами которого является следующий набор данных: условия страховки, количество пользователей страховки и сколько
полных месяцев прошло с начала договора. Каждый новый месяц добавляется новый узел (так как приходит новый поток клиентов,
возможно на новых условиях)*/

using namespace std;

class Customer {
public:
    Customer(Insurance cond, int num) {
        conditions = cond;
        number = num;
        next = nullptr;
        month = -1;
    }

    Insurance conditions; //страховка передается не по ссылке, чтобы при изменении условий страховки старые клиенты оставались на старых условиях.
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

    void UpdateInsurance(Customer* cust, Insurance& insurance){ //клиенты, у которых истёк срок действия договора встают снова в начало очереди (продлевают договор и встают на нулевой месяц), при этом часть из них может отказаться от продления
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
                Customer* p = cur;
                cur = cur->next;
                UpdateInsurance(p, insurance);
            }
            else{
                cur = cur->next;
            };
        }
    }

    void InsuredLoss(long long& capital, double p_max ){
        std::uniform_real_distribution<double> probDist(0.0, p_max);
        double p = probDist(gen);
        Customer* cur = head;
        while (cur) {
            std::binomial_distribution<int> claimDist(cur->number, p);
            int claims = claimDist(gen);
            capital -= CalculateLoss(claims, cur);
            cur = cur->next;
        }
    }

    int CalculateLoss(int n, Customer* cur){
        int loss = 0;
        for(int i = 0; i < n; i++){
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            double x = dist(gen);
            double y;
            if(x < 0.5){
                std::uniform_real_distribution<double> dist1(0.0, 0.15);
                y = dist1(gen);
            }
            else if(x < 0.9){
                std::uniform_real_distribution<double> dist1(0.15, 0.5);
                y = dist1(gen);
            }
            else {
                std::uniform_real_distribution<double> dist1(0.5, 1.0);
                y = dist1(gen);
            }
            loss += cur->conditions.get_max_compenstation() * y;
        }
        return loss;
    }

private:
    Customer* head;
    Customer* tail;
};
