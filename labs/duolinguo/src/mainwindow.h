#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "controller.h"

class TaskSelectionWidget;

class DifficultyDialog : public QDialog {
    Q_OBJECT
public:
    DifficultyDialog(QWidget* parent = nullptr);
    QComboBox* difficulty_combo_box_{};

private:
    QFormLayout* layout_{};
    QPushButton* ok_button_{};
};

class DuolinguoApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit DuolinguoApp(QWidget* parent = nullptr);

private slots:
    void showDifficultyDialog();

private:
    QMenuBar* menu_bar_{};
    QMenu* menu_{};
    QAction* difficulty_action_{};
    QVBoxLayout* main_layout_{};
    QStackedWidget* stacked_widget_{};
    TaskSelectionWidget* task_selection_widget_{};
    QWidget* central_widget_{};

    Controller controller_;
};

class TaskSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskSelectionWidget(QWidget* parent = nullptr);
    [[nodiscard]] QSize sizeHint() const override;

private:
    QVBoxLayout* main_layout_{};
    QLabel* task_choose_label_{};
    QHBoxLayout* task_choose_layout_{};
    QVBoxLayout* grammar_button_layout_{};
    QLabel* task_grammar_label_{};
    QPushButton* grammar_test_button_{};
    QPushButton* grammar_gap_fill_button_{};
    QVBoxLayout* translation_button_layout_{};
    QLabel* task_translate_label_{};
    QPushButton* translation_en_to_ru_button_{};
    QPushButton* translation_ru_to_en_button_{};
};

#endif // MAINWINDOW_H