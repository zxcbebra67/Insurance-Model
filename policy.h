// Порхай как бабочка, жаль что петух
#pragma once
#include <QString>
#include <random>
#include <memory>

class Policy {
public:
    Policy(const QString& name,
           double monthlyPremium,
           int durationMonths,
           double maxCompensation,
           double franchise);
    virtual ~Policy() = default;

    QString name() const;
    double monthlyPremium() const;
    int durationMonths() const;
    double maxCompensation() const;
    double franchise() const;

    virtual double totalPremium() const;
    virtual double simulateClaimAmount(std::mt19937 &rng) const = 0;
    virtual std::unique_ptr<Policy> clone() const = 0;

protected:
    QString name_;
    double monthlyPremium_;
    int durationMonths_;
    double maxCompensation_;
    double franchise_;
};

// ↓↓↓ ДОБАВЬ ЭТИ ОБЪЯВЛЕНИЯ ↓↓↓

// Фабричные функции для создания конкретных типов полисов
std::unique_ptr<Policy> makeHousePolicy(double monthlyPremium, int durationMonths,
                                        double maxCompensation, double franchise);
std::unique_ptr<Policy> makeCarPolicy(double monthlyPremium, int durationMonths,
                                      double maxCompensation, double franchise);
std::unique_ptr<Policy> makeHealthPolicy(double monthlyPremium, int durationMonths,
                                         double maxCompensation, double franchise);
