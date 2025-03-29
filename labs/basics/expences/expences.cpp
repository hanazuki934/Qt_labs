#include "expences.h"

ExpenceProgressBar::ExpenceProgressBar(QWidget *parent)
    : QProgressBar(parent) {
    setFormat(QString("Потрачено - %1%. Осталось - %2%").arg(0).arg(100));
    setAlignment(Qt::AlignCenter);
}

void ExpenceProgressBar::updateProgress(double expence, double budget) {
    double left = budget - expence;
    if (left <= 0) {
        setRange(0, 1);
        setValue(1);
        setFormat(QString("Потрачено - %1 BYN. Бюджет превышен!").arg(expence, 0, 'f', 2));
        setStyleSheet("QProgressBar::chunk { background-color: red; }");
        return;
    }
    setRange(0, 100);
    setValue(static_cast<int>(expence / budget * 100));
    setFormat(QString("Потрачено - %1 BYN. Осталось - %2 BYN").arg(expence, 0, 'f', 2).arg(left, 0, 'f', 2));
    if ((expence / budget) <= 0.33) {
        setStyleSheet("QProgressBar::chunk { background-color: green; }");
    } else if ((expence / budget) <= 0.66) {
        setStyleSheet("QProgressBar::chunk { background-color: yellow; }");
    } else {
        setStyleSheet("QProgressBar::chunk { background-color: red; }");
    }
}

ExpenceAddWindow::ExpenceAddWindow(QWidget *parent) : QDialog(parent) {
    mainLayout_ = new QGridLayout(this);
    setWindowTitle("Добавление информации");
    resize(200, 300);

    infoLabel_ = new QLabel("Введите информацию о новой трате: сумму, категорию, дату", this);

    addSumSpinBox_ = new QDoubleSpinBox(this);
    addSumSpinBox_->setMinimum(0.01);
    addSumSpinBox_->setMaximum(1000000.00);
    addSumSpinBox_->setValue(1.00);
    addSumSpinBox_->setSuffix(" BYN");

    addCategoryComboBox_ = new QComboBox(this);
    addCategoryComboBox_->addItems({"Не задано", "Продукты", "Жильё", "Транспорт", "Развлечения"});

    addDateEdit_ = new QDateEdit(this);
    addDateEdit_->setDate(QDate::currentDate());
    addDateEdit_->setDisplayFormat("dd/MM/yyyy");

    addInfoLineEdit_ = new QLineEdit(this);
    addInfoLineEdit_->setPlaceholderText("Дополнительная информация");

    addExpenceButton_ = new QPushButton("Добавить трату", this);

    mainLayout_->addWidget(infoLabel_);
    mainLayout_->addWidget(addSumSpinBox_);
    mainLayout_->addWidget(addCategoryComboBox_);
    mainLayout_->addWidget(addDateEdit_);
    mainLayout_->addWidget(addInfoLineEdit_);
    mainLayout_->addWidget(addExpenceButton_);

    connect(addExpenceButton_, &QPushButton::clicked, this, &ExpenceAddWindow::accept);
}

double ExpenceAddWindow::getSum() {
    return addSumSpinBox_->value();
}

QString ExpenceAddWindow::getCategory() {
    return addCategoryComboBox_->currentText();
}

QDate ExpenceAddWindow::getDate() {
    return addDateEdit_->date();
}

QString ExpenceAddWindow::getInfo() {
    return addInfoLineEdit_->text();
}

ExpenceChangeWindow::ExpenceChangeWindow(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Изменение информации");
    resize(200, 300);
    mainLayout_ = new QVBoxLayout(this);

    confirmChangePushButton_ = new QPushButton("Изменить информацию в таблице", this);

    deletePushButton_ = new QPushButton("Удалить информацию", this);

    changeSumSpinBox_ = new QDoubleSpinBox(this);
    changeSumSpinBox_->setMinimum(0.01);
    changeSumSpinBox_->setMaximum(1000000.00);
    changeSumSpinBox_->setValue(1.00);
    changeSumSpinBox_->setSuffix(" BYN");

    changeCategoryComboBox_ = new QComboBox(this);
    changeCategoryComboBox_->addItems({"Не задано", "Продукты", "Жильё", "Транспорт", "Развлечения"});

    changeDateEdit_ = new QDateEdit(this);
    changeDateEdit_->setDate(QDate::currentDate());
    changeDateEdit_->setDisplayFormat("dd/MM/yyyy");

    changeInfoLineEdit_ = new QLineEdit(this);
    changeInfoLineEdit_->setPlaceholderText("Дополнительная информация");

    mainLayout_->addWidget(changeSumSpinBox_);
    mainLayout_->addWidget(changeCategoryComboBox_);
    mainLayout_->addWidget(changeDateEdit_);
    mainLayout_->addWidget(changeInfoLineEdit_);
    mainLayout_->addWidget(confirmChangePushButton_);
    mainLayout_->addWidget(deletePushButton_);

    connect(confirmChangePushButton_, &QPushButton::clicked, this, &ExpenceChangeWindow::accept);
    connect(deletePushButton_, &QPushButton::clicked, this, &ExpenceChangeWindow::onDeleteButtonClicked);
}

void ExpenceChangeWindow::setValues(int sum, int index, QDate date, QString info) {
    changeSumSpinBox_->setValue(sum);
    changeCategoryComboBox_->setCurrentIndex(index);
    changeDateEdit_->setDate(date);
    changeInfoLineEdit_->setText(info);
}

double ExpenceChangeWindow::getSum() {
    return changeSumSpinBox_->value();
}

QString ExpenceChangeWindow::getCategory() {
    return changeCategoryComboBox_->currentText();
}

QDate ExpenceChangeWindow::getDate() {
    return changeDateEdit_->date();
}

QString ExpenceChangeWindow::getInfo() {
    return changeInfoLineEdit_->text();
}

ExpenceApp::ExpenceApp(QWidget *parent)
    : QWidget(parent) {
    setWindowTitle("Приложение для мониторинга расходов");
    resize(800, 600);

    addPushButton_ = new QPushButton("Добавить расход", this);

    expenceTable_ = new QTableWidget(this);
    expenceTable_->setColumnCount(4);
    expenceTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    expenceTable_->setHorizontalHeaderLabels(QStringList() << "Дата" << "Категория" << "Сумма" << "Информация");
    expenceTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    expenceTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    expenceTable_->horizontalHeader()->setStretchLastSection(true);

    budgetSpentProgressBar_ = new ExpenceProgressBar(this);

    budgetSetSpinBox_ = new QDoubleSpinBox(this);
    budgetSetSpinBox_->setMinimum(0);
    budgetSetSpinBox_->setMaximum(1000000);
    budgetSetSpinBox_->setSuffix(" BYN");

    budgetSetGroupBox_ = new QGroupBox(this);
    QVBoxLayout *budgetLayout_ = new QVBoxLayout(budgetSetGroupBox_);
    budgetLayout_->addWidget(new QLabel("Введите месячный бюджет:"));
    budgetLayout_->addWidget(budgetSetSpinBox_);

    changeExpenceButton_ = new QPushButton("Изменить выбранную трату", this);
    changeExpenceButton_->setEnabled(false);
    clearExpenceButton_ = new QPushButton("Очистить список трат", this);
    clearExpenceButton_->setEnabled(false);

    changeExpenceGroupBox_ = new QGroupBox(this);
    QVBoxLayout *changeExpenceLayout_ = new QVBoxLayout(changeExpenceGroupBox_);
    changeExpenceLayout_->addWidget(changeExpenceButton_);
    changeExpenceLayout_->addWidget(clearExpenceButton_);

    chooseSortComboBox_ = new QComboBox(this);
    chooseSortComboBox_->addItems({
        "Не сортировано", "По возрастанию даты", "По убыванию даты", "По возрастанию суммы", "По убыванию суммы",
        "Не задано", "Продукты", "Жильё", "Транспорт", "Развлечения"
    });

    sortGroupBox_ = new QGroupBox(this);
    QVBoxLayout *sortLayout_ = new QVBoxLayout(sortGroupBox_);
    sortLayout_->addWidget(new QLabel("Выберите способ сортировки:"));
    sortLayout_->addWidget(chooseSortComboBox_);

    operationStatusBar_ = new QStatusBar(this);

    mainLayout_ = new QVBoxLayout(this);

    QHBoxLayout *expenceMonitorLayout_ = new QHBoxLayout();

    expenceMonitorLayout_->addWidget(budgetSetGroupBox_);
    expenceMonitorLayout_->addWidget(changeExpenceGroupBox_);
    expenceMonitorLayout_->addWidget(sortGroupBox_);

    mainLayout_->addWidget(addPushButton_);
    mainLayout_->addWidget(expenceTable_);
    mainLayout_->addWidget(budgetSpentProgressBar_);
    mainLayout_->addLayout(expenceMonitorLayout_);
    mainLayout_->addWidget(operationStatusBar_);

    setLayout(mainLayout_);

    connect(addPushButton_, &QPushButton::clicked, this, &ExpenceApp::onAddClicked);
    connect(expenceTable_, &QTableWidget::cellClicked, this, &ExpenceApp::onExpenceSelected);
    connect(budgetSetSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &ExpenceApp::onBudgetChanged);
    connect(changeExpenceButton_, &QPushButton::clicked, this, &ExpenceApp::changeExpence);
    connect(clearExpenceButton_, &QPushButton::clicked, this, &ExpenceApp::clearExpences);
    connect(chooseSortComboBox_, &QComboBox::textActivated, this,
            &ExpenceApp::onChooseSortComboBox);
}

void ExpenceApp::onAddClicked() {
    ExpenceAddWindow addWindow;

    if (addWindow.exec() == QDialog::Accepted) {
        double value = addWindow.getSum();
        QString category = addWindow.getCategory();
        QDate date = addWindow.getDate();
        QString info = addWindow.getInfo();

        budgetSpent_ += value;
        if (budgetSpent_ > budget_) {
            operationStatusBar_->showMessage(
                "Бюджет превышен на " + QString::number(budgetSpent_ - budget_, 'f', 2) + " BYN");
        }
        budgetSpentProgressBar_->updateProgress(budgetSpent_, budget_);

        clearExpenceButton_->setEnabled(true);

        totalExpences_++;
        expenceTable_->setRowCount(totalExpences_);
        expenceTable_->setItem(totalExpences_ - 1, 0, new QTableWidgetItem(date.toString("dd.MM.yyyy")));
        expenceTable_->setItem(totalExpences_ - 1, 1, new QTableWidgetItem(category));
        expenceTable_->setItem(totalExpences_ - 1, 2, new QTableWidgetItem(QString::number(value, 'f', 2)));
        expenceTable_->setItem(totalExpences_ - 1, 3, new QTableWidgetItem(info));

        QColor rowColor;
        if (category == "Продукты") {
            rowColor = QColor(144, 238, 144); // Светло-зеленый
        } else if (category == "Жильё") {
            rowColor = QColor(173, 216, 230); // Светло-голубой
        } else if (category == "Транспорт") {
            rowColor = QColor(255, 223, 0); // Золотой
        } else if (category == "Развлечения") {
            rowColor = QColor(255, 182, 193); // Розовый
        } else {
            rowColor = QColor(211, 211, 211); // Серый
        }

        for (int col = 0; col < expenceTable_->columnCount(); ++col) {
            QTableWidgetItem *item = expenceTable_->item(totalExpences_ - 1, col);
            if (item) {
                item->setBackground(rowColor);
            }
        }

        sort();
    }
}

void ExpenceChangeWindow::onDeleteButtonClicked() {
    emit deleteRequested();
    reject();
}

void ExpenceApp::onBudgetChanged(double value) {
    budget_ = value;
    budgetSpentProgressBar_->updateProgress(budgetSpent_, budget_);
    operationStatusBar_->showMessage("Бюджет установлен: " + QString::number(value, 'f', 2) + " BYN");
}

void ExpenceApp::clearExpences() {
    totalExpences_ = 0;
    budgetSpent_ = 0;
    expenceTable_->clear();
    expenceTable_->setRowCount(0);
    budgetSpentProgressBar_->updateProgress(0, budget_);
    operationStatusBar_->showMessage("Вся информация очищены");
    clearExpenceButton_->setEnabled(false);
    changeExpenceButton_->setEnabled(false);
}

void ExpenceApp::onExpenceSelected(int row, int column) {
    changeExpenceButton_->setEnabled(true);
}

void ExpenceApp::changeExpence() {
    int row = expenceTable_->currentRow();

    ExpenceChangeWindow changeWindow;

    double currentSum = expenceTable_->item(row, 2)->text().toDouble();
    QString currentCategory = expenceTable_->item(row, 1)->text();
    int categoryIndex = 0;
    if (currentCategory == "Продукты") {
        categoryIndex = 1;
    }
    if (currentCategory == "Жильё") {
        categoryIndex = 2;
    }
    if (currentCategory == "Транспорт") {
        categoryIndex = 3;
    }
    if (currentCategory == "Развлечения") {
        categoryIndex = 4;
    }
    QDate currentDate = QDate::fromString(expenceTable_->item(row, 0)->text(), "dd.MM.yyyy");
    QString currentInfo = expenceTable_->item(row, 3)->text();

    changeWindow.setValues(currentSum, categoryIndex, currentDate, currentInfo);

    connect(&changeWindow, &ExpenceChangeWindow::deleteRequested, this, [this, row, currentSum]() {
        expenceTable_->removeRow(row);
        totalExpences_--;
        budgetSpent_ -= currentSum;
        budgetSpentProgressBar_->updateProgress(budgetSpent_, budget_);
        operationStatusBar_->showMessage("Трата успешно удалена");

        if (totalExpences_ == 0) {
            clearExpenceButton_->setEnabled(false);
            changeExpenceButton_->setEnabled(false);
        }
    });

    if (changeWindow.exec() == QDialog::Accepted) {
        double newSum = changeWindow.getSum();
        QString newCategory = changeWindow.getCategory();
        QDate newDate = changeWindow.getDate();
        QString newInfo = changeWindow.getInfo();

        expenceTable_->setItem(row, 0, new QTableWidgetItem(newDate.toString("dd.MM.yyyy")));
        expenceTable_->setItem(row, 1, new QTableWidgetItem(newCategory));
        expenceTable_->setItem(row, 2, new QTableWidgetItem(QString::number(newSum, 'f', 2)));
        expenceTable_->setItem(row, 3, new QTableWidgetItem(newInfo));

        QColor rowColor;
        if (newCategory == "Продукты") {
            rowColor = QColor(144, 238, 144); // Светло-зеленый
        } else if (newCategory == "Жильё") {
            rowColor = QColor(173, 216, 230); // Светло-голубой
        } else if (newCategory == "Транспорт") {
            rowColor = QColor(255, 223, 0); // Золотой
        } else if (newCategory == "Развлечения") {
            rowColor = QColor(255, 182, 193); // Розовый
        } else {
            rowColor = QColor(211, 211, 211); // Серый
        }

        for (int col = 0; col < expenceTable_->columnCount(); ++col) {
            QTableWidgetItem *item = expenceTable_->item(row, col);
            if (item) {
                item->setBackground(rowColor);
            }
        }

        budgetSpent_ -= currentSum;
        budgetSpent_ += newSum;
        budgetSpentProgressBar_->updateProgress(budgetSpent_, budget_);
        operationStatusBar_->showMessage("Информация успешно изменена");
    }
}

void ExpenceApp::onChooseSortComboBox() {
    sortType = chooseSortComboBox_->currentText();
    operationStatusBar_->showMessage("Установлен вид сортировки: " + sortType);
    sort();
}

void ExpenceApp::sort() {
    if (totalExpences_ == 0 || sortType == "Не сортировано") {
        return;
    }
    QList<QList<QTableWidgetItem *> > rows;
    for (int row = 0; row < totalExpences_; ++row) {
        QList<QTableWidgetItem *> rowData;
        for (int col = 0; col < expenceTable_->columnCount(); ++col) {
            rowData.append(expenceTable_->takeItem(row, col));
        }
        rows.append(rowData);
    }
    if (sortType == "Не задано") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem *> &a, const QList<QTableWidgetItem *> &b) {
            QString categoryA = a[1]->text();
            QString categoryB = b[1]->text();
            QMap<QString, int> priority = {
                {"Не задано", 0},
                {"Продукты", 1},
                {"Жильё", 2},
                {"Транспорт", 3},
                {"Развлечения", 4}
            };
            return priority[categoryA] < priority[categoryB];
        });
    }
    if (sortType == "Продукты") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem *> &a, const QList<QTableWidgetItem *> &b) {
            QString categoryA = a[1]->text();
            QString categoryB = b[1]->text();
            QMap<QString, int> priority = {
                {"Продукты", 0},
                {"Жильё", 1},
                {"Транспорт", 2},
                {"Развлечения", 3},
                {"Не задано", 4}
            };
            return priority[categoryA] < priority[categoryB];
        });
    }
    if (sortType == "Жильё") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem *> &a, const QList<QTableWidgetItem *> &b) {
            QString categoryA = a[1]->text();
            QString categoryB = b[1]->text();
            QMap<QString, int> priority = {
                {"Жильё", 0},
                {"Продукты", 1},
                {"Транспорт", 2},
                {"Развлечения", 3},
                {"Не задано", 4}
            };
            return priority[categoryA] < priority[categoryB];
        });
    }
    if (sortType == "Транспорт") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem *> &a, const QList<QTableWidgetItem *> &b) {
            QString categoryA = a[1]->text();
            QString categoryB = b[1]->text();
            QMap<QString, int> priority = {
                {"Транспорт", 0},
                {"Продукты", 1},
                {"Жильё", 2},
                {"Развлечения", 3},
                {"Не задано", 4}
            };
            return priority[categoryA] < priority[categoryB];
        });
    }
    if (sortType == "Развлечения") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem *> &a, const QList<QTableWidgetItem *> &b) {
            QString categoryA = a[1]->text();
            QString categoryB = b[1]->text();
            QMap<QString, int> priority = {
                {"Развлечения", 0},
                {"Продукты", 1},
                {"Жильё", 2},
                {"Транспорт", 3},
                {"Не задано", 4}
            };
            return priority[categoryA] < priority[categoryB];
        });
    }
    if (sortType == "По возрастанию даты") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem *> &a, const QList<QTableWidgetItem *> &b) {
            QDate dateA = QDate::fromString(a[0]->text(), "dd.MM.yyyy");
            QDate dateB = QDate::fromString(b[0]->text(), "dd.MM.yyyy");
            return dateA < dateB;
        });
    }
    if (sortType == "По убыванию даты") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem *> &a, const QList<QTableWidgetItem *> &b) {
            QDate dateA = QDate::fromString(a[0]->text(), "dd.MM.yyyy");
            QDate dateB = QDate::fromString(b[0]->text(), "dd.MM.yyyy");
            return dateA > dateB;
        });
    }
    if (sortType == "По возрастанию суммы") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem *> &a, const QList<QTableWidgetItem *> &b) {
            double valueA = a[2]->text().toDouble();
            double valueB = b[2]->text().toDouble();
            return valueA < valueB;
        });
    }
    if (sortType == "По убыванию суммы") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem *> &a, const QList<QTableWidgetItem *> &b) {
            double valueA = a[2]->text().toDouble();
            double valueB = b[2]->text().toDouble();
            return valueA > valueB;
        });
    }
    for (int row = 0; row < rows.size(); ++row) {
        for (int col = 0; col < rows[row].size(); ++col) {
            expenceTable_->setItem(row, col, rows[row][col]);
        }
    }
}