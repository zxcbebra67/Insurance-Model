// Порхай как бабочка, жаль что петух
#include "policy.h"

// ===== Реализация базового класса Policy =====
Policy::Policy(const QString& name,
               double monthlyPremium,
               int durationMonths,
               double maxCompensation,
               double franchise)
    : name_(name),
      monthlyPremium_(monthlyPremium),
      durationMonths_(durationMonths),
      maxCompensation_(maxCompensation),
      franchise_(franchise)
{}

QString Policy::name() const { return name_; }
double Policy::monthlyPremium() const { return monthlyPremium_; }
int Policy::durationMonths() const { return durationMonths_; }
double Policy::maxCompensation() const { return maxCompensation_; }
double Policy::franchise() const { return franchise_; }

double Policy::totalPremium() const {
    return monthlyPremium_ * durationMonths_;
}
// Домашняя страховка
class HousePolicy : public Policy {
public:
    HousePolicy(double monthlyPremium, int durationMonths, double maxCompensation, double franchise)
        : Policy("House", monthlyPremium, durationMonths, maxCompensation, franchise) {}

    double simulateClaimAmount(std::mt19937 &rng) const override {
        // Коэффициент ущерба Uniform(0.01, 1.0]
        std::uniform_real_distribution<double> dist(0.01, 1.0);
        double coef = dist(rng);
        double raw = maxCompensation_ * coef;
        if (raw <= franchise_) return 0.0;
        return raw - franchise_;
    }

    std::unique_ptr<Policy> clone() const override {
        return std::make_unique<HousePolicy>(*this);
    }
};

// Авто
class CarPolicy : public Policy {
public:
    CarPolicy(double monthlyPremium, int durationMonths, double maxCompensation, double franchise)
        : Policy("Car", monthlyPremium, durationMonths, maxCompensation, franchise) {}

    double simulateClaimAmount(std::mt19937 &rng) const override {
        // Часто меньше повреждение: Uniform(0.05, 0.8]
        std::uniform_real_distribution<double> dist(0.05, 0.8);
        double coef = dist(rng);
        double raw = maxCompensation_ * coef;
        if (raw <= franchise_) return 0.0;
        return raw - franchise_;
    }

    std::unique_ptr<Policy> clone() const override {
        return std::make_unique<CarPolicy>(*this);
    }
};

// Здоровье
class HealthPolicy : public Policy {
public:
    HealthPolicy(double monthlyPremium, int durationMonths, double maxCompensation, double franchise)
        : Policy("Health", monthlyPremium, durationMonths, maxCompensation, franchise) {}

    double simulateClaimAmount(std::mt19937 &rng) const override {
        // Возможны и маленькие и большие суммы: Uniform(0.01, 0.5]
        std::uniform_real_distribution<double> dist(0.01, 0.5);
        double coef = dist(rng);
        double raw = maxCompensation_ * coef;
        if (raw <= franchise_) return 0.0;
        return raw - franchise_;
    }

    std::unique_ptr<Policy> clone() const override {
        return std::make_unique<HealthPolicy>(*this);
    }
};

// Чтобы отдельный файл policy.cpp знал о подклассах, экспортируем фабрики:
std::unique_ptr<Policy> makeHousePolicy(double monthlyPremium, int durationMonths, double maxCompensation, double franchise) {
    return std::make_unique<HousePolicy>(monthlyPremium, durationMonths, maxCompensation, franchise);
}
std::unique_ptr<Policy> makeCarPolicy(double monthlyPremium, int durationMonths, double maxCompensation, double franchise) {
    return std::make_unique<CarPolicy>(monthlyPremium, durationMonths, maxCompensation, franchise);
}
std::unique_ptr<Policy> makeHealthPolicy(double monthlyPremium, int durationMonths, double maxCompensation, double franchise) {
    return std::make_unique<HealthPolicy>(monthlyPremium, durationMonths, maxCompensation, franchise);
}
