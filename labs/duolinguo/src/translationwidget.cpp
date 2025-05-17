#include "translationwidget.h"

#include <QMessageBox>
#include <QKeyEvent>
#include <QPushButton>
#include <QTimer>
#include <QLineEdit>
#include <QSize>
#include <qtmetamacros.h>

// TranslationRuToEnWidget Implementation

TranslationRuToEnWidget::TranslationRuToEnWidget(QWidget *parent, Controller *controller)
    : QWidget(parent), controller_(controller) {
    layout_ = new QVBoxLayout(this);

    progress_bar_ = new ProgressBar(this);
    test_number_ = new QLabel(this);
    question_label_ = new QLabel(this);
    answer_edit_ = new QLineEdit(this);
    submit_button_ = new QPushButton("Следующий вопрос", this);
    submit_button_->setDefault(true);
    exit_button_ = new QPushButton("Выход", this);
    timer_label_ = new QLabel("Время: 10:00", this);

    test_layout_ = new QHBoxLayout();
    test_layout_->addWidget(progress_bar_);
    test_layout_->addWidget(test_number_);

    button_layout_ = new QHBoxLayout();
    button_layout_->addWidget(timer_label_);
    button_layout_->addWidget(submit_button_);
    button_layout_->addWidget(exit_button_);

    layout_->addLayout(test_layout_);
    layout_->addWidget(question_label_);
    layout_->addWidget(answer_edit_);
    layout_->addLayout(button_layout_);
    setLayout(layout_);

    connect(submit_button_, &QPushButton::clicked, this, &TranslationRuToEnWidget::onSubmitClicked);
    connect(exit_button_, &QPushButton::clicked, this, &TranslationRuToEnWidget::OnExitClicked);
    connect(answer_edit_, &QLineEdit::returnPressed, this, &TranslationRuToEnWidget::onSubmitClicked);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [this]() {
        if (test_stats_.timeElapsed > 0) {
            test_stats_.timeElapsed--;
            int minutes = test_stats_.timeElapsed / 60;
            int seconds = test_stats_.timeElapsed % 60;
            timer_label_->setText(QString("Время: %1:%2")
                .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0')));
        } else {
            timer_->stop();
            timer_label_->setText("Время: 00:00");
            QMessageBox::warning(this, "Время истекло", "Время теста закончилось!");
            OnExitClicked();
        }
    });
}

void TranslationRuToEnWidget::UpdateTest() {
    test_stats_.Clear();
    test_stats_.difficulty = controller_->GetDifficulty();
    question_set_.clear();
    question_set_ = controller_->RequestTranslationQuestionSet(Controller::QuestionType::TranslationRuToEn,
                                                               test_stats_.difficulty, test_stats_);
    test_stats_.answers.resize(5, Controller::AnswerType::NoAnswer);
    test_stats_.timeElapsed = Controller::kTranslateTestDurationSeconds;
    timer_label_->setText(QString("Время: %1:%2")
        .arg(test_stats_.timeElapsed / 60, 2, 10, QChar('0'))
        .arg(test_stats_.timeElapsed % 60, 2, 10, QChar('0')));
    progress_bar_->setAnswers(test_stats_.answers);
    test_number_ = new QLabel(this);
    test_number_->setText(QString("Тест №%1").arg(test_stats_.testId));
    timer_->start(1000);
    if (test_stats_.testId == -1) {
        timer_->stop();
        QMessageBox::information(this, "Результат",
                                 "Все тесты данной темы и сложности пройдены");
        emit exitRequested();
    }
    ToNextQuestion();
}

void TranslationRuToEnWidget::ToNextQuestion() {
    if (test_stats_.questionsAnswered >= 5) {
        OnExitClicked();
        return;
    }
    current_question_ = question_set_[test_stats_.questionsAnswered];
    question_label_->setText(current_question_.source_text);

    qDebug() << "Получен вопрос из GetNextTranslationQuestion:";
    qDebug() << "type:" << static_cast<int>(current_question_.type);
    qDebug() << "source_text:" << current_question_.source_text;
    qDebug() << "correct_translations:" << current_question_.correct_translations;
    qDebug() << "hint:" << current_question_.hint;

    if (current_question_.type != Controller::QuestionType::TranslationRuToEn) {
        question_label_->setText("Этот вопрос не является заданием на перевод с русского на английский.");
        return;
    }

    answer_edit_->clear();
    answer_edit_->setFocus();
}

void TranslationRuToEnWidget::onSubmitClicked() {
    QString user_answer = answer_edit_->text().trimmed();
    if (user_answer.isEmpty()) {
        QMessageBox::warning(this, "Введите ответ", "Пожалуйста, введите перевод.");
        return;
    }

    bool is_correct = controller_->CheckAnswer(user_answer, current_question_.correct_translations);

    test_stats_.answers[test_stats_.questionsAnswered] = (is_correct ? Controller::AnswerType::Right : Controller::AnswerType::Wrong);
    test_stats_.questionsAnswered++;
    progress_bar_->setAnswers(test_stats_.answers);
    if (!is_correct) {
        test_stats_.mistakes++;
    }
    if (test_stats_.mistakes == Controller::kMistakesMax) {
        OnExitClicked();
        return;
    }

    ToNextQuestion();
}

void TranslationRuToEnWidget::OnExitClicked() {
    timer_->stop();
    int cnt_answered = 0;
    for (auto i: test_stats_.answers) {
        cnt_answered += (i == Controller::AnswerType::Right);
    }
    test_stats_.rightAnswers = cnt_answered;
    int minutes = test_stats_.timeElapsed / 60;
    int seconds = test_stats_.timeElapsed % 60;
    if (test_stats_.mistakes == Controller::kMistakesMax) {
        QMessageBox::information(this, "Результат",
                                 QString(
                                     "Выполнен тест №%1\nПравильных ответов: %2 из 5\nОставшееся время: %3:%4\nВы превысили допустимое число ошибок: %5")
                                 .arg(test_stats_.testId)
                                 .arg(cnt_answered)
                                 .arg(minutes, 2, 10, QChar('0'))
                                 .arg(seconds, 2, 10, QChar('0'))
                                 .arg(Controller::kMistakesMax));
    } else {
        QMessageBox::information(this, "Результат",
                                 QString("Выполнен тест №%1\nПравильных ответов: %2 из 5\nОставшееся время: %3:%4")
                                 .arg(test_stats_.testId)
                                 .arg(cnt_answered)
                                 .arg(minutes, 2, 10, QChar('0'))
                                 .arg(seconds, 2, 10, QChar('0')));
    }
    controller_->SendDataAboutTest(Controller::QuestionType::TranslationRuToEn, test_stats_.difficulty, test_stats_);
    emit exitRequested();
}

QSize TranslationRuToEnWidget::sizeHint() const {
    return {500, 250};
}

void TranslationRuToEnWidget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_H) {
        QString hint = current_question_.hint.isEmpty() ? "Подсказка недоступна." : current_question_.hint;
        QMessageBox::information(this, "Подсказка", hint);
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        onSubmitClicked();
    }
    QWidget::keyPressEvent(event);
}

// TranslationEnToRuWidget Implementation

TranslationEnToRuWidget::TranslationEnToRuWidget(QWidget *parent, Controller *controller)
    : QWidget(parent), controller_(controller) {
    layout_ = new QVBoxLayout(this);

    progress_bar_ = new ProgressBar(this);
    test_number_ = new QLabel(this);
    question_label_ = new QLabel(this);
    answer_edit_ = new QLineEdit(this);
    submit_button_ = new QPushButton("Следующий вопрос", this);
    submit_button_->setDefault(true);
    exit_button_ = new QPushButton("Выход", this);
    timer_label_ = new QLabel("Время: 10:00", this);

    test_layout_ = new QHBoxLayout();
    test_layout_->addWidget(progress_bar_);
    test_layout_->addWidget(test_number_);

    button_layout_ = new QHBoxLayout();
    button_layout_->addWidget(timer_label_);
    button_layout_->addWidget(submit_button_);
    button_layout_->addWidget(exit_button_);

    layout_->addLayout(test_layout_);
    layout_->addWidget(question_label_);
    layout_->addWidget(answer_edit_);
    layout_->addLayout(button_layout_);
    setLayout(layout_);

    connect(submit_button_, &QPushButton::clicked, this, &TranslationEnToRuWidget::onSubmitClicked);
    connect(exit_button_, &QPushButton::clicked, this, &TranslationEnToRuWidget::OnExitClicked);
    connect(answer_edit_, &QLineEdit::returnPressed, this, &TranslationEnToRuWidget::onSubmitClicked);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [this]() {
        if (test_stats_.timeElapsed > 0) {
            test_stats_.timeElapsed--;
            int minutes = test_stats_.timeElapsed / 60;
            int seconds = test_stats_.timeElapsed % 60;
            timer_label_->setText(QString("Время: %1:%2")
                .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0')));
        } else {
            timer_->stop();
            timer_label_->setText("Время: 00:00");
            QMessageBox::warning(this, "Время истекло", "Время теста закончилось!");
            OnExitClicked();
        }
    });
}

void TranslationEnToRuWidget::UpdateTest() {
    test_stats_.Clear();
    test_stats_.difficulty = controller_->GetDifficulty();
    question_set_.clear();
    question_set_ = controller_->RequestTranslationQuestionSet(Controller::QuestionType::TranslationEnToRu,
                                                               test_stats_.difficulty, test_stats_);
    test_stats_.answers.resize(5, Controller::AnswerType::NoAnswer);
    test_stats_.timeElapsed = Controller::kTranslateTestDurationSeconds;
    timer_label_->setText(QString("Время: %1:%2")
        .arg(test_stats_.timeElapsed / 60, 2, 10, QChar('0'))
        .arg(test_stats_.timeElapsed % 60, 2, 10, QChar('0')));
    progress_bar_->setAnswers(test_stats_.answers);
    test_number_->setText(QString("Тест №%1").arg(test_stats_.testId));
    timer_->start(1000);
    if (test_stats_.testId == -1) {
        timer_->stop();
        QMessageBox::information(this, "Результат",
                                 "Все тесты данной темы и сложности пройдены");
        emit exitRequested();
    }
    ToNextQuestion();
}

void TranslationEnToRuWidget::ToNextQuestion() {
    if (test_stats_.questionsAnswered >= 5) {
        OnExitClicked();
        return;
    }
    current_question_ = question_set_[test_stats_.questionsAnswered];
    question_label_->setText(current_question_.source_text);

    qDebug() << "Получен вопрос из GetNextTranslationQuestion:";
    qDebug() << "type:" << static_cast<int>(current_question_.type);
    qDebug() << "source_text:" << current_question_.source_text;
    qDebug() << "correct_translations:" << current_question_.correct_translations;
    qDebug() << "hint:" << current_question_.hint;

    if (current_question_.type != Controller::QuestionType::TranslationEnToRu) {
        question_label_->setText("Этот вопрос не является заданием на перевод с английского на русский.");
        return;
    }

    answer_edit_->clear();
    answer_edit_->setFocus();
}

void TranslationEnToRuWidget::onSubmitClicked() {
    QString user_answer = answer_edit_->text().trimmed();
    if (user_answer.isEmpty()) {
        QMessageBox::warning(this, "Введите ответ", "Пожалуйста, введите перевод.");
        return;
    }

    bool is_correct = controller_->CheckAnswer(user_answer, current_question_.correct_translations);

    test_stats_.answers[test_stats_.questionsAnswered] = (is_correct ? Controller::AnswerType::Right : Controller::AnswerType::Wrong);
    test_stats_.questionsAnswered++;
    progress_bar_->setAnswers(test_stats_.answers);
    if (!is_correct) {
        test_stats_.mistakes++;
    }
    if (test_stats_.mistakes == Controller::kMistakesMax) {
        OnExitClicked();
        return;
    }

    ToNextQuestion();
}

void TranslationEnToRuWidget::OnExitClicked() {
    timer_->stop();
    int cnt_answered = 0;
    for (auto i: test_stats_.answers) {
        cnt_answered += (i == Controller::AnswerType::Right);
    }
    test_stats_.rightAnswers = cnt_answered;
    int minutes = test_stats_.timeElapsed / 60;
    int seconds = test_stats_.timeElapsed % 60;
    if (test_stats_.mistakes == Controller::kMistakesMax) {
        QMessageBox::information(this, "Результат",
                                 QString(
                                     "Выполнен тест №%1\nПравильных ответов: %2 из 5\nОставшееся время: %3:%4\nВы превысили допустимое число ошибок: %5")
                                 .arg(test_stats_.testId)
                                 .arg(cnt_answered)
                                 .arg(minutes, 2, 10, QChar('0'))
                                 .arg(seconds, 2, 10, QChar('0'))
                                 .arg(Controller::kMistakesMax));
    } else {
        QMessageBox::information(this, "Результат",
                                 QString("Выполнен тест №%1\nПравильных ответов: %2 из 5\nОставшееся время: %3:%4")
                                 .arg(test_stats_.testId)
                                 .arg(cnt_answered)
                                 .arg(minutes, 2, 10, QChar('0'))
                                 .arg(seconds, 2, 10, QChar('0')));
    }
    controller_->SendDataAboutTest(Controller::QuestionType::TranslationEnToRu, test_stats_.difficulty, test_stats_);
    emit exitRequested();
}

QSize TranslationEnToRuWidget::sizeHint() const {
    return {500, 250};
}

void TranslationEnToRuWidget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_H) {
        QString hint = current_question_.hint.isEmpty() ? "Подсказка недоступна." : current_question_.hint;
        QMessageBox::information(this, "Подсказка", hint);
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        onSubmitClicked();
    }
    QWidget::keyPressEvent(event);
}
