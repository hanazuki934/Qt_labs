#include "grammarwidget.h"

#include <QMessageBox>
#include <qpushbutton.h>
#include <qtmetamacros.h>

GrammarTestWidget::GrammarTestWidget(QWidget *parent, Controller *controller)
    : QWidget(parent), controller_(controller) {
    layout_ = new QVBoxLayout(this);
    exit_button_ = new QPushButton("Выход", this);
    question_label_ = new QLabel(this);
    option1_button_ = new QRadioButton(this);
    option2_button_ = new QRadioButton(this);
    option3_button_ = new QRadioButton(this);
    option4_button_ = new QRadioButton(this);
    button_group_ = new QButtonGroup(this);

    layout_->addWidget(question_label_);
    layout_->addWidget(option1_button_);
    layout_->addWidget(option2_button_);
    layout_->addWidget(option3_button_);
    layout_->addWidget(option4_button_);
    layout_->addWidget(exit_button_);
    setLayout(layout_);

    button_group_->setExclusive(true);
    button_group_->addButton(option1_button_, 1);
    button_group_->addButton(option2_button_, 2);
    button_group_->addButton(option3_button_, 3);
    button_group_->addButton(option4_button_, 4);

    // Alternative Qt5-style syntax
    connect(button_group_, &QButtonGroup::buttonClicked,
            this, [this](QAbstractButton *button) {
                int id = button_group_->id(button);
                onOptionClicked(id);
            });
    connect(exit_button_, &QPushButton::clicked, this, &GrammarTestWidget::OnExitClicked);
}

void GrammarTestWidget::UpdateTest() {
    test_stats_.Clear();
    test_stats_.difficulty = controller_->GetDifficulty();
    test_stats_.type = Controller::QuestionType::MultipleChoice;
    ToNextQuestion();
}

void GrammarTestWidget::ToNextQuestion() {
    if (test_stats_.questionsAnswered == 5) {
        OnExitClicked();
        return;
    }
    current_question_ = controller_->GetNextGrammarQuestion(Controller::QuestionType::MultipleChoice);
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
        case 1: selected_answer = option1_button_->text();
            break;
        case 2: selected_answer = option2_button_->text();
            break;
        case 3: selected_answer = option3_button_->text();
            break;
        case 4: selected_answer = option4_button_->text();
            break;
        default: return;
    }

    bool is_correct = current_question_.correct_answers.contains(selected_answer);

    test_stats_.questionsAnswered++;
    test_stats_.answers.append(is_correct ? 1 : 0);

    /*QMessageBox::information(this, "Результат",
                             is_correct ? "Правильно!" : "Неправильно. Правильный ответ: " + current_question_.correct_answers.first());*/

    ToNextQuestion();
}

void GrammarTestWidget::OnExitClicked() {
    int cnt_answered = 0;
    for (auto i: test_stats_.answers) {
        cnt_answered += (i == 1);
    }
    QMessageBox::information(this, "Результат",
                             "Правильных ответов: " + QString::number(cnt_answered) + " из 5");
    emit exitRequested();
}

QSize GrammarTestWidget::sizeHint() const {
    return {500, 250};
}
