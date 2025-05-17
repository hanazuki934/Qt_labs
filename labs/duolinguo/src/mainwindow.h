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
#include "grammarwidget.h"
#include "translationwidget.h"

class TaskSelectionWidget;

class DifficultyDialog : public QDialog {
    Q_OBJECT
public:
    DifficultyDialog(QWidget* parent = nullptr, Controller::DifficultyLevel difficulty_level = Controller::DifficultyLevel::Easy);
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
    void HandleTaskSelection(int taskType);
    void showStatistics();
    void HandleExit();

private:
    QMenuBar* menu_bar_{};
    QMenu* menu_{};
    QAction* difficulty_action_{};
    QVBoxLayout* main_layout_{};
    QStackedWidget* stacked_widget_{};
    TaskSelectionWidget* task_selection_widget_{};
    GrammarTestWidget* grammar_test_widget_{};
    GrammarQuestionWidget* grammar_gap_fill_widget_{};
    TranslationRuToEnWidget* translation_ru_to_en_widget_{};
    TranslationEnToRuWidget* translation_en_to_ru_widget_{};
    QAction* statistics_action_{};
    QWidget* central_widget_{};

    Controller controller_;
};

class TaskSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    enum TaskType {
        GrammarTest,
        GrammarGapFill,
        TranslationEnToRu,
        TranslationRuToEn
    };

    explicit TaskSelectionWidget(QWidget* parent = nullptr, Controller* controller = nullptr);
    [[nodiscard]] QSize sizeHint() const override;
    void UpdateBall();

signals:
    void taskSelected(int taskType);

public slots:
    void OnGrammarTestClicked();
    void OnGrammarGapFillClicked();
    void OnTranslationEnToRuClicked();
    void OnTranslationRuToEnClicked();

private:
    Controller* controller_{};

    QVBoxLayout* main_layout_{};
    QLabel* label_ball_{};
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