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
        MultipleChoice,
        GapFill,
        TranslationRuToEn,
        TranslationEnToRu
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

    enum class AnswerType {
        Right,
        Wrong,
        NoAnswer
    };

    struct TotalStats {
        int totalQuestionsAnswered = 0;
        int totalCorrectAnswers = 0;
        int totalIncorrectAnswers = 0;
        int grammarTotal = 0;
        int grammarCorrect = 0;
        int grammarIncorrect = 0;
        int translationTotal = 0;
        int translationCorrect = 0;
        int translationIncorrect = 0;
        int easyTotal = 0;
        int easyCorrect = 0;
        int easyIncorrect = 0;
        int hardTotal = 0;
        int hardCorrect = 0;
        int hardIncorrect = 0;
    };

    struct TestStats {
        int questionsAnswered = 0;
        QVector<AnswerType> answers{};
        QuestionType type{};
        DifficultyLevel difficulty{};

        void Clear();
    };

    Controller(const QString& grammarTestEasyDbPath = "grammartesteasy.db",
               const QString& grammarTestHardDbPath = "grammarTestHard.db",
               const QString& translationDbPath = "translation.db");

    [[nodiscard]] DifficultyLevel GetDifficulty() const;
    void SetDifficulty(DifficultyLevel difficulty);

    bool InitializeDatabase(const QString& grammarTestEasyDbPath, const QString& grammarTestHardDbPath, const QString& translationDbPath);
    GrammarQuestion GetNextGrammarQuestion(QuestionType type);
    TranslationQuestion GetNextTranslationQuestion();

private:
    DifficultyLevel difficulty_{DifficultyLevel::Easy};
    QSqlDatabase grammar_test_easy_db_;
    QSqlDatabase grammar_test_hard_db_;
    QSqlDatabase translation_db_;

    int currentGrammarTestEasyId_{1};
    int currentGrammarTestHardId_{1};
    int currentGrammarGapFillEasyId_{1};
    int currentGrammarGapFillHardId_{1};
    int currentTranslationRuToEnEasyId_{1};
    int currentTranslationRuToEnHardId_{1};
    int currentTranslationEnToRuEasyId_{1};
    int currentTranslationEnToRuHardId_{1};

    void ShuffleOptions(GrammarQuestion& question);
};

#endif // CONTROLLER_H