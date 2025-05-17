#ifndef TRANSLATIONWIDGET_H
#define TRANSLATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "controller.h"
#include "progressbar.h"

class TranslationRuToEnWidget : public QWidget {
    Q_OBJECT

public:
    explicit TranslationRuToEnWidget(QWidget *parent = nullptr, Controller *controller = nullptr);

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

    std::vector<Controller::TranslationQuestion> question_set_{};
    Controller::TranslationQuestion current_question_{};

    Controller::TestStats test_stats_{};

    void ToNextQuestion();
};

class TranslationEnToRuWidget : public QWidget {
    Q_OBJECT

public:
    explicit TranslationEnToRuWidget(QWidget *parent = nullptr, Controller *controller = nullptr);

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

    std::vector<Controller::TranslationQuestion> question_set_{};
    Controller::TranslationQuestion current_question_{};

    Controller::TestStats test_stats_{};

    void ToNextQuestion();
};

#endif // TRANSLATIONWIDGET_H