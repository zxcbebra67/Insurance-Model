// Порхай как бабочка, жаль что петух
#pragma once
#include <QMainWindow>
#include "company.h"

#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT   // <--- добавь ЭТО

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onInitClicked();
    void onNextMonthClicked();

private:
    void setupUi();
    void updateStatus();

    Company company_;

    // UI elements
    QLineEdit *initialCapitalEdit_;
    QLineEdit *taxRateEdit_;
    QLineEdit *monthsEdit_;

    QLineEdit *housePremiumEdit_;
    QLineEdit *houseDurationEdit_;
    QLineEdit *houseMaxCompEdit_;
    QLineEdit *houseFranchiseEdit_;

    QLineEdit *carPremiumEdit_;
    QLineEdit *carDurationEdit_;
    QLineEdit *carMaxCompEdit_;
    QLineEdit *carFranchiseEdit_;

    QLineEdit *healthPremiumEdit_;
    QLineEdit *healthDurationEdit_;
    QLineEdit *healthMaxCompEdit_;
    QLineEdit *healthFranchiseEdit_;

    QLineEdit *baseDemandHouseEdit_;
    QLineEdit *baseDemandCarEdit_;
    QLineEdit *baseDemandHealthEdit_;

    QLineEdit *trustFactorEdit_;

    QPushButton *initButton_;
    QPushButton *nextMonthButton_;
    QTextEdit *logView_;
    QLabel *capitalLabel_;
    QTableWidget *conditionsTable_;
};
