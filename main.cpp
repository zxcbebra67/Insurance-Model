#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "CustomersManager.h"
#include "Insurance.cpp"

using namespace std;

int main() {
	std::default_random_engine gen;
	std::random_device rd;
	gen.seed(rd());
	double Capital = 10000;
	Life life(1000, 12, 24, 5000, 500);
	Estate estate(1000, 12, 24, 5000, 500);
	Vehicle vehicle(1000, 12, 24, 5000, 500);
	int M = 1;
	int order = 100;
	CustomerManager manager;
	for(int i = 0; i < M; i++){
		Capital *= 0.91;
		life.new_customers(gen, order, manager);
	}
	cout << life.get_customers();
}