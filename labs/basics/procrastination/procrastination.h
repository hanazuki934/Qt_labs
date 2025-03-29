//
// Created by andrei on 3/12/25.
//

#ifndef PROCRASTINATION_H
#define PROCRASTINATION_H

#include <QWidget>
#include <QTableWidget>
#include <QProgressBar>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTableWidgetItem>
#include <QWidget>
#include <QHeaderView>
#include <QRandomGenerator>
#include <QDebug>

class StackChosen {
   public:
    explicit StackChosen() = default;

    void stackPush(int index);

    void stackClear();

    int getStackElement(int index);

    int size();

    int currentPosition = -1;
    int lastResetPosition = -1;

   private:
    QVector<int> stack = {};
};

class TotalProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit TotalProgressBar(QWidget *parent = nullptr);
    void updateProgress(int progress);

private:
    void updateColor(int progress);
};

class GreenProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit GreenProgressBar(QWidget *parent = nullptr);
    void updateProgress(int progress);

private:
    void updateColor(int progress);
};

class QuestionViewGroupBox : public QGroupBox {
    Q_OBJECT

   public:
    explicit QuestionViewGroupBox(QWidget* parent = nullptr);
    void setTicketInfo(int number, QString name, QString status);

   private:
    friend class TicketApp;
    int chosenTicket_ = 0;
    StackChosen stackChosen;

    QLabel* ticketNumber_;
    QLabel* ticketName_;
    QComboBox* ticketBoxStatus_;

    QLineEdit* ticketNameEdit_;
};

class TicketApp : public QWidget
{
    Q_OBJECT

public:
        explicit TicketApp(QWidget* parent = nullptr);

public slots:
    void onUpdateTicketName();


private slots:
    friend class QuestionViewGroupBox;
    void onGenerateTickets();
    void resetTickets();
    void toNextTicket();
    void toPrevTicket();
    void onTicketSelected(int row, int column);
    void onChangeStatus(int row, int column);
    void onChangeStatusThroughComboBox();

private:
    friend class QuestionViewGroupBox;
    int totalTickets_;
    int totalProgress_;
    int greenProgress_;

    QSpinBox* ticketCountSpinBox_;
    QPushButton* resetButton_;
    QPushButton* prevQuestionButton_;
    QPushButton* nextQuestionButton_;
    TotalProgressBar* progressBarTotal_;
    GreenProgressBar* progressBarGreen_;
    QTableWidget* ticketTable_;
    QuestionViewGroupBox* QuestionViewGroup_;
};

#endif //PROCRASTINATION_H
