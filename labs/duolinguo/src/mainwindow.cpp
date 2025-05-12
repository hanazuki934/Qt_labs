#include "mainwindow.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QSize>
#include <QStackedWidget>
#include <QWidget>

#include <qdialog.h>
#include <qnamespace.h>

DifficultyDialog::DifficultyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Установить сложность");

    setMinimumSize(340, 90);
    layout_ = new QFormLayout(this);

    difficulty_combo_box_ = new QComboBox(this);
    difficulty_combo_box_->addItems({"Легкая", "Сложная"});
    layout_->addRow("Сложность:", difficulty_combo_box_);

    ok_button_ = new QPushButton("OK", this);
    layout_->addRow(ok_button_);
    connect(ok_button_, &QPushButton::clicked, this, &QDialog::accept);
}

DuolinguoApp::DuolinguoApp(QWidget *parent) : QMainWindow(parent), difficulty_action_(menu_->addAction("Уровень сложности")) {
    menu_bar_ = new QMenuBar(this);
    menu_ = menu_bar_->addMenu("Настройки");
    menu_->addAction("Статистика");
    setMenuBar(menu_bar_);

    connect(difficulty_action_, &QAction::triggered, this, &DuolinguoApp::showDifficultyDialog);

    central_widget_ = new QWidget(this);
    main_layout_ = new QVBoxLayout(central_widget_);

    stacked_widget_ = new QStackedWidget(central_widget_);
    task_selection_widget_ = new TaskSelectionWidget(central_widget_);
    stacked_widget_->addWidget(task_selection_widget_);
    main_layout_->addWidget(stacked_widget_);

    setCentralWidget(central_widget_);

    stacked_widget_->setCurrentWidget(task_selection_widget_);
    adjustSize();
}

void DuolinguoApp::showDifficultyDialog() {
    DifficultyDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString const selected_difficulty = dialog.difficulty_combo_box_->currentText();
        if (selected_difficulty == "Легкая") {
            controller_.SetDifficulty(Controller::DifficultyLevel::Easy);
        } else {
            controller_.SetDifficulty(Controller::DifficultyLevel::Hard);
        }
        //qDebug() << "Выбрана сложность:" << selected_difficulty;
    }
}

TaskSelectionWidget::TaskSelectionWidget(QWidget *parent) : QWidget(parent), task_choose_layout_(new QHBoxLayout()),
                                                            grammar_button_layout_(new QVBoxLayout()),
                                                            translation_button_layout_(new QVBoxLayout()) {
    main_layout_ = new QVBoxLayout(this);

    task_choose_label_ = new QLabel("Выберите задание:", this);
    task_choose_label_->setAlignment(Qt::AlignCenter);
    main_layout_->addWidget(task_choose_label_);

    task_grammar_label_ = new QLabel("Грамматика:", this);
    task_grammar_label_->setAlignment(Qt::AlignCenter);
    grammar_test_button_ = new QPushButton("Тест", this);
    grammar_gap_fill_button_ = new QPushButton("Заполнение пропусков", this);
    grammar_button_layout_->addWidget(task_grammar_label_);
    grammar_button_layout_->addWidget(grammar_test_button_);
    grammar_button_layout_->addWidget(grammar_gap_fill_button_);
    task_choose_layout_->addLayout(grammar_button_layout_);

    grammar_button_layout_->addStretch();

    task_translate_label_ = new QLabel("Перевод:", this);
    task_translate_label_->setAlignment(Qt::AlignCenter);
    translation_en_to_ru_button_ = new QPushButton("Англ → Рус", this);
    translation_ru_to_en_button_ = new QPushButton("Рус → Англ", this);
    translation_button_layout_->addWidget(task_translate_label_);
    translation_button_layout_->addWidget(translation_en_to_ru_button_);
    translation_button_layout_->addWidget(translation_ru_to_en_button_);
    task_choose_layout_->addLayout(translation_button_layout_);

    translation_button_layout_->addStretch();

    main_layout_->addLayout(task_choose_layout_);
}

QSize TaskSelectionWidget::sizeHint() const {
    return {500, 250};
}
