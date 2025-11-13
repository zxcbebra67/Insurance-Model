// Порхай как бабочка, жаль что петух
#include "mainwindow.h"
#include "policy.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QIntValidator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), company_(30000.0, 0.09)
{
    setupUi();
}

void MainWindow::setupUi() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // === ПАРАМЕТРЫ КОМПАНИИ ===
    QGroupBox *companyBox = new QGroupBox("Параметры компании");
    QFormLayout *companyForm = new QFormLayout();
    initialCapitalEdit_ = new QLineEdit("30000");
    taxRateEdit_ = new QLineEdit("0.09");
    monthsEdit_ = new QLineEdit("12");
    companyForm->addRow("Начальный капитал", initialCapitalEdit_);
    companyForm->addRow("Налоговая ставка (например, 0.09)", taxRateEdit_);
    companyForm->addRow("Количество месяцев моделирования", monthsEdit_);
    companyBox->setLayout(companyForm);

    // === УСЛОВИЯ ПОЛИСОВ ===
    QGroupBox *policiesBox = new QGroupBox("Страховые продукты");
    QGridLayout *polGrid = new QGridLayout();

    polGrid->addWidget(new QLabel("Тип"), 0, 0);
    polGrid->addWidget(new QLabel("Ежемесячный взнос"), 0, 1);
    polGrid->addWidget(new QLabel("Длительность (мес.)"), 0, 2);
    polGrid->addWidget(new QLabel("Макс. выплата"), 0, 3);
    polGrid->addWidget(new QLabel("Франшиза"), 0, 4);

    // Дом
    polGrid->addWidget(new QLabel("Дом"), 1, 0);
    housePremiumEdit_ = new QLineEdit("135");
    houseDurationEdit_ = new QLineEdit("12");
    houseMaxCompEdit_ = new QLineEdit("10000");
    houseFranchiseEdit_ = new QLineEdit("1000");
    polGrid->addWidget(housePremiumEdit_, 1, 1);
    polGrid->addWidget(houseDurationEdit_, 1, 2);
    polGrid->addWidget(houseMaxCompEdit_, 1, 3);
    polGrid->addWidget(houseFranchiseEdit_, 1, 4);

    // Авто
    polGrid->addWidget(new QLabel("Авто"), 2, 0);
    carPremiumEdit_ = new QLineEdit("180");
    carDurationEdit_ = new QLineEdit("12");
    carMaxCompEdit_ = new QLineEdit("12000");
    carFranchiseEdit_ = new QLineEdit("500");
    polGrid->addWidget(carPremiumEdit_, 2, 1);
    polGrid->addWidget(carDurationEdit_, 2, 2);
    polGrid->addWidget(carMaxCompEdit_, 2, 3);
    polGrid->addWidget(carFranchiseEdit_, 2, 4);

    // Здоровье
    polGrid->addWidget(new QLabel("Здоровье"), 3, 0);
    healthPremiumEdit_ = new QLineEdit("105");
    healthDurationEdit_ = new QLineEdit("12");
    healthMaxCompEdit_ = new QLineEdit("8000");
    healthFranchiseEdit_ = new QLineEdit("200");
    polGrid->addWidget(healthPremiumEdit_, 3, 1);
    polGrid->addWidget(healthDurationEdit_, 3, 2);
    polGrid->addWidget(healthMaxCompEdit_, 3, 3);
    polGrid->addWidget(healthFranchiseEdit_, 3, 4);

    policiesBox->setLayout(polGrid);

    // === СПРОС ===
    QGroupBox *demandBox = new QGroupBox("Базовый спрос населения");
    QFormLayout *demandForm = new QFormLayout();
    baseDemandHouseEdit_ = new QLineEdit("50");
    baseDemandCarEdit_ = new QLineEdit("40");
    baseDemandHealthEdit_ = new QLineEdit("60");
    trustFactorEdit_ = new QLineEdit("0.00002");
    demandForm->addRow("Дом", baseDemandHouseEdit_);
    demandForm->addRow("Авто", baseDemandCarEdit_);
    demandForm->addRow("Здоровье", baseDemandHealthEdit_);
    demandForm->addRow("Коэффициент доверия", trustFactorEdit_);
    demandBox->setLayout(demandForm);

    // === КНОПКИ ===
    QHBoxLayout *buttonsLay = new QHBoxLayout();
    initButton_ = new QPushButton("Инициализировать симуляцию");
    nextMonthButton_ = new QPushButton("Следующий месяц");
    updatePoliciesButton_ = new QPushButton("Обновить условия полисов"); // ✅ новая кнопка

    nextMonthButton_->setEnabled(false);
    updatePoliciesButton_->setEnabled(false);

    buttonsLay->addWidget(initButton_);
    buttonsLay->addWidget(nextMonthButton_);
    buttonsLay->addWidget(updatePoliciesButton_);

    // === СТАТУС ===
    QHBoxLayout *statusLay = new QHBoxLayout();
    capitalLabel_ = new QLabel("Капитал: 0.00");
    statusLay->addWidget(capitalLabel_);
    statusLay->addStretch();

    // === ЛОГ ===
    logView_ = new QTextEdit();
    logView_->setReadOnly(true);

    mainLayout->addWidget(companyBox);
    mainLayout->addWidget(policiesBox);
    mainLayout->addWidget(demandBox);
    mainLayout->addLayout(buttonsLay);
    mainLayout->addLayout(statusLay);
    mainLayout->addWidget(new QLabel("Журнал симуляции:"));
    mainLayout->addWidget(logView_, 1);

    connect(initButton_, &QPushButton::clicked, this, &MainWindow::onInitClicked);
    connect(nextMonthButton_, &QPushButton::clicked, this, &MainWindow::onNextMonthClicked);
    connect(updatePoliciesButton_, &QPushButton::clicked, this, &MainWindow::onUpdatePoliciesClicked);

    resize(900, 700);
    setWindowTitle("Модель страховой компании");

    // === Валидаторы ===
    auto *doubleVal = new QDoubleValidator(0, 1e9, 6, this);
    doubleVal->setLocale(QLocale::English);
    auto *intVal = new QIntValidator(1, 1000000, this);

    QList<QLineEdit*> doubles = {
        taxRateEdit_, housePremiumEdit_, carPremiumEdit_, healthPremiumEdit_,
        houseMaxCompEdit_, carMaxCompEdit_, healthMaxCompEdit_,
        houseFranchiseEdit_, carFranchiseEdit_, healthFranchiseEdit_,
        trustFactorEdit_
    };
    for (auto *e : doubles) e->setValidator(doubleVal);

    QList<QLineEdit*> ints = {
        monthsEdit_, houseDurationEdit_, carDurationEdit_, healthDurationEdit_,
        baseDemandHouseEdit_, baseDemandCarEdit_, baseDemandHealthEdit_
    };
    for (auto *e : ints) e->setValidator(intVal);

    initialCapitalEdit_->setValidator(doubleVal);
}

void MainWindow::onInitClicked() {
    QList<QLineEdit*> allFields = {
        initialCapitalEdit_, taxRateEdit_, monthsEdit_,
        housePremiumEdit_, houseDurationEdit_, houseMaxCompEdit_, houseFranchiseEdit_,
        carPremiumEdit_, carDurationEdit_, carMaxCompEdit_, carFranchiseEdit_,
        healthPremiumEdit_, healthDurationEdit_, healthMaxCompEdit_, healthFranchiseEdit_,
        baseDemandHouseEdit_, baseDemandCarEdit_, baseDemandHealthEdit_,
        trustFactorEdit_
    };

    for (auto *e : allFields) {
        if (e->text().isEmpty() || !e->hasAcceptableInput()) {
            QMessageBox::warning(this, "Неправильный ввод", "Проверьте введённые значения (только числа).");
            return;
        }
    }

    double initialCapital = initialCapitalEdit_->text().toDouble();
    double taxRate = taxRateEdit_->text().toDouble();
    int months = monthsEdit_->text().toInt();

    double hPremium = housePremiumEdit_->text().toDouble();
    int hDuration = houseDurationEdit_->text().toInt();
    double hMax = houseMaxCompEdit_->text().toDouble();
    double hFran = houseFranchiseEdit_->text().toDouble();

    double cPremium = carPremiumEdit_->text().toDouble();
    int cDuration = carDurationEdit_->text().toInt();
    double cMax = carMaxCompEdit_->text().toDouble();
    double cFran = carFranchiseEdit_->text().toDouble();

    double hePremium = healthPremiumEdit_->text().toDouble();
    int heDuration = healthDurationEdit_->text().toInt();
    double heMax = healthMaxCompEdit_->text().toDouble();
    double heFran = healthFranchiseEdit_->text().toDouble();

    int baseH = baseDemandHouseEdit_->text().toInt();
    int baseC = baseDemandCarEdit_->text().toInt();
    int baseHe = baseDemandHealthEdit_->text().toInt();
    double trust = trustFactorEdit_->text().toDouble();

    company_ = Company(initialCapital, taxRate);
    company_.setBaseDemandHouse(baseH);
    company_.setBaseDemandCar(baseC);
    company_.setBaseDemandHealth(baseHe);
    company_.setHouseTemplate(makeHousePolicy(hPremium, hDuration, hMax, hFran));
    company_.setCarTemplate(makeCarPolicy(cPremium, cDuration, cMax, cFran));
    company_.setHealthTemplate(makeHealthPolicy(hePremium, heDuration, heMax, heFran));
    company_.setTrustFactor(trust);

    company_.initSimulation(months, static_cast<std::uint32_t>(std::random_device{}()));
    logView_->clear();
    logView_->append(QString("Симуляция инициализирована. Начальный капитал: %1, месяцев: %2").arg(initialCapital).arg(months));
    updateStatus();

    nextMonthButton_->setEnabled(true);
    updatePoliciesButton_->setEnabled(true); // ✅ включаем возможность обновления
}

void MainWindow::onNextMonthClicked() {
    if (company_.isBankrupt()) {
        logView_->append("Компания банкрот. Симуляция остановлена.");
        nextMonthButton_->setEnabled(false);
        return;
    }
    QString log = company_.simulateNextMonth();
    logView_->append(log);
    updateStatus();
    if (company_.isBankrupt() || company_.currentMonth() >= company_.monthsTotal()) {
        nextMonthButton_->setEnabled(false);
        updatePoliciesButton_->setEnabled(false);
    }
}

void MainWindow::onUpdatePoliciesClicked() {
    if (company_.isBankrupt()) {
        QMessageBox::warning(this, "Ошибка", "Компания банкрот, обновление невозможно.");
        return;
    }

    double hPremium = housePremiumEdit_->text().toDouble();
    int hDuration = houseDurationEdit_->text().toInt();
    double hMax = houseMaxCompEdit_->text().toDouble();
    double hFran = houseFranchiseEdit_->text().toDouble();

    double cPremium = carPremiumEdit_->text().toDouble();
    int cDuration = carDurationEdit_->text().toInt();
    double cMax = carMaxCompEdit_->text().toDouble();
    double cFran = carFranchiseEdit_->text().toDouble();

    double hePremium = healthPremiumEdit_->text().toDouble();
    int heDuration = healthDurationEdit_->text().toInt();
    double heMax = healthMaxCompEdit_->text().toDouble();
    double heFran = healthFranchiseEdit_->text().toDouble();

    company_.setHouseTemplate(makeHousePolicy(hPremium, hDuration, hMax, hFran));
    company_.setCarTemplate(makeCarPolicy(cPremium, cDuration, cMax, cFran));
    company_.setHealthTemplate(makeHealthPolicy(hePremium, heDuration, heMax, heFran));

    logView_->append("✅ Условия страховых продуктов обновлены. Новые полисы будут продаваться по обновлённым тарифам.");
}

void MainWindow::updateStatus() {
    capitalLabel_->setText(QString("Капитал: %1").arg(company_.capital(), 0, 'f', 2));
}