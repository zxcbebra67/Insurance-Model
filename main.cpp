#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "CustomersManager.h"
#include "Insurance.cpp"

using namespace std;

int main() {
	gen.seed(rd());
	long long Capital = 10000;
	Life life(1000, 12, 24, 5000, 500);
	Estate estate(1000, 12, 24, 5000, 500);
	Vehicle vehicle(1000, 12, 24, 5000, 500);
	int M = 5;
	int order = 1000;
	double probability = 0.3;
	CustomerManager manager_life;
	CustomerManager manager_estate;
	CustomerManager manager_vehicle;
	for(int i = 0; i < 1; i++){
		Capital *= 0.91;
		int n = life.new_customers(order);
		life.add_customers(n);
    	manager_life.AddCustomer(life, n);
		cout << Capital << '\n';
		manager_life.MoneyLender(Capital, life);
		cout << Capital << '\n';
		manager_life.InsuredLoss(Capital, probability);
		/* estate.new_customers(gen, order, manager_estate);
		vehicle.new_customers(gen, order, manager_vehicle);
		manager_life.MoneyLender(Capital, life); */
		cout << Capital << '\n';
	}
}