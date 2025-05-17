#ifndef GRAMMARWIDGET_H
#define GRAMMARWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <qlineedit.h>

#include "controller.h"
#include "progressbar.h"

class GrammarTestWidget : public QWidget {
    Q_OBJECT

public:
    explicit GrammarTestWidget(QWidget *parent = nullptr, Controller *controller = nullptr);

    void UpdateTest();
    [[nodiscard]] QSize sizeHint() const override;

signals:
    void exitRequested();

private slots:
    void onOptionClicked(int id);
    void OnExitClicked();
    void keyPressEvent(QKeyEvent *event) override;

private:
    Controller *controller_{};

    ProgressBar *progress_bar_{};
    QLabel *test_number_{};
    QLabel *question_label_{};
    QRadioButton *option1_button_{};
    QRadioButton *option2_button_{};
    QRadioButton *option3_button_{};
    QRadioButton *option4_button_{};
    QButtonGroup *button_group_{};
    QPushButton *submit_button_{};
    QLabel *timer_label_{};
    QPushButton *exit_button_{};
    QHBoxLayout *test_layout_{};
    QHBoxLayout *button_layout_{};
    QVBoxLayout *layout_{};

    QTimer *timer_{};

    std::vector<Controller::GrammarQuestion> question_set_{};
    Controller::GrammarQuestion question_{};

    Controller::TestStats test_stats_{};

    Controller::GrammarQuestion current_question_{};

    void ToNextQuestion();
};

class GrammarQuestionWidget : public QWidget {
    Q_OBJECT

public:
    explicit GrammarQuestionWidget(QWidget *parent = nullptr, Controller *controller = nullptr);

    void UpdateTest();
    [[nodiscard]] QSize sizeHint() const override;

    signals:
        void exitRequested();

private slots:
    void onSubmitClicked();
    void OnExitClicked();
    void keyPressEvent(QKeyEvent *event) override;

private:
    Controller *controller_{};

    ProgressBar *progress_bar_{};
    QLabel *test_number_{};
    QLabel *question_label_{};
    QLineEdit *answer_edit_{};
    QPushButton *submit_button_{};
    QLabel *timer_label_{};
    QPushButton *exit_button_{};
    QHBoxLayout *test_layout_{};
    QHBoxLayout *button_layout_{};
    QVBoxLayout *layout_{};

    QTimer *timer_{};

    std::vector<Controller::GrammarQuestion> question_set_{};
    Controller::GrammarQuestion question_{};

    Controller::TestStats test_stats_{};

    Controller::GrammarQuestion current_question_{};

    void ToNextQuestion();
};

#endif // GRAMMARWIDGET_H
