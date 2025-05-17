#include "mainwindow.h"
#include "grammarwidget.h"
#include "translationwidget.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QSize>
#include <QStackedWidget>
#include <QWidget>
#include <QMessageBox>

#include <qdialog.h>
#include <qnamespace.h>
#include <qtmetamacros.h>

DifficultyDialog::DifficultyDialog(QWidget *parent, Controller::DifficultyLevel difficulty_level) : QDialog(parent) {
    setWindowTitle("Установить сложность");

    setMinimumSize(340, 90);
    layout_ = new QFormLayout(this);

    difficulty_combo_box_ = new QComboBox(this);
    difficulty_combo_box_->addItems({"Легкая", "Сложная"});
    layout_->addRow("Сложность:", difficulty_combo_box_);
    difficulty_combo_box_->setCurrentText(difficulty_level == Controller::DifficultyLevel::Easy ? "Легкая" : "Сложная");

    ok_button_ = new QPushButton("OK", this);
    layout_->addRow(ok_button_);
    connect(ok_button_, &QPushButton::clicked, this, &QDialog::accept);
}

DuolinguoApp::DuolinguoApp(QWidget *parent) : QMainWindow(parent) {
    menu_bar_ = new QMenuBar(this);
    menu_ = menu_bar_->addMenu("Настройки");
    difficulty_action_ = menu_->addAction("Уровень сложности");
    statistics_action_ = menu_->addAction("Статистика");
    setMenuBar(menu_bar_);

    connect(difficulty_action_, &QAction::triggered, this, &DuolinguoApp::showDifficultyDialog);
    connect(statistics_action_, &QAction::triggered, this, &DuolinguoApp::showStatistics);

    central_widget_ = new QWidget(this);
    main_layout_ = new QVBoxLayout(central_widget_);

    stacked_widget_ = new QStackedWidget(central_widget_);
    grammar_test_widget_ = new GrammarTestWidget(central_widget_, &controller_);
    grammar_gap_fill_widget_ = new GrammarQuestionWidget(central_widget_, &controller_);
    translation_ru_to_en_widget_ = new TranslationRuToEnWidget(central_widget_, &controller_);
    translation_en_to_ru_widget_ = new TranslationEnToRuWidget(central_widget_, &controller_);
    task_selection_widget_ = new TaskSelectionWidget(central_widget_, &controller_);
    stacked_widget_->addWidget(task_selection_widget_);
    stacked_widget_->addWidget(grammar_test_widget_);
    stacked_widget_->addWidget(grammar_gap_fill_widget_);
    stacked_widget_->addWidget(translation_ru_to_en_widget_);
    stacked_widget_->addWidget(translation_en_to_ru_widget_);
    main_layout_->addWidget(stacked_widget_);

    setCentralWidget(central_widget_);

    connect(task_selection_widget_, &TaskSelectionWidget::taskSelected, this, &DuolinguoApp::HandleTaskSelection);
    connect(grammar_test_widget_, &GrammarTestWidget::exitRequested, this, &DuolinguoApp::HandleExit);
    connect(grammar_gap_fill_widget_, &GrammarQuestionWidget::exitRequested, this, &DuolinguoApp::HandleExit);
    connect(translation_ru_to_en_widget_, &TranslationRuToEnWidget::exitRequested, this, &DuolinguoApp::HandleExit);
    connect(translation_en_to_ru_widget_, &TranslationEnToRuWidget::exitRequested, this, &DuolinguoApp::HandleExit);

    stacked_widget_->setCurrentWidget(task_selection_widget_);
    adjustSize();
}

void DuolinguoApp::showDifficultyDialog() {
    DifficultyDialog dialog(this, controller_.GetDifficulty());
    if (dialog.exec() == QDialog::Accepted) {
        QString const selected_difficulty = dialog.difficulty_combo_box_->currentText();
        if (selected_difficulty == "Легкая") {
            controller_.SetDifficulty(Controller::DifficultyLevel::Easy);
        } else {
            controller_.SetDifficulty(Controller::DifficultyLevel::Hard);
        }
    }
    qDebug() << QString(controller_.GetDifficulty() == Controller::DifficultyLevel::Easy
                            ? "Установлена сложность: просто"
                            : "Установлена сложность: сложно");
}

void DuolinguoApp::showStatistics() {
    QString stats = controller_.GetTotalStats();
    QMessageBox::information(this, "Общая статистика", stats);
}

void DuolinguoApp::HandleTaskSelection(int taskType) {
    switch (taskType) {
        case TaskSelectionWidget::GrammarTest:
            stacked_widget_->setCurrentWidget(grammar_test_widget_);
            grammar_test_widget_->UpdateTest();
            break;
        case TaskSelectionWidget::GrammarGapFill:
            stacked_widget_->setCurrentWidget(grammar_gap_fill_widget_);
            grammar_gap_fill_widget_->UpdateTest();
            break;
        case TaskSelectionWidget::TranslationEnToRu:
            stacked_widget_->setCurrentWidget(translation_en_to_ru_widget_);
            translation_en_to_ru_widget_->UpdateTest();
            break;
        case TaskSelectionWidget::TranslationRuToEn:
            stacked_widget_->setCurrentWidget(translation_ru_to_en_widget_);
            translation_ru_to_en_widget_->UpdateTest();
            break;
        default:
            stacked_widget_->setCurrentWidget(task_selection_widget_);
            break;
    }
}

void DuolinguoApp::HandleExit() {
    stacked_widget_->setCurrentWidget(task_selection_widget_);
    task_selection_widget_->UpdateBall();
}

TaskSelectionWidget::TaskSelectionWidget(QWidget *parent, Controller *controller) : QWidget(parent),
    controller_(controller), task_choose_layout_(new QHBoxLayout()),
    grammar_button_layout_(new QVBoxLayout()),
    translation_button_layout_(new QVBoxLayout()) {
    main_layout_ = new QVBoxLayout(this);

    label_ball_ = new QLabel(this);
    label_ball_->setText(QString("Общая оценка: %1 из %2").arg(controller_->GetBall()).arg(Controller::kMark));

    main_layout_->addWidget(label_ball_);

    task_choose_label_ = new QLabel("Выберите задание:", this);
    task_choose_label_->setAlignment(Qt::AlignCenter);
    main_layout_->addWidget(task_choose_label_);

    task_grammar_label_ = new QLabel("Грамматика:", this);
    task_grammar_label_->setAlignment(Qt::AlignCenter);
    grammar_test_button_ = new QPushButton("Тест", this);
    grammar_gap_fill_button_ = new QPushButton("Заполнение пропусков", this);
    grammar_test_button_->setMinimumSize(QSize(200, 35));
    grammar_gap_fill_button_->setMinimumSize(QSize(200, 35));
    grammar_button_layout_->addWidget(task_grammar_label_);
    grammar_button_layout_->addWidget(grammar_test_button_);
    grammar_button_layout_->addWidget(grammar_gap_fill_button_);
    task_choose_layout_->addLayout(grammar_button_layout_);

    grammar_button_layout_->addStretch();

    task_translate_label_ = new QLabel("Перевод:", this);
    task_translate_label_->setAlignment(Qt::AlignCenter);
    translation_en_to_ru_button_ = new QPushButton("Англ → Рус", this);
    translation_ru_to_en_button_ = new QPushButton("Рус → Англ", this);
    translation_en_to_ru_button_->setMinimumSize(QSize(200, 35));
    translation_ru_to_en_button_->setMinimumSize(QSize(200, 35));
    translation_button_layout_->addWidget(task_translate_label_);
    translation_button_layout_->addWidget(translation_en_to_ru_button_);
    translation_button_layout_->addWidget(translation_ru_to_en_button_);
    task_choose_layout_->addLayout(translation_button_layout_);

    translation_button_layout_->addStretch();

    main_layout_->addLayout(task_choose_layout_);

    connect(grammar_test_button_, &QPushButton::clicked, this, &TaskSelectionWidget::OnGrammarTestClicked);
    connect(grammar_gap_fill_button_, &QPushButton::clicked, this, &TaskSelectionWidget::OnGrammarGapFillClicked);
    connect(translation_en_to_ru_button_, &QPushButton::clicked, this,
            &TaskSelectionWidget::OnTranslationEnToRuClicked);
    connect(translation_ru_to_en_button_, &QPushButton::clicked, this,
            &TaskSelectionWidget::OnTranslationRuToEnClicked);
}

void TaskSelectionWidget::UpdateBall() {
    label_ball_->setText(QString("Общая оценка: %1 из %2").arg(controller_->GetBall()).arg(Controller::kMark));
}

QSize TaskSelectionWidget::sizeHint() const {
    return {500, 250};
}

void TaskSelectionWidget::OnGrammarTestClicked() {
    emit taskSelected(TaskType::GrammarTest);
}

void TaskSelectionWidget::OnGrammarGapFillClicked() {
    emit taskSelected(TaskType::GrammarGapFill);
}

void TaskSelectionWidget::OnTranslationEnToRuClicked() {
    emit taskSelected(TaskType::TranslationEnToRu);
}

void TaskSelectionWidget::OnTranslationRuToEnClicked() {
    emit taskSelected(TaskType::TranslationRuToEn);
}