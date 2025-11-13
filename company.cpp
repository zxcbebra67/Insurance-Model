// Порхай как бабочка, жаль что петух
#include "company.h"
#include <QString>
#include <cmath>
#include <algorithm>

Company::Company(double initialCapital, double taxRate)
    : capital_(initialCapital), taxRate_(taxRate)
{
    rng_.seed(std::random_device{}());
}

void Company::setBaseDemandHouse(int d) { baseDemandHouse_ = d; }
void Company::setBaseDemandCar(int d) { baseDemandCar_ = d; }
void Company::setBaseDemandHealth(int d) { baseDemandHealth_ = d; }

void Company::setHouseTemplate(std::unique_ptr<Policy> p) { houseTemplate_ = std::move(p); }
void Company::setCarTemplate(std::unique_ptr<Policy> p) { carTemplate_ = std::move(p); }
void Company::setHealthTemplate(std::unique_ptr<Policy> p) { healthTemplate_ = std::move(p); }

void Company::initSimulation(int monthsToSimulate, std::uint32_t rngSeed) {
    monthsTotal_ = monthsToSimulate;
    currentMonth_ = 0;
    bankrupt_ = false;
    soldContracts_.clear();
    soldHouse_ = soldCar_ = soldHealth_ = 0;
    rng_.seed(rngSeed);
}

// === Спрос с учётом доверия ===
int Company::computeDemand(const Policy* tpl, int baseDemand) {
    if (!tpl) return 0;

    double ratio = tpl->totalPremium() / tpl->maxCompensation();
    double factor = 1.0 + std::max(0.0, 1.0 - ratio);
    std::uniform_int_distribution<int> randExtra(0, 10);

    // 🔹 Добавляем коэффициент доверия
    // Чем выше капитал, тем выше доверие населения и спрос.
    // Формула: demandBoost = 1 + trustFactor_ * log1p(capital_)
    double demandBoost = 1.0 + trustFactor_ * std::log1p(capital_);
    double adjustedBase = baseDemand * demandBoost;

    int demand = static_cast<int>(std::round(adjustedBase * factor)) + randExtra(rng_);
    return std::max(0, demand);
}

int Company::simulateClaimsNumber(int minCases, int maxCases) {
    std::uniform_int_distribution<int> dist(minCases, maxCases);
    return dist(rng_);
}

void Company::payTaxes(double profit) {
    if (profit <= 0) return;
    double tax = profit * (taxRate_ / 100.00);
    capital_ -= tax;
}

void Company::sellPolicies(int soldHouse, int soldCar, int soldHealth) {
    for (int i = 0; i < soldHouse; ++i) {
        SoldContract sc;
        sc.policy = houseTemplate_->clone();
        sc.monthsLeft = houseTemplate_->durationMonths();
        sc.collectedPremium = sc.policy->totalPremium();
        soldContracts_.push_back(std::move(sc));
        capital_ += sc.collectedPremium;
        soldHouse_++;
    }
    for (int i = 0; i < soldCar; ++i) {
        SoldContract sc;
        sc.policy = carTemplate_->clone();
        sc.monthsLeft = carTemplate_->durationMonths();
        sc.collectedPremium = sc.policy->totalPremium();
        soldContracts_.push_back(std::move(sc));
        capital_ += sc.collectedPremium;
        soldCar_++;
    }
    for (int i = 0; i < soldHealth; ++i) {
        SoldContract sc;
        sc.policy = healthTemplate_->clone();
        sc.monthsLeft = healthTemplate_->durationMonths();
        sc.collectedPremium = sc.policy->totalPremium();
        soldContracts_.push_back(std::move(sc));
        capital_ += sc.collectedPremium;
        soldHealth_++;
    }
}

double Company::processClaimsAndPayouts() {
    double totalPayout = 0.0;

    int houseContracts = 0, carContracts = 0, healthContracts = 0;
    for (const auto &c : soldContracts_) {
        if (c.policy->name() == "House") ++houseContracts;
        else if (c.policy->name() == "Car") ++carContracts;
        else if (c.policy->name() == "Health") ++healthContracts;
    }

    auto handleType = [&](const QString& typeName, int contracts) {
        if (contracts == 0) return 0.0;
        int numCases = simulateClaimsNumber(claimMin_, claimMax_);
        numCases = std::min(numCases, contracts * 2);
        double payoutForType = 0.0;
        std::uniform_int_distribution<int> pick(0, contracts - 1);
        std::vector<int> indices;
        for (int i = 0; i < (int)soldContracts_.size(); ++i) {
            if (soldContracts_[i].policy->name() == typeName) indices.push_back(i);
        }
        for (int c = 0; c < numCases; ++c) {
            int idx = indices[pick(rng_)];
            double amt = soldContracts_[idx].policy->simulateClaimAmount(rng_);
            if (amt > 0.0) {
                payoutForType += amt;
                capital_ -= amt;
            }
        }
        return payoutForType;
    };

    double p1 = handleType("House", houseContracts);
    double p2 = handleType("Car", carContracts);
    double p3 = handleType("Health", healthContracts);

    totalPayout = p1 + p2 + p3;
    return totalPayout;
}

QString Company::simulateNextMonth() {
    if (bankrupt_) return QString("Компания обанкротилась. Симуляция остановлена.");
    if (currentMonth_ >= monthsTotal_) return QString("Симуляция успешно завершена.");

    ++currentMonth_;
    QString log;
    log += QString("----- Месяц %1 -----\n").arg(currentMonth_);

    double capitalBefore = capital_;

    // 1️⃣ Продажа страховок
    int demandHouse = computeDemand(houseTemplate_.get(), baseDemandHouse_);
    int demandCar = computeDemand(carTemplate_.get(), baseDemandCar_);
    int demandHealth = computeDemand(healthTemplate_.get(), baseDemandHealth_);

    std::uniform_int_distribution<int> adj(-5, 5);
    demandHouse = std::max(0, demandHouse + adj(rng_));
    demandCar = std::max(0, demandCar + adj(rng_));
    demandHealth = std::max(0, demandHealth + adj(rng_));

    log += QString("Оценка спроса (с доверием): Дом=%1, Авто=%2, Здоровье=%3\n")
            .arg(demandHouse).arg(demandCar).arg(demandHealth);

    double conversion = 0.3;
    int soldHouse = static_cast<int>(std::round(demandHouse * conversion));
    int soldCar = static_cast<int>(std::round(demandCar * conversion));
    int soldHealth = static_cast<int>(std::round(demandHealth * conversion));

    sellPolicies(soldHouse, soldCar, soldHealth);
    log += QString("Продано полисов: Дом=%1, Авто=%2, Здоровье=%3\n")
            .arg(soldHouse).arg(soldCar).arg(soldHealth);

    // 2️⃣ Выплаты
    double payouts = processClaimsAndPayouts();
    log += QString("Выплаты по страховым случаям: %1\n").arg(payouts, 0, 'f', 2);

    // 3️⃣ Удаляем истёкшие контракты
    for (auto &c : soldContracts_) c.monthsLeft -= 1;
    soldContracts_.erase(std::remove_if(soldContracts_.begin(), soldContracts_.end(),
                                        [](const SoldContract &sc) { return sc.monthsLeft <= 0; }),
                         soldContracts_.end());

    // 4️⃣ Подсчёт прибыли и налог
    double profit = capital_ - capitalBefore;
    payTaxes(profit);
    log += QString("Прибыль за месяц: %1\n").arg(profit, 0, 'f', 2);
    if (profit > 0)
        log += QString("Уплачено налогов: %1\n").arg(profit * (taxRate_ / 100.00), 0, 'f', 2);
    else
        log += "Убыток, налог не уплачен.\n";

    log += QString("Активных контрактов: %1\n").arg((int)soldContracts_.size());
    log += QString("Капитал после месяца %1: %2\n").arg(currentMonth_).arg(capital_, 0, 'f', 2);

    if (capital_ < 0.0) {
        bankrupt_ = true;
        log += "Компания банкрот. Симуляция остановлена.\n";
    }

    if (currentMonth_ >= monthsTotal_) {
        log += "Симуляция успешно завершена.\n";
    }

    return log;
}
