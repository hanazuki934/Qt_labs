#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class Controller {
public:
    static constexpr int kGrammarTestDurationSeconds = 180;
    static constexpr int kTranslateTestDurationSeconds = 600;
    static constexpr int kTestSize = 5;
    static constexpr int kMistakesMax = 3;
    static constexpr int kMark = 12;

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
        QString hint;
    };

    struct TranslationQuestion {
        QuestionType type{QuestionType::TranslationRuToEn};
        QString source_text;
        QStringList correct_translations;
        QString hint;
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
        std::vector<AnswerType> answers{};
        QuestionType type{};
        DifficultyLevel difficulty{};
        int timeElapsed = 0;
        int rightAnswers = 0;
        int testId = 0;
        int mistakes = 0;

        void Clear();
    };

    explicit Controller();

    [[nodiscard]] DifficultyLevel GetDifficulty() const;
    void SetDifficulty(DifficultyLevel difficulty);

    [[nodiscard]] int GetBall() const;
    void SetBall(int ball);

    bool InitializeDatabase(const QString& grammar_test_easy_db_path, const QString& grammar_test_hard_db_path, const QString& translationDbPath);
    GrammarQuestion GetNextGrammarQuestion(int question_index, QuestionType type, DifficultyLevel difficulty = DifficultyLevel::Easy);
    TranslationQuestion GetNextTranslationQuestion();
    std::vector<GrammarQuestion> RequestGrammarQuestionSet(QuestionType type, DifficultyLevel difficulty, TestStats& stats);
    void SendDataAboutTest(QuestionType type, DifficultyLevel difficulty, const TestStats& stats);

private:
    DifficultyLevel difficulty_{DifficultyLevel::Easy};
    QSqlDatabase grammar_test_easy_db_;
    QSqlDatabase grammar_test_hard_db_;
    int ball_ = 0;

    int current_grammar_test_easy_id_{1};
    int current_grammar_test_hard_id_{1};
    int current_grammar_gap_fill_easy_id_{1};
    int current_grammar_gap_fill_hard_id_{1};
    int current_translation_ru_to_en_easy_id_{1};
    int current_translation_ru_to_en_hard_id_{1};
    int current_translation_en_to_ru_easy_id_{1};
    int current_translation_en_to_ru_hard_id_{1};

    std::vector<TestStats> grammar_test_easy_stats_{};
    std::vector<TestStats> grammar_test_hard_stats_{};
    std::vector<TestStats> grammar_gap_fill_easy_stats_{};
    std::vector<TestStats> grammar_gap_fill_hard_stats_{};
    std::vector<TestStats> translation_test_ru_to_en_easy_stats_{};
    std::vector<TestStats> translation_test_ru_to_en_hard_stats_{};
    std::vector<TestStats> translation_test_en_to_ru_easy_stats_{};
    std::vector<TestStats> translation_test_en_to_ru_hard_stats_{};

    void ShuffleOptions(GrammarQuestion& question);
};

#endif // CONTROLLER_H
