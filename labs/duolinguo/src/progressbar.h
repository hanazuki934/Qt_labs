#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QLabel>
#include <QHBoxLayout>

#include "controller.h"

class ProgressBar : public QWidget {
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = nullptr);
    void setAnswers(const std::vector<Controller::AnswerType> &answers);

private:
    QHBoxLayout *layout_{};
    std::vector<QLabel*> blocks_{};
    void updateBlocks(const std::vector<Controller::AnswerType> &answers);
};

#endif // PROGRESSBAR_H
