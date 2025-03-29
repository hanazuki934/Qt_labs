#include "procrastination.h"

void StackChosen::stackPush(int index) {
    stack.append(index);
}
int StackChosen::getStackElement(int index) {
    return stack[index];
}

int StackChosen::size() {
    return stack.size();
}

void StackChosen::stackClear() {
    stack.clear();
    currentPosition = -1;
    lastResetPosition = -1;
}

TotalProgressBar::TotalProgressBar(QWidget *parent)
    : QProgressBar(parent) {
    setRange(0, 100);
    setFormat(QString("Просмотрено - %1%. Осталось - %2%").arg(0).arg(100));
    setAlignment(Qt::AlignCenter);
}

void TotalProgressBar::updateProgress(int progress) {
    setValue(progress);
    setFormat(QString("Просмотрено - %1%. Осталось - %2%").arg(progress).arg(100 - progress));
    updateColor(progress);
}

void TotalProgressBar::updateColor(int progress) {
    setStyleSheet("QProgressBar::chunk { background-color: gray; }");
}

GreenProgressBar::GreenProgressBar(QWidget *parent)
    : QProgressBar(parent) {
    setRange(0, 100);
    setFormat(QString("Изучено - %1%. Осталось - %2%").arg(0).arg(100));
    setAlignment(Qt::AlignCenter);
}

void GreenProgressBar::updateProgress(int progress) {
    setValue(progress);
    setFormat(QString("Изучено - %1%. Осталось - %2%").arg(progress).arg(100 - progress));
    updateColor(progress);
}

void GreenProgressBar::updateColor(int progress) {
    if (progress <= 33) {
        setStyleSheet("QProgressBar::chunk { background-color: red; }");
    } else if (progress <= 66) {
        setStyleSheet("QProgressBar::chunk { background-color: yellow; }");
    } else {
        setStyleSheet("QProgressBar::chunk { background-color: green; }");
    }
}

QuestionViewGroupBox::QuestionViewGroupBox(QWidget *parent) : QGroupBox("Информация о билете", parent) {
    QGridLayout *mainLayout = new QGridLayout(this);

    ticketNumber_ = new QLabel("Номер билета: ", this);
    ticketName_ = new QLabel("Название: ", this);
    ticketBoxStatus_ = new QComboBox(this);
    ticketBoxStatus_->addItems({"Не начато", "В процессе", "Завершено"});
    ticketNameEdit_ = new QLineEdit(this);
    ticketNameEdit_->setPlaceholderText("Введите название билета");

    mainLayout->addWidget(ticketNumber_);
    mainLayout->addWidget(ticketName_);
    mainLayout->addWidget(ticketBoxStatus_);
    mainLayout->addWidget(ticketNameEdit_);
}

TicketApp::TicketApp(QWidget *parent)
    : QWidget(parent), totalTickets_(0), totalProgress_(0), greenProgress_(0) {
    setWindowTitle("The greatest exam revising app of the all times");

    ticketCountSpinBox_ = new QSpinBox(this);
    ticketCountSpinBox_->setMinimum(0);
    ticketCountSpinBox_->setMaximum(100);

    resetButton_ = new QPushButton("Сбросить прогресс", this);
    prevQuestionButton_ = new QPushButton("Предыдущий вопрос", this);
    prevQuestionButton_->setEnabled(false);
    nextQuestionButton_ = new QPushButton("Следующий вопрос", this);
    nextQuestionButton_->setEnabled(false);

    progressBarTotal_ = new TotalProgressBar(this);
    progressBarGreen_ = new GreenProgressBar(this);

    ticketTable_ = new QTableWidget(this);
    ticketTable_->setColumnCount(3);
    ticketTable_->setHorizontalHeaderLabels(QStringList() << "Билет" << "Статус" << "Информация");
    ticketTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    ticketTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ticketTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ticketTable_->horizontalHeader()->setStretchLastSection(true);

    QuestionViewGroup_ = new QuestionViewGroupBox(this);

    resize(800, 600);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *ticketLayout = new QHBoxLayout();
    ticketLayout->addWidget(ticketCountSpinBox_);
    ticketLayout->addWidget(resetButton_);
    ticketLayout->addWidget(nextQuestionButton_);
    ticketLayout->addWidget(prevQuestionButton_);
    mainLayout->addWidget(QuestionViewGroup_);
    mainLayout->addWidget(ticketTable_);
    mainLayout->addWidget(progressBarTotal_);
    mainLayout->addWidget(progressBarGreen_);
    mainLayout->addLayout(ticketLayout);

    setLayout(mainLayout);

    connect(ticketCountSpinBox_, &QSpinBox::textChanged, this, &TicketApp::onGenerateTickets);
    connect(resetButton_, &QPushButton::clicked, this, &TicketApp::resetTickets);
    connect(nextQuestionButton_, &QPushButton::clicked, this, &TicketApp::toNextTicket);
    connect(prevQuestionButton_, &QPushButton::clicked, this, &TicketApp::toPrevTicket);
    connect(ticketTable_, &QTableWidget::cellClicked, this, &TicketApp::onTicketSelected);
    connect(ticketTable_, &QTableWidget::cellDoubleClicked, this, &TicketApp::onChangeStatus);
    connect(QuestionViewGroup_->ticketBoxStatus_, &QComboBox::textActivated, this,
            &TicketApp::onChangeStatusThroughComboBox);
    connect(QuestionViewGroup_->ticketNameEdit_, &QLineEdit::editingFinished, this, &TicketApp::onUpdateTicketName);
}

void TicketApp::onGenerateTickets() {
    totalTickets_ = ticketCountSpinBox_->value();
    totalProgress_ = 0;
    greenProgress_ = 0;

    prevQuestionButton_->setEnabled(false);

    if (totalTickets_ != 0) {
        nextQuestionButton_->setEnabled(true);
    } else {
        nextQuestionButton_->setEnabled(false);
    }

    QuestionViewGroup_->ticketNameEdit_->clear();
    QuestionViewGroup_->ticketNumber_->setText("Номер билета: ");
    QuestionViewGroup_->ticketName_->setText("Название: ");
    QuestionViewGroup_->chosenTicket_ = 0;
    QuestionViewGroup_->stackChosen.stackClear();

    ticketTable_->setRowCount(totalTickets_);
    for (int i = 0; i < totalTickets_; ++i) {
        ticketTable_->setItem(i, 0, new QTableWidgetItem("Билет " + QString::number(i + 1)));
        QTableWidgetItem *statusItem = new QTableWidgetItem("Не начато");
        statusItem->setBackground(Qt::gray);
        ticketTable_->setItem(i, 1, statusItem);
    }

    progressBarTotal_->updateProgress(0);
    progressBarGreen_->updateProgress(0);
}

void TicketApp::resetTickets() {
    totalProgress_ = 0;
    greenProgress_ = 0;

    prevQuestionButton_->setEnabled(false);

    if (totalTickets_ != 0) {
        nextQuestionButton_->setEnabled(true);
    }

    QuestionViewGroup_->ticketNameEdit_->clear();
    QuestionViewGroup_->ticketNumber_->setText("Номер билета: ");
    QuestionViewGroup_->ticketName_->setText("Название: ");
    QuestionViewGroup_->chosenTicket_ = 0;
    QuestionViewGroup_->stackChosen.currentPosition = QuestionViewGroup_->stackChosen.size() - 1;
    QuestionViewGroup_->stackChosen.lastResetPosition = QuestionViewGroup_->stackChosen.size() - 1;

    for (int i = 0; i < totalTickets_; ++i) {
        QTableWidgetItem *statusItem = new QTableWidgetItem("Не начато");
        statusItem->setBackground(Qt::gray);
        ticketTable_->setItem(i, 1, statusItem);
    }

    progressBarTotal_->updateProgress(0);
    progressBarGreen_->updateProgress(0);
}

void TicketApp::onChangeStatus(int row, int column) {
    if (column == 1) {
        QTableWidgetItem *statusItem = ticketTable_->item(row, 1);
        QString currentStatus = statusItem->text();
        QTableWidgetItem *nameItem = ticketTable_->item(row, 0);
        QString currentName = nameItem->text();

        if (currentStatus == "Не начато") {
            statusItem->setText("Завершено");
            statusItem->setBackground(Qt::green);
            QuestionViewGroup_->setTicketInfo(row + 1, currentName, "Завершено");
            totalProgress_++;
            greenProgress_++;
            progressBarTotal_->updateProgress((totalProgress_ * 100) / totalTickets_);
            progressBarGreen_->updateProgress((greenProgress_ * 100) / totalTickets_);
            if (greenProgress_ == totalTickets_) {
                nextQuestionButton_->setEnabled(false);
            }
        } else if (currentStatus == "В процессе") {
            statusItem->setText("Завершено");
            statusItem->setBackground(Qt::green);
            QuestionViewGroup_->setTicketInfo(row + 1, currentName, "Завершено");
            greenProgress_++;
            progressBarGreen_->updateProgress((greenProgress_ * 100) / totalTickets_);
            if (greenProgress_ == totalTickets_) {
                nextQuestionButton_->setEnabled(false);
            }
        } else if (currentStatus == "Завершено") {
            statusItem->setText("В процессе");
            statusItem->setBackground(Qt::yellow);
            QuestionViewGroup_->setTicketInfo(row + 1, currentName, "В процессе");
            greenProgress_--;
            progressBarGreen_->updateProgress((greenProgress_ * 100) / totalTickets_);
            if (greenProgress_ != totalTickets_) {
                nextQuestionButton_->setEnabled(true);
            }
        }
    }
}

void TicketApp::onChangeStatusThroughComboBox() {
    if (QuestionViewGroup_->chosenTicket_ != 0) {
        QString statusToChange = QuestionViewGroup_->ticketBoxStatus_->currentText();
        int row = QuestionViewGroup_->chosenTicket_ - 1;
        QTableWidgetItem *statusItem = ticketTable_->item(row, 1);
        QString currentStatus = statusItem->text();
        QTableWidgetItem *nameItem = ticketTable_->item(row, 0);
        QString currentName = nameItem->text();

        if (currentStatus == statusToChange) {
            return;
        }
        if (currentStatus == "Не начато" && statusToChange == "В процессе") {
            statusItem->setText("В процессе");
            statusItem->setBackground(Qt::yellow);
            QuestionViewGroup_->setTicketInfo(row + 1, currentName, "В процессе");
            totalProgress_++;
            progressBarTotal_->updateProgress((totalProgress_ * 100) / totalTickets_);
            progressBarGreen_->updateProgress((greenProgress_ * 100) / totalTickets_);
            if (greenProgress_ == totalTickets_) {
                nextQuestionButton_->setEnabled(false);
            }
        } else if (currentStatus == "В процессе" && statusToChange == "Не начато") {
            statusItem->setText("Не начато");
            statusItem->setBackground(Qt::gray);
            QuestionViewGroup_->setTicketInfo(row + 1, currentName, "Не начато");
            totalProgress_--;
            progressBarTotal_->updateProgress((totalProgress_ * 100) / totalTickets_);
            progressBarGreen_->updateProgress((greenProgress_ * 100) / totalTickets_);
            if (greenProgress_ == totalTickets_) {
                nextQuestionButton_->setEnabled(false);
            }
        } else if (currentStatus == "Не начато" && statusToChange == "Завершено") {
            statusItem->setText("Завершено");
            statusItem->setBackground(Qt::green);
            QuestionViewGroup_->setTicketInfo(row + 1, currentName, "Завершено");
            totalProgress_++;
            greenProgress_++;
            progressBarTotal_->updateProgress((totalProgress_ * 100) / totalTickets_);
            progressBarGreen_->updateProgress((greenProgress_ * 100) / totalTickets_);
            if (greenProgress_ == totalTickets_) {
                nextQuestionButton_->setEnabled(false);
            }
        } else if (currentStatus == "Завершено" && statusToChange == "Не начато") {
            statusItem->setText("Не начато");
            statusItem->setBackground(Qt::gray);
            QuestionViewGroup_->setTicketInfo(row + 1, currentName, "Не начато");
            totalProgress_--;
            greenProgress_--;
            progressBarTotal_->updateProgress((totalProgress_ * 100) / totalTickets_);
            progressBarGreen_->updateProgress((greenProgress_ * 100) / totalTickets_);
            if (greenProgress_ != totalTickets_) {
                nextQuestionButton_->setEnabled(true);
            }
        } else if (currentStatus == "В процессе" && statusToChange == "Завершено") {
            statusItem->setText("Завершено");
            statusItem->setBackground(Qt::green);
            QuestionViewGroup_->setTicketInfo(row + 1, currentName, "Завершено");
            greenProgress_++;
            progressBarTotal_->updateProgress((totalProgress_ * 100) / totalTickets_);
            progressBarGreen_->updateProgress((greenProgress_ * 100) / totalTickets_);
            if (greenProgress_ == totalTickets_) {
                nextQuestionButton_->setEnabled(false);
            }
        } else if (currentStatus == "Завершено" && statusToChange == "В процессе") {
            statusItem->setText("В процессе");
            statusItem->setBackground(Qt::yellow);
            QuestionViewGroup_->setTicketInfo(row + 1, currentName, "В процессе");
            greenProgress_--;
            progressBarTotal_->updateProgress((totalProgress_ * 100) / totalTickets_);
            progressBarGreen_->updateProgress((greenProgress_ * 100) / totalTickets_);
            if (greenProgress_ != totalTickets_) {
                nextQuestionButton_->setEnabled(true);
            }
        }
    }
}

void TicketApp::toNextTicket() {
    int cntAvalible = 0;

    for (int i = 0; i < totalTickets_; ++i) {
        if (ticketTable_->item(i, 1)->text() != "Завершено") {
            cntAvalible++;
        }
    }

    if (cntAvalible == 1) {
        int position = -1;
        for (int i = 0; i < totalTickets_; ++i) {
            if (ticketTable_->item(i, 1)->text() != "Завершено") {
                position = i;
                break;
            }
        }


        if (position != -1) {
            QuestionViewGroup_->stackChosen.currentPosition = QuestionViewGroup_->stackChosen.size();
            QuestionViewGroup_->setTicketInfo(position + 1,
                                              ticketTable_->item(position, 0)->text(),
                                              ticketTable_->item(position, 1)->text());
            if (QuestionViewGroup_->stackChosen.currentPosition - QuestionViewGroup_->stackChosen.lastResetPosition > 1) {
                prevQuestionButton_->setEnabled(true);
            }
            ticketTable_->selectRow(position);
        } else {
            qDebug() << "Ошибка: не найден единственный доступный билет!";
        }

    } else if (cntAvalible == 0) {
        nextQuestionButton_->setEnabled(false);

    } else {
        QVector<int> availableTickets;
        for (int i = 0; i < totalTickets_; ++i) {
            if (ticketTable_->item(i, 1)->text() != "Завершено" &&
                QuestionViewGroup_->chosenTicket_ - 1 != i) {
                availableTickets.append(i);
            }
        }

        if (!availableTickets.isEmpty()) {
            int randomValue = QRandomGenerator::global()->bounded(availableTickets.size());
            int position = availableTickets[randomValue];


            ticketTable_->selectRow(position);
            QuestionViewGroup_->stackChosen.currentPosition = QuestionViewGroup_->stackChosen.size();
            QuestionViewGroup_->setTicketInfo(position + 1,
                                              ticketTable_->item(position, 0)->text(),
                                              ticketTable_->item(position, 1)->text());
            if (QuestionViewGroup_->stackChosen.currentPosition - QuestionViewGroup_->stackChosen.lastResetPosition > 1) {
                prevQuestionButton_->setEnabled(true);
            }
        } else {
            qDebug() << "Ошибка: нет доступных билетов для выбора!";
        }
    }
}

void TicketApp::toPrevTicket() {
    QuestionViewGroup_->stackChosen.currentPosition--;
    int to = QuestionViewGroup_->stackChosen.getStackElement(QuestionViewGroup_->stackChosen.currentPosition);
    QuestionViewGroup_->setTicketInfo(to, ticketTable_->item(to - 1, 0)->text(),
                                          ticketTable_->item(to - 1, 1)->text());
    ticketTable_->selectRow(to - 1);
    if (QuestionViewGroup_->stackChosen.currentPosition - QuestionViewGroup_->stackChosen.lastResetPosition <= 1) {
        prevQuestionButton_->setEnabled(false);
    }
}

void TicketApp::onTicketSelected(int row, int column) {
    QString ticketName = ticketTable_->item(row, 0)->text();
    QString ticketStatus = ticketTable_->item(row, 1)->text();
    QuestionViewGroup_->stackChosen.currentPosition = QuestionViewGroup_->stackChosen.size();
    QuestionViewGroup_->setTicketInfo(row + 1, ticketName, ticketStatus);
    if (QuestionViewGroup_->stackChosen.currentPosition - QuestionViewGroup_->stackChosen.lastResetPosition > 1) {
        prevQuestionButton_->setEnabled(true);
    }
}

void QuestionViewGroupBox::setTicketInfo(int number, QString name, QString status) {
    ticketNameEdit_->clear();
    ticketNumber_->setText("Номер билета: " + QString::number(number));
    ticketName_->setText("Название: " + name);
    ticketNameEdit_->setText(name);
    ticketBoxStatus_->setCurrentText(status);
    chosenTicket_ = number;
    if (stackChosen.size() - 1 == stackChosen.lastResetPosition  || number != stackChosen.getStackElement(stackChosen.size() - 1)) {
        stackChosen.stackPush(chosenTicket_);
    }
}

void TicketApp::onUpdateTicketName() {
    if (QuestionViewGroup_->chosenTicket_ != 0) {
        ticketTable_->item(QuestionViewGroup_->chosenTicket_ - 1, 0)->setText(
            QuestionViewGroup_->ticketNameEdit_->text());
        QuestionViewGroup_->ticketName_->setText("Название: " + QuestionViewGroup_->ticketNameEdit_->text());
    }
}
