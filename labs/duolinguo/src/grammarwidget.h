#ifndef GRAMMARTESTWIDGET_H
#define GRAMMARTESTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QButtonGroup>

#include "controller.h"

class GrammarTestWidget : public QWidget {
    Q_OBJECT

public:
    explicit GrammarTestWidget(QWidget *parent = nullptr, Controller *controller = nullptr);

    [[nodiscard]] QSize sizeHint() const override;

signals:
    void exitRequested();

private slots:
    void onOptionClicked(int id);
    void OnExitClicked();

private:
    Controller *controller_;

    QLabel *question_label_{};
    QRadioButton *option1_button_{};
    QRadioButton *option2_button_{};
    QRadioButton *option3_button_{};
    QRadioButton *option4_button_{};
    QButtonGroup *button_group_{};
    QPushButton *exit_button_{};
    QVBoxLayout *layout_{};

    void updateQuestion();
};

#endif // GRAMMARTESTWIDGET_H
