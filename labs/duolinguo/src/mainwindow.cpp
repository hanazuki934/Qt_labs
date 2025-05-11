#include "mainwindow.h"

#include <QHBoxLayout>
#include <QMainWindow>
#include <QSize>
#include <QStackedWidget>
#include <QWidget>

#include <qnamespace.h>

DuolinguoApp::DuolinguoApp(QWidget *parent) : QMainWindow(parent) {
    menu_bar_ = new QMenuBar(this);
    menu_ = menu_bar_->addMenu("Настройки");
    menu_->addAction("Уровень сложности");
    menu_->addAction("Статистика");
    setMenuBar(menu_bar_);

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
