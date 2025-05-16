#include "progressbar.h"
#include <QDebug>

ProgressBar::ProgressBar(QWidget *parent) : QWidget(parent) {
    layout_ = new QHBoxLayout(this);
    layout_->setSpacing(5);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_);

    // Инициализируем 5 блоков (для 5 вопросов)
    for (int i = 0; i < 5; ++i) {
        QLabel *block = new QLabel(this);
        block->setFixedSize(20, 20);
        block->setStyleSheet("background-color: gray; border: 1px solid black;");
        blocks_.push_back(block);
        layout_->addWidget(block);
    }
    layout_->addStretch();
}

void ProgressBar::setAnswers(const std::vector<Controller::AnswerType> &answers) {
    if (answers.size() > blocks_.size()) {
        qDebug() << "Предупреждение: размер answers (" << answers.size() << ") превышает количество блоков (" << blocks_.size() << ")";
        return;
    }
    updateBlocks(answers);
}

void ProgressBar::updateBlocks(const std::vector<Controller::AnswerType> &answers) {
    for (size_t i = 0; i < blocks_.size(); ++i) {
        QString style = "border: 1px solid black;";
        if (i < answers.size()) {
            switch (answers[i]) {
                case Controller::AnswerType::Right:
                    style = "background-color: green; " + style;
                    break;
                case Controller::AnswerType::Wrong:
                    style = "background-color: red; " + style;
                    break;
                case Controller::AnswerType::NoAnswer:
                    style = "background-color: gray; " + style;
                    break;
            }
        } else {
            style = "background-color: gray; " + style;
        }
        blocks_[i]->setStyleSheet(style);
    }
}
