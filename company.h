// Порхай как бабочка, жаль что петух
#pragma once
#include "policy.h"
#include <vector>
#include <memory>
#include <random>
#include <QString>

// Контракт, проданный населению
struct SoldContract {
    std::unique_ptr<Policy> policy; // клонированный продукт
    int monthsLeft;
    double collectedPremium; // уже собранная премия
};

class Company {
public:
    Company(double initialCapital = 30000.0, double taxRate = 0.09);

    // === Параметры спроса ===
    void setBaseDemandHouse(int d);
    void setBaseDemandCar(int d);
    void setBaseDemandHealth(int d);

    // === Шаблоны страховок ===
    void setHouseTemplate(std::unique_ptr<Policy> p);
    void setCarTemplate(std::unique_ptr<Policy> p);
    void setHealthTemplate(std::unique_ptr<Policy> p);

    // === Новый параметр: коэффициент доверия ===
    void setTrustFactor(double factor) { trustFactor_ = factor; }

    // === Симуляция ===
    void initSimulation(int monthsToSimulate, std::uint32_t rngSeed = std::random_device{}());
    QString simulateNextMonth();

    // === Доступ к состоянию ===
    double capital() const { return capital_; }
    bool isBankrupt() const { return bankrupt_; }
    int currentMonth() const { return currentMonth_; }
    int monthsTotal() const { return monthsTotal_; }

private:
    // === Внутренние функции ===
    int computeDemand(const Policy* tpl, int baseDemand);
    int simulateClaimsNumber(int minCases, int maxCases);
    void payTaxes(double profit);
    void sellPolicies(int soldHouse, int soldCar, int soldHealth);
    double processClaimsAndPayouts();

private:
    double capital_;
    double taxRate_;
    double trustFactor_ = 0.00002; // коэффициент доверия по умолчанию
    bool bankrupt_ = false;

    std::unique_ptr<Policy> houseTemplate_;
    std::unique_ptr<Policy> carTemplate_;
    std::unique_ptr<Policy> healthTemplate_;

    std::vector<SoldContract> soldContracts_;

    // Базовый спрос
    int baseDemandHouse_ = 50;
    int baseDemandCar_ = 40;
    int baseDemandHealth_ = 60;

    // Статистика
    int soldHouse_ = 0;
    int soldCar_ = 0;
    int soldHealth_ = 0;

    // Симуляция
    int currentMonth_ = 0;
    int monthsTotal_ = 12;

    // Генератор случайных чисел
    std::mt19937 rng_;

    // Диапазон числа страховых случаев
    int claimMin_ = 1;
    int claimMax_ = 5;
};
