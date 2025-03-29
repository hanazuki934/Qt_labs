//
// Created by andrei on 3/12/25.
//

#ifndef EXPENCES_H
#define EXPENCES_H

#include <QWidget>
#include <QTableWidget>
#include <QProgressBar>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QDialog>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTableWidgetItem>
#include <QWidget>
#include <QHeaderView>
#include <QRandomGenerator>
#include <QDebug>

class ExpenceProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit ExpenceProgressBar(QWidget *parent = nullptr);
    void updateProgress(double expence, double budget);
};

class ExpenceAddWindow : public QDialog {
    Q_OBJECT

public:
    explicit ExpenceAddWindow(QWidget* parent = nullptr);
    double getSum();
    QString getCategory();
    QDate getDate();
    QString getInfo();

private:
    friend class ExpenceApp;

    QLabel* infoLabel_;
    QDoubleSpinBox* addSumSpinBox_;
    QComboBox* addCategoryComboBox_;
    QDateEdit* addDateEdit_;
    QPushButton* addExpenceButton_;
    QLineEdit* addInfoLineEdit_;
    QGridLayout* mainLayout_;
};

class ExpenceChangeWindow : public QDialog {
    Q_OBJECT

public:
    explicit ExpenceChangeWindow(QWidget* parent = nullptr);
    void setValues(int sum, int index, QDate date, QString info);
    double getSum();
    QString getCategory();
    QDate getDate();
    QString getInfo();

private slots:
    void onDeleteButtonClicked();

signals:
    void deleteRequested();

private:
    friend class ExpenceApp;

    QVBoxLayout* mainLayout_;
    QPushButton* confirmChangePushButton_;
    QPushButton* deletePushButton_;
    QGroupBox* changeExpenceGroupBox_;
    QDoubleSpinBox* changeSumSpinBox_;
    QComboBox* changeCategoryComboBox_;
    QDateEdit* changeDateEdit_;
    QLineEdit* changeInfoLineEdit_;
};

class ExpenceApp : public QWidget
{
    Q_OBJECT

public:
    explicit ExpenceApp(QWidget* parent = nullptr);

private slots:
    void onAddClicked();
    void onExpenceSelected(int row, int column);
    void onBudgetChanged(double value);
    void changeExpence();
    void clearExpences();
    void onChooseSortComboBox();

private:
    friend class ExpenceAddBox;
    double budget_ = 0;
    double budgetSpent_ = 0;
    int totalExpences_ = 0;
    QString sortType = {};

    void sort();

    QVBoxLayout* mainLayout_;

    QPushButton* addPushButton_;

    QTableWidget* expenceTable_;
    ExpenceProgressBar* budgetSpentProgressBar_;
    QStatusBar* operationStatusBar_;

    QDoubleSpinBox* budgetSetSpinBox_;
    QGroupBox* budgetSetGroupBox_;

    QPushButton* changeExpenceButton_;
    QPushButton* clearExpenceButton_;
    QGroupBox* changeExpenceGroupBox_;

    QComboBox* chooseSortComboBox_;
    QGroupBox* sortGroupBox_;
};

#endif //EXPENCES_H
