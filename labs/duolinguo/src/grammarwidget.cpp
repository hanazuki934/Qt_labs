#include "grammarwidget.h"

#include <QMessageBox>
#include <QKeyEvent>
#include <QPushButton>
#include <QTimer>
#include <qlineedit.h>
#include <qsize.h>
#include <qtmetamacros.h>

GrammarTestWidget::GrammarTestWidget(QWidget *parent, Controller *controller)
    : QWidget(parent), controller_(controller) {
    layout_ = new QVBoxLayout(this);

    progress_bar_ = new ProgressBar(this);

    test_number_ = new QLabel(this);
    question_label_ = new QLabel(this);
    option1_button_ = new QRadioButton(this);
    option2_button_ = new QRadioButton(this);
    option3_button_ = new QRadioButton(this);
    option4_button_ = new QRadioButton(this);
    button_group_ = new QButtonGroup(this);

    submit_button_ = new QPushButton("Следующий вопрос", this);
    exit_button_ = new QPushButton("Выход", this);
    timer_label_ = new QLabel("Время: 03:00", this);

    test_layout_ = new QHBoxLayout(this);

    test_layout_->addWidget(progress_bar_);
    test_layout_->addWidget(test_number_);

    button_layout_ = new QHBoxLayout(this);

    button_layout_->addWidget(timer_label_);
    button_layout_->addWidget(submit_button_);
    button_layout_->addWidget(exit_button_);

    layout_->addLayout(test_layout_);
    layout_->addWidget(question_label_);
    layout_->addWidget(option1_button_);
    layout_->addWidget(option2_button_);
    layout_->addWidget(option3_button_);
    layout_->addWidget(option4_button_);
    layout_->addLayout(button_layout_);
    setLayout(layout_);

    button_group_->setExclusive(true);
    button_group_->addButton(option1_button_, 1);
    button_group_->addButton(option2_button_, 2);
    button_group_->addButton(option3_button_, 3);
    button_group_->addButton(option4_button_, 4);

    connect(submit_button_, &QPushButton::clicked,
        this, [this]() {
            QAbstractButton *selected_button = button_group_->checkedButton();
            if (selected_button) {
                int id = button_group_->id(selected_button);
                onOptionClicked(id);
            } else {
                onOptionClicked(-1);
            }
        });
    connect(exit_button_, &QPushButton::clicked, this, &GrammarTestWidget::OnExitClicked);

    // Инициализация таймера с обратным отсчетом
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
            OnExitClicked(); // Завершаем тест
        }
    });
}

void GrammarTestWidget::UpdateTest() {
    test_stats_.Clear();
    test_stats_.difficulty = controller_->GetDifficulty();
    question_set_.clear();
    question_set_ = controller_->RequestGrammarQuestionSet(Controller::QuestionType::MultipleChoice, test_stats_.difficulty, test_stats_);
    test_stats_.answers.resize(5, Controller::AnswerType::NoAnswer);
    test_stats_.timeElapsed = Controller::kGrammarTestDurationSeconds; // Устанавливаем начальное время (180 секунд)
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
        emit(exitRequested());
    }
    ToNextQuestion();
}

void GrammarTestWidget::ToNextQuestion() {
    if (test_stats_.questionsAnswered == 5) {
        OnExitClicked();
        return;
    }
    current_question_ = question_set_[test_stats_.questionsAnswered];
    question_label_->setText(current_question_.question);

    qDebug() << "Получен вопрос из GetNextGrammarQuestion:";
    qDebug() << "type:" << static_cast<int>(current_question_.type);
    qDebug() << "question:" << current_question_.question;
    qDebug() << "correct_answers:" << current_question_.correct_answers;
    qDebug() << "options:" << current_question_.options;
    qDebug() << "options size:" << current_question_.options.size();

    if (current_question_.type != Controller::QuestionType::MultipleChoice || current_question_.options.size() < 4) {
        question_label_->setText(
            "Этот вопрос не является тестом с вариантами ответа или имеет неверное количество вариантов.");
        return;
    }

    option1_button_->setText(current_question_.options[0]);
    option2_button_->setText(current_question_.options[1]);
    option3_button_->setText(current_question_.options[2]);
    option4_button_->setText(current_question_.options[3]);

    button_group_->setExclusive(false);
    option1_button_->setChecked(false);
    option2_button_->setChecked(false);
    option3_button_->setChecked(false);
    option4_button_->setChecked(false);
    button_group_->setExclusive(true);
}

void GrammarTestWidget::onOptionClicked(int id) {
    QString selected_answer;
    switch (id) {
        case 1: selected_answer = option1_button_->text(); break;
        case 2: selected_answer = option2_button_->text(); break;
        case 3: selected_answer = option3_button_->text(); break;
        case 4: selected_answer = option4_button_->text(); break;
        default: selected_answer = "~";
    }

    bool is_correct = current_question_.correct_answers.contains(selected_answer);

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

void GrammarTestWidget::OnExitClicked() {
    timer_->stop(); // Останавливаем таймер при выходе
    int cnt_answered = 0;
    for (auto i : test_stats_.answers) {
        cnt_answered += (i == Controller::AnswerType::Right);
    }
    test_stats_.rightAnswers = cnt_answered;
    int minutes = test_stats_.timeElapsed / 60;
    int seconds = test_stats_.timeElapsed % 60;
    if (test_stats_.mistakes == Controller::kMistakesMax) {
        QMessageBox::information(this, "Результат",
                             QString("Выполнен тест №%1\nПравильных ответов: %2 из 5\nОставшееся время: %3:%4\nВы превысили допустимое число ошибок: %5")
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
    controller_->SendDataAboutTest(Controller::QuestionType::MultipleChoice, test_stats_.difficulty, test_stats_);
    emit exitRequested();
}

QSize GrammarTestWidget::sizeHint() const {
    return {500, 250};
}

void GrammarTestWidget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_H) {
        QString hint = current_question_.hint.isEmpty() ? "Подсказка недоступна." : current_question_.hint;
        QMessageBox::information(this, "Подсказка", hint);
    }
    QWidget::keyPressEvent(event);
}

GrammarQuestionWidget::GrammarQuestionWidget(QWidget *parent, Controller *controller)
    : QWidget(parent), controller_(controller) {
    layout_ = new QVBoxLayout(this);

    progress_bar_ = new ProgressBar(this);
    test_number_ = new QLabel(this);
    question_label_ = new QLabel(this);
    answer_edit_ = new QLineEdit(this);
    submit_button_ = new QPushButton("Следующий вопрос", this);
    exit_button_ = new QPushButton("Выход", this);
    timer_label_ = new QLabel("Время: 03:00", this);

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

    connect(submit_button_, &QPushButton::clicked, this, &GrammarQuestionWidget::onSubmitClicked);
    connect(exit_button_, &QPushButton::clicked, this, &GrammarQuestionWidget::OnExitClicked);
    connect(answer_edit_, &QLineEdit::returnPressed, this, &GrammarQuestionWidget::onSubmitClicked);

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

void GrammarQuestionWidget::UpdateTest() {
    test_stats_.Clear();
    test_stats_.difficulty = controller_->GetDifficulty();
    question_set_.clear();
    question_set_ = controller_->RequestGrammarQuestionSet(Controller::QuestionType::GapFill, test_stats_.difficulty, test_stats_);
    test_stats_.answers.resize(5, Controller::AnswerType::NoAnswer);
    test_stats_.timeElapsed = Controller::kGrammarTestDurationSeconds;
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

void GrammarQuestionWidget::ToNextQuestion() {
    if (test_stats_.questionsAnswered >= 5) {
        OnExitClicked();
        return;
    }
    current_question_ = question_set_[test_stats_.questionsAnswered];
    question_label_->setText(current_question_.question);

    qDebug() << "Получен вопрос из GetNextGrammarQuestion:";
    qDebug() << "type:" << static_cast<int>(current_question_.type);
    qDebug() << "question:" << current_question_.question;
    qDebug() << "correct_answers:" << current_question_.correct_answers;
    qDebug() << "options:" << current_question_.options;
    qDebug() << "options size:" << current_question_.options.size();

    if (current_question_.type != Controller::QuestionType::GapFill) {
        question_label_->setText("Этот вопрос не является тестом с заполнением пропусков.");
        return;
    }

    answer_edit_->clear();
    answer_edit_->setFocus();
}

void GrammarQuestionWidget::onSubmitClicked() {
    QString user_answer = answer_edit_->text().trimmed();
    if (user_answer.isEmpty()) {
        user_answer = "~";
    }

    bool is_correct = controller_->CheckAnswer(user_answer, current_question_.correct_answers);

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

void GrammarQuestionWidget::OnExitClicked() {
    timer_->stop();
    int cnt_answered = 0;
    for (auto i : test_stats_.answers) {
        cnt_answered += (i == Controller::AnswerType::Right);
    }
    test_stats_.rightAnswers = cnt_answered;
    int minutes = test_stats_.timeElapsed / 60;
    int seconds = test_stats_.timeElapsed % 60;
    if (test_stats_.mistakes == Controller::kMistakesMax) {
        QMessageBox::information(this, "Результат",
                             QString("Выполнен тест №%1\nПравильных ответов: %2 из 5\nОставшееся время: %3:%4\nВы превысили допустимое число ошибок: %5")
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
    controller_->SendDataAboutTest(Controller::QuestionType::GapFill, test_stats_.difficulty, test_stats_);
    emit exitRequested();
}

QSize GrammarQuestionWidget::sizeHint() const {
    return {500, 250};
}

void GrammarQuestionWidget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_H) {
        QString hint = current_question_.hint.isEmpty() ? "Подсказка недоступна." : current_question_.hint;
        QMessageBox::information(this, "Подсказка", hint);
    } else if (event->key() == Qt::Key_Enter) {
        onSubmitClicked();
    }
    QWidget::keyPressEvent(event);
}