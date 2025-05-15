#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class Controller {
public:
    enum class DifficultyLevel {
        Easy,
        Hard
    };

    enum class QuestionType {
        MultipleChoice,     // Тест с выбором ответа
        GapFill,            // Вставка слов
        TranslationRuToEn,  // Перевод: русский → английский
        TranslationEnToRu   // Перевод: английский → русский
    };

    struct GrammarQuestion {
        QuestionType type{QuestionType::MultipleChoice};
        QString question;
        QStringList correct_answers;
        QStringList options;
    };

    struct TranslationQuestion {
        QuestionType type{QuestionType::TranslationRuToEn};
        QString source_text;
        QStringList correct_translations;
    };

    struct TotalStats {
        // Общая статистика
        int totalQuestionsAnswered = 0;         // Общее вопросов отвечено
        int totalCorrectAnswers = 0;            // Правильных ответов
        int totalIncorrectAnswers = 0;          // Неправильных ответов

        // По грамматике
        int grammarTotal = 0;                   // По грамматике всего
        int grammarCorrect = 0;                 // Правильных ответов по грамматике
        int grammarIncorrect = 0;               // Неправильных ответов по грамматике

        // По переводу
        int translationTotal = 0;               // По переводу всего
        int translationCorrect = 0;             // Правильных ответов по переводу
        int translationIncorrect = 0;           // Неправильных ответов по переводу

        // По простым вопросам
        int easyTotal = 0;                      // Простых всего
        int easyCorrect = 0;                    // Правильных ответов простых
        int easyIncorrect = 0;                  // Неправильных ответов простых

        // По сложным вопросам
        int hardTotal = 0;                      // Сложных всего
        int hardCorrect = 0;                    // Правильных ответов сложных
        int hardIncorrect = 0;                  // Неправильных ответов сложных
    };

    struct TestStats {
        int questionsAnswered = 0;
        QVector<int> answers{};
        QuestionType type{};
        DifficultyLevel difficulty{};

        void Clear();
    };

    Controller(const QString& grammarDbPath = "grammar.db",
               const QString& translationDbPath = "translation.db");

    [[nodiscard]] DifficultyLevel GetDifficulty() const;
    void SetDifficulty(DifficultyLevel difficulty);

    bool InitializeDatabase(const QString& grammarDbPath, const QString& translationDbPath);
    GrammarQuestion GetNextGrammarQuestion(QuestionType type);
    TranslationQuestion GetNextTranslationQuestion();

private:
    DifficultyLevel difficulty_{DifficultyLevel::Easy};
    QSqlDatabase grammar_db_;  // База данных для грамматики
    QSqlDatabase translation_db_;  // База данных для переводов

    // Поля для отслеживания текущих id вопросов для каждого типа и сложности
    int currentGrammarTestEasyId_{1};  // Грамматика: тест, простые
    int currentGrammarTestHardId_{1};  // Грамматика: тест, сложные
    int currentGrammarGapFillEasyId_{1};  // Грамматика: вставка слов, простые
    int currentGrammarGapFillHardId_{1};  // Грамматика: вставка слов, сложные
    int currentTranslationRuToEnEasyId_{1};  // Перевод: русский → английский, простые
    int currentTranslationRuToEnHardId_{1};  // Перевод: русский → английский, сложные
    int currentTranslationEnToRuEasyId_{1};  // Перевод: английский → русский, простые
    int currentTranslationEnToRuHardId_{1};  // Перевод: английский → русский, сложные

    void ShuffleOptions(GrammarQuestion& question);
};

#endif // CONTROLLER_H