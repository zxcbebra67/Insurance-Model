#include "httplib.h"
#include "Insurance.h"
#include "CustomersManager.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

using namespace std;

// Глобальные объекты
CustomerManager ManagerLife;
CustomerManager ManagerEstate;
CustomerManager ManagerVehicle;
vector<shared_ptr<Insurance>> insurance_products;
long long capital = 100000000;
bool capital_set = false;
int current_month = 0;
double probability = 0.05;
int order = 100; // Добавляем переменную order

void initialize_data() {
    auto life_insurance = make_shared<Life>(5000, 1, 12, 1000000, 0);
    auto estate_insurance = make_shared<Estate>(10000, 3, 24, 500000, 50000);
    auto vehicle_insurance = make_shared<Vehicle>(3000, 1, 6, 500000, 10000);
    
    insurance_products.push_back(life_insurance);
    insurance_products.push_back(estate_insurance);
    insurance_products.push_back(vehicle_insurance);
    
    ManagerLife.AddCustomer(*life_insurance, 0);
    ManagerEstate.AddCustomer(*estate_insurance, 0);
    ManagerVehicle.AddCustomer(*vehicle_insurance, 0);
}

int main() {
    initialize_data();
    
    httplib::Server server;
    
    // CORS
    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Credentials", "true"},
    });
    
    server.Options(R"(.*)", [](const httplib::Request& req, httplib::Response& res) {
        res.status = 200;
    });

    // Тест
    auto handle_test = [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"ok\",\"message\":\"Server is working\"}", "application/json");
    };

    // Состояние - ДОБАВЛЯЕМ order в ответ
    auto handle_state = [](const httplib::Request&, httplib::Response& res) {
        try {
            stringstream json;
            json << "{";
            json << "\"capital\":" << capital << ",";
            json << "\"current_month\":" << current_month << ",";
            json << "\"probability\":" << probability << ",";
            json << "\"order\":" << order << ","; // ДОБАВЛЯЕМ order
            json << "\"insurance_products\":[";

            for (size_t i = 0; i < insurance_products.size(); i++) {
                if (i > 0) json << ",";
                json << "{";
                json << "\"id\":" << i << ",";
                json << "\"type\":\"" << insurance_products[i]->get_type_name() << "\",";
                json << "\"fee\":" << insurance_products[i]->get_fee() << ",";
                json << "\"fee_period\":" << insurance_products[i]->get_fee_period_() << ",";
                json << "\"duration\":" << insurance_products[i]->get_duration() << ",";
                json << "\"max_compensation\":" << insurance_products[i]->get_max_compenstation() << ",";
                json << "\"franchise\":" << insurance_products[i]->get_franchise() << ",";
                json << "\"customers\":" << insurance_products[i]->get_customers() << ",";
                json << "\"demand\":" << probabilityFromDemand(insurance_products[i]->demand()) * 100;
                json << "}";
            }
            json << "]}";
            res.set_content(json.str(), "application/json");
        } catch (const exception& e) {
            string error_json = "{\"error\":\"Server error: " + string(e.what()) + "\"}";
            res.set_content(error_json, "application/json");
        }
    };

    // Симуляция месяца - ИСПОЛЬЗУЕМ order вместо hardcoded 100
    auto handle_simulate = [](const httplib::Request&, httplib::Response& res) {
        if (!capital_set) {
            res.set_content("{\"status\":\"error\",\"message\":\"Capital not set yet\"}", "application/json");
            return;
        }
        try {
            double income_total = 0.0;
            double losses_total = 0.0;
            int new_customers_total = 0;

            for (auto& insurance : insurance_products) {
                int n = insurance->new_customers(order); // ИСПОЛЬЗУЕМ order вместо 100
                insurance->add_customers(n);

                if (insurance->get_type_name() == "Life Insurance") {
                    long long before = capital;
                    ManagerLife.MoneyLender(capital, *insurance);
                    income_total += std::max(0LL, capital - before);
                    before = capital;
                    ManagerLife.InsuredLoss(capital, probability);
                    losses_total += std::max(0LL, before - capital);
                    ManagerLife.AddCustomer(*insurance, n);
                }

                if (insurance->get_type_name() == "Estate Insurance") {
                    long long before = capital;
                    ManagerEstate.MoneyLender(capital, *insurance);
                    income_total += std::max(0LL, capital - before);
                    before = capital;
                    ManagerEstate.InsuredLoss(capital, probability);
                    losses_total += std::max(0LL, before - capital);
                    ManagerEstate.AddCustomer(*insurance, n);
                }

                if (insurance->get_type_name() == "Vehicle Insurance") {
                    long long before = capital;
                    ManagerVehicle.MoneyLender(capital, *insurance);
                    income_total += std::max(0LL, capital - before);
                    before = capital;
                    ManagerVehicle.InsuredLoss(capital, probability);
                    losses_total += std::max(0LL, before - capital);
                    ManagerVehicle.AddCustomer(*insurance, n);
                }

                new_customers_total += n;
            }

            current_month++;

            std::stringstream json;
            json << "{";
            json << "\"status\":\"success\",";
            json << "\"month\":" << current_month << ",";
            json << "\"income\":" << income_total << ",";
            json << "\"losses\":" << losses_total << ",";
            json << "\"new_customers\":" << new_customers_total << ",";
            json << "\"probability\":" << probability << ",";
            json << "\"order\":" << order; // ДОБАВЛЯЕМ order в ответ симуляции
            json << "}";

            res.set_content(json.str(), "application/json");
        } catch (const std::exception& e) {
            res.set_content(
                "{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}",
                "application/json"
            );
        }
    };

    // Добавить страховку
    auto handle_add_insurance = [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto type = req.get_param_value("type");
            auto fee = stoi(req.get_param_value("fee"));
            auto fee_period = stoi(req.get_param_value("fee_period"));
            auto duration = stoi(req.get_param_value("duration"));
            auto max_compensation = stoi(req.get_param_value("max_compensation"));
            auto franchise = stoi(req.get_param_value("franchise"));
            
            shared_ptr<Insurance> new_insurance;
            if (type == "life")
                new_insurance = make_shared<Life>(fee, fee_period, duration, max_compensation, franchise);
            else if (type == "estate")
                new_insurance = make_shared<Estate>(fee, fee_period, duration, max_compensation, franchise);
            else if (type == "vehicle")
                new_insurance = make_shared<Vehicle>(fee, fee_period, duration, max_compensation, franchise);

            if (new_insurance) {
                insurance_products.push_back(new_insurance);
                res.set_content("{\"status\":\"success\"}", "application/json");
            } else {
                res.set_content("{\"status\":\"error\",\"message\":\"Invalid insurance type\"}", "application/json");
            }
        } catch (const exception& e) {
            res.set_content("{\"status\":\"error\",\"message\":\"" + string(e.what()) + "\"}", "application/json");
        }
    };

    // Редактировать страховку
    auto handle_edit_insurance = [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto id = stoi(req.get_param_value("id"));
            auto fee = stoi(req.get_param_value("fee"));
            auto fee_period = stoi(req.get_param_value("fee_period"));
            auto duration = stoi(req.get_param_value("duration"));
            auto max_compensation = stoi(req.get_param_value("max_compensation"));
            auto franchise = stoi(req.get_param_value("franchise"));
            
            if (id < 0 || id >= insurance_products.size()) {
                res.set_content("{\"status\":\"error\",\"message\":\"Invalid insurance ID\"}", "application/json");
                return;
            }
            
            // Создаем новую страховку с обновленными параметрами
            shared_ptr<Insurance> updated_insurance;
            string type_name = insurance_products[id]->get_type_name();
            
            if (type_name == "Life Insurance")
                updated_insurance = make_shared<Life>(fee, fee_period, duration, max_compensation, franchise);
            else if (type_name == "Estate Insurance")
                updated_insurance = make_shared<Estate>(fee, fee_period, duration, max_compensation, franchise);
            else if (type_name == "Vehicle Insurance")
                updated_insurance = make_shared<Vehicle>(fee, fee_period, duration, max_compensation, franchise);
            
            if (updated_insurance) {
                // Сохраняем количество клиентов
                int current_customers = insurance_products[id]->get_customers();
                updated_insurance->add_customers(current_customers);
                
                // Заменяем страховку
                insurance_products[id] = updated_insurance;
                res.set_content("{\"status\":\"success\"}", "application/json");
            } else {
                res.set_content("{\"status\":\"error\",\"message\":\"Failed to update insurance\"}", "application/json");
            }
        } catch (const exception& e) {
            res.set_content("{\"status\":\"error\",\"message\":\"" + string(e.what()) + "\"}", "application/json");
        }
    };

    // Удалить страховку
    auto handle_delete_insurance = [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto id = stoi(req.get_param_value("id"));
            
            if (id < 0 || id >= insurance_products.size()) {
                res.set_content("{\"status\":\"error\",\"message\":\"Invalid insurance ID\"}", "application/json");
                return;
            }
            
            if (insurance_products.size() <= 1) {
                res.set_content("{\"status\":\"error\",\"message\":\"Cannot delete the last insurance product\"}", "application/json");
                return;
            }
            
            // Удаляем страховку
            insurance_products.erase(insurance_products.begin() + id);
            res.set_content("{\"status\":\"success\"}", "application/json");
        } catch (const exception& e) {
            res.set_content("{\"status\":\"error\",\"message\":\"" + string(e.what()) + "\"}", "application/json");
        }
    };

    // Изменить капитал
    auto handle_set_capital = [](const httplib::Request& req, httplib::Response& res) {
        try {
            if (capital_set) {
                res.set_content("{\"status\":\"error\",\"message\":\"Capital can only be set once at the start\"}", "application/json");
                return;
            }

            capital = stoll(req.get_param_value("capital"));
            if (capital <= 0) {
                res.set_content("{\"status\":\"error\",\"message\":\"Capital must be positive\"}", "application/json");
                return;
            }

            capital_set = true;
            res.set_content("{\"status\":\"success\",\"capital\":" + to_string(capital) + "}", "application/json");
        } catch (const exception& e) {
            res.set_content("{\"status\":\"error\",\"message\":\"Invalid capital value\"}", "application/json");
        }
    };

    // Установить вероятность
    auto handle_set_probability = [](const httplib::Request& req, httplib::Response& res) {
        try {
            double new_probability = stod(req.get_param_value("probability"));
            
            if (new_probability < 0.0 || new_probability > 1.0) {
                res.set_content("{\"status\":\"error\",\"message\":\"Probability must be between 0 and 1\"}", "application/json");
                return;
            }

            probability = new_probability;
            res.set_content("{\"status\":\"success\",\"probability\":" + to_string(probability) + "}", "application/json");
        } catch (const exception& e) {
            res.set_content("{\"status\":\"error\",\"message\":\"Invalid probability value\"}", "application/json");
        }
    };

    // Установить order - НОВЫЙ ENDPOINT
    auto handle_set_order = [](const httplib::Request& req, httplib::Response& res) {
        try {
            int new_order = stoi(req.get_param_value("order"));
            
            if (new_order < 0 || new_order > 10000) {
                res.set_content("{\"status\":\"error\",\"message\":\"Order must be between 0 and 10000\"}", "application/json");
                return;
            }

            order = new_order;
            res.set_content("{\"status\":\"success\",\"order\":" + to_string(order) + "}", "application/json");
        } catch (const exception& e) {
            res.set_content("{\"status\":\"error\",\"message\":\"Invalid order value\"}", "application/json");
        }
    };

    // Сброс
    auto handle_reset = [](const httplib::Request&, httplib::Response& res) {
        try {
            capital = 100000000;
            capital_set = false;
            current_month = 0;
            probability = 0.05;
            order = 100; // Сбрасываем order
            insurance_products.clear();
            ManagerLife = CustomerManager();
            ManagerEstate = CustomerManager();
            ManagerVehicle = CustomerManager();
            initialize_data();
            res.set_content("{\"status\":\"success\"}", "application/json");
        } catch (const exception& e) {
            res.set_content("{\"status\":\"error\",\"message\":\"" + string(e.what()) + "\"}", "application/json");
        }
    };


    // === Регистрация маршрутов ===
    server.Get("/test", handle_test);
    server.Get("/api/test", handle_test);

    server.Get("/state", handle_state);
    server.Get("/api/state", handle_state);

    server.Post("/simulate_month", handle_simulate);
    server.Post("/api/simulate_month", handle_simulate);
    server.Get("/simulate_month", handle_simulate);

    server.Post("/add_insurance", handle_add_insurance);
    server.Post("/api/add_insurance", handle_add_insurance);

    server.Post("/edit_insurance", handle_edit_insurance);
    server.Post("/api/edit_insurance", handle_edit_insurance);

    server.Post("/delete_insurance", handle_delete_insurance);
    server.Post("/api/delete_insurance", handle_delete_insurance);

    server.Post("/set_capital", handle_set_capital);
    server.Post("/api/set_capital", handle_set_capital);

    server.Post("/set_probability", handle_set_probability);
    server.Post("/api/set_probability", handle_set_probability);
    server.Get("/set_probability", handle_set_probability);
    server.Get("/api/set_probability", handle_set_probability);

    server.Post("/set_order", handle_set_order);
    server.Post("/api/set_order", handle_set_order);
    server.Get("/set_order", handle_set_order);
    server.Get("/api/set_order", handle_set_order);

    server.Post("/reset", handle_reset);
    server.Post("/api/reset", handle_reset);
    server.Get("/reset", handle_reset);
    server.Get("/api/reset", handle_reset);

    // Статические файлы
    server.set_mount_point("/", "../www");

    cout << "Insurance Server running on http://localhost:8080" << endl;

    server.listen("localhost", 8080);
    return 0;
}