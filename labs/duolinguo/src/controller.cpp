#include "controller.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QSqlError>
#include <QSqlQuery>

Controller::Controller() {
    const QString grammar_test_easy_db_path = "labs/duolinguo/src/db/grammartesteasy.db";
    const QString grammar_test_hard_db_path = "labs/duolinguo/src/db/grammartesthard.db";
    const QString grammar_question_easy_db_path = "labs/duolinguo/src/db/grammarquestioneasy.db";
    const QString grammar_question_hard_db_path = "labs/duolinguo/src/db/grammarquestionhard.db";
    const QString translation_ru_to_en_easy_db_path = "labs/duolinguo/src/db/translationrutoeneasy.db";
    const QString translation_ru_to_en_hard_db_path = "labs/duolinguo/src/db/translationrutoenhard.db";
    const QString translation_en_to_ru_easy_db_path = "labs/duolinguo/src/db/translationentorueasy.db";
    const QString translation_en_to_ru_hard_db_path = "labs/duolinguo/src/db/translationentoruhard.db";

    // Get absolute paths
    const QFileInfo grammar_test_easy_db_file(grammar_test_easy_db_path);
    const QFileInfo grammar_test_hard_db_file(grammar_test_hard_db_path);
    const QFileInfo grammar_question_easy_db_file(grammar_question_easy_db_path);
    const QFileInfo grammar_question_hard_db_file(grammar_question_hard_db_path);
    const QFileInfo translation_ru_to_en_easy_db_file(translation_ru_to_en_easy_db_path);
    const QFileInfo translation_ru_to_en_hard_db_file(translation_ru_to_en_hard_db_path);
    const QFileInfo translation_en_to_ru_easy_db_file(translation_en_to_ru_easy_db_path);
    const QFileInfo translation_en_to_ru_hard_db_file(translation_en_to_ru_hard_db_path);

    const QString grammar_test_easy_absolute_path = grammar_test_easy_db_file.absoluteFilePath();
    const QString grammar_test_hard_absolute_path = grammar_test_hard_db_file.absoluteFilePath();
    const QString grammar_question_easy_absolute_path = grammar_question_easy_db_file.absoluteFilePath();
    const QString grammar_question_hard_absolute_path = grammar_question_hard_db_file.absoluteFilePath();
    const QString translation_ru_to_en_easy_absolute_path = translation_ru_to_en_easy_db_file.absoluteFilePath();
    const QString translation_ru_to_en_hard_absolute_path = translation_ru_to_en_hard_db_file.absoluteFilePath();
    const QString translation_en_to_ru_easy_absolute_path = translation_en_to_ru_easy_db_file.absoluteFilePath();
    const QString translation_en_to_ru_hard_absolute_path = translation_en_to_ru_hard_db_file.absoluteFilePath();

    qDebug() << "Database paths:";
    qDebug() << "Grammar Test Easy:" << grammar_test_easy_absolute_path;
    qDebug() << "Grammar Test Hard:" << grammar_test_hard_absolute_path;
    qDebug() << "Grammar Question Easy:" << grammar_question_easy_absolute_path;
    qDebug() << "Grammar Question Hard:" << grammar_question_hard_absolute_path;
    qDebug() << "Translation RU->EN Easy:" << translation_ru_to_en_easy_absolute_path;
    qDebug() << "Translation RU->EN Hard:" << translation_ru_to_en_hard_absolute_path;
    qDebug() << "Translation EN->RU Easy:" << translation_en_to_ru_easy_absolute_path;
    qDebug() << "Translation EN->RU Hard:" << translation_en_to_ru_hard_absolute_path;

    grammar_test_easy_stats_.resize(2);
    grammar_test_hard_stats_.resize(2);
    grammar_gap_fill_easy_stats_.resize(2);
    grammar_gap_fill_hard_stats_.resize(2);
    translation_test_ru_to_en_easy_stats_.resize(2);
    translation_test_ru_to_en_hard_stats_.resize(2);
    translation_test_en_to_ru_easy_stats_.resize(2);
    translation_test_en_to_ru_hard_stats_.resize(2);

    if (!InitializeDatabase(
        grammar_test_easy_absolute_path,
        grammar_test_hard_absolute_path,
        grammar_question_easy_absolute_path,
        grammar_question_hard_absolute_path,
        translation_ru_to_en_easy_absolute_path,
        translation_ru_to_en_hard_absolute_path,
        translation_en_to_ru_easy_absolute_path,
        translation_en_to_ru_hard_absolute_path
    )) {
        qCritical() << "Failed to initialize databases!";
    }
}

Controller::DifficultyLevel Controller::GetDifficulty() const {
    return difficulty_;
}

void Controller::SetDifficulty(DifficultyLevel difficulty) {
    difficulty_ = difficulty;
}

int Controller::GetBall() const {
    return ball_;
}

void Controller::SetBall(const int ball) {
    ball_ = ball;
}

bool Controller::InitializeDatabase(const QString &grammar_test_easy_db_path,
                                    const QString &grammar_test_hard_db_path,
                                    const QString &grammar_question_easy_db_path,
                                    const QString &grammar_question_hard_db_path,
                                    const QString &translation_ru_to_en_easy_db_path,
                                    const QString &translation_ru_to_en_hard_db_path,
                                    const QString &translation_en_to_ru_easy_db_path,
                                    const QString &translation_en_to_ru_hard_db_path) {
    // Проверка наличия файлов БД
    const QFileInfo grammar_test_easy_db_file(grammar_test_easy_db_path);
    if (!grammar_test_easy_db_file.exists()) {
        qCritical() << "Database file (Grammar Test Easy) not found:" << grammar_test_easy_db_path;
        return false;
    }
    const QFileInfo grammar_test_hard_db_file(grammar_test_hard_db_path);
    if (!grammar_test_hard_db_file.exists()) {
        qCritical() << "Database file (Grammar Test Hard) not found:" << grammar_test_hard_db_path;
        return false;
    }
    const QFileInfo grammar_question_easy_db_file(grammar_question_easy_db_path);
    if (!grammar_question_easy_db_file.exists()) {
        qCritical() << "Database file (Grammar Question Easy) not found:" << grammar_question_easy_db_path;
        return false;
    }
    const QFileInfo grammar_question_hard_db_file(grammar_question_hard_db_path);
    if (!grammar_question_hard_db_file.exists()) {
        qCritical() << "Database file (Grammar Question Hard) not found:" << grammar_question_hard_db_path;
        return false;
    }

    const QFileInfo translation_ru_to_en_easy_db_file(translation_ru_to_en_easy_db_path);
    if (!translation_ru_to_en_easy_db_file.exists()) {
        qCritical() << "Database file (Translation RU->EN Easy) not found:" << translation_ru_to_en_easy_db_path;
        return false;
    }
    const QFileInfo translation_ru_to_en_hard_db_file(translation_ru_to_en_hard_db_path);
    if (!translation_ru_to_en_hard_db_file.exists()) {
        qCritical() << "Database file (Translation RU->EN Hard) not found:" << translation_ru_to_en_hard_db_path;
        return false;
    }

    const QFileInfo translation_en_to_ru_easy_db_file(translation_en_to_ru_easy_db_path);
    if (!translation_en_to_ru_easy_db_file.exists()) {
        qCritical() << "Database file (Translation EN->RU Easy) not found:" << translation_en_to_ru_easy_db_path;
        return false;
    }
    const QFileInfo translation_en_to_ru_hard_db_file(translation_en_to_ru_hard_db_path);
    if (!translation_en_to_ru_hard_db_file.exists()) {
        qCritical() << "Database file (Translation EN->RU Hard) not found:" << translation_en_to_ru_hard_db_path;
        return false;
    }

    // Подключение к grammar_test_easy
    grammar_test_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_test_easy");
    grammar_test_easy_db_.setDatabaseName(grammar_test_easy_db_path);
    if (!grammar_test_easy_db_.open()) {
        qCritical() << "Error opening database (Grammar Test Easy):" << grammar_test_easy_db_.lastError().text();
        return false;
    }
    QSqlQuery grammar_test_easy_check_query(grammar_test_easy_db_);
    if (!grammar_test_easy_check_query.exec(
            "SELECT name FROM sqlite_master WHERE type='table' AND name='grammartesteasy_questions'") ||
        !grammar_test_easy_check_query.next()) {
        qCritical() << "Table grammartesteasy_questions not found!";
        grammar_test_easy_db_.close();
        return false;
    }

    // Подключение к grammar_test_hard
    grammar_test_hard_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_test_hard");
    grammar_test_hard_db_.setDatabaseName(grammar_test_hard_db_path);
    if (!grammar_test_hard_db_.open()) {
        qCritical() << "Error opening database (Grammar Test Hard):" << grammar_test_hard_db_.lastError().text();
        grammar_test_easy_db_.close();
        return false;
    }
    QSqlQuery grammar_test_hard_check_query(grammar_test_hard_db_);
    if (!grammar_test_hard_check_query.exec(
            "SELECT name FROM sqlite_master WHERE type='table' AND name='grammartesthard_questions'") ||
        !grammar_test_hard_check_query.next()) {
        qCritical() << "Table grammartesthard_questions not found!";
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        return false;
    }

    // Подключение к grammar_question_easy
    grammar_question_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_question_easy");
    grammar_question_easy_db_.setDatabaseName(grammar_question_easy_db_path);
    if (!grammar_question_easy_db_.open()) {
        qCritical() << "Error opening database (Grammar Question Easy):" << grammar_question_easy_db_.lastError().
                text();
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        return false;
    }
    QSqlQuery grammar_question_easy_check_query(grammar_question_easy_db_);
    if (!grammar_question_easy_check_query.exec(
            "SELECT name FROM sqlite_master WHERE type='table' AND name='grammarquestioneasy_questions'") ||
        !grammar_question_easy_check_query.next()) {
        qCritical() << "Table grammarquestioneasy_questions not found!";
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        return false;
    }

    // Подключение к grammar_question_hard
    grammar_question_hard_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_question_hard");
    grammar_question_hard_db_.setDatabaseName(grammar_question_hard_db_path);
    if (!grammar_question_hard_db_.open()) {
        qCritical() << "Error opening database (Grammar Question Hard):" << grammar_question_hard_db_.lastError().
                text();
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        return false;
    }
    QSqlQuery grammar_question_hard_check_query(grammar_question_hard_db_);
    if (!grammar_question_hard_check_query.exec(
            "SELECT name FROM sqlite_master WHERE type='table' AND name='grammarquestionhard_questions'") ||
        !grammar_question_hard_check_query.next()) {
        qCritical() << "Table grammarquestionhard_questions not found!";
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        grammar_question_hard_db_.close();
        return false;
    }

    // Подключение к translation_ru_to_en_easy
    translation_ru_to_en_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "translation_ru_to_en_easy");
    translation_ru_to_en_easy_db_.setDatabaseName(translation_ru_to_en_easy_db_path);
    if (!translation_ru_to_en_easy_db_.open()) {
        qCritical() << "Error opening database (Translation RU->EN Easy):" << translation_ru_to_en_easy_db_.lastError().
                text();
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        grammar_question_hard_db_.close();
        return false;
    }
    QSqlQuery translation_ru_to_en_easy_check_query(translation_ru_to_en_easy_db_);
    if (!translation_ru_to_en_easy_check_query.exec(
            "SELECT name FROM sqlite_master WHERE type='table' AND name='translationrutoeneasy_questions'") ||
        !translation_ru_to_en_easy_check_query.next()) {
        qCritical() << "Table translationrutoeneasy_questions not found!";
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        grammar_question_hard_db_.close();
        translation_ru_to_en_easy_db_.close();
        return false;
    }

    // Подключение к translation_ru_to_en_hard
    translation_ru_to_en_hard_db_ = QSqlDatabase::addDatabase("QSQLITE", "translation_ru_to_en_hard");
    translation_ru_to_en_hard_db_.setDatabaseName(translation_ru_to_en_hard_db_path);
    if (!translation_ru_to_en_hard_db_.open()) {
        qCritical() << "Error opening database (Translation RU->EN Hard):" << translation_ru_to_en_hard_db_.lastError().
                text();
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        grammar_question_hard_db_.close();
        translation_ru_to_en_easy_db_.close();
        return false;
    }
    QSqlQuery translation_ru_to_en_hard_check_query(translation_ru_to_en_hard_db_);
    if (!translation_ru_to_en_hard_check_query.exec(
            "SELECT name FROM sqlite_master WHERE type='table' AND name='translationrutoenhard_questions'") ||
        !translation_ru_to_en_hard_check_query.next()) {
        qCritical() << "Table translationrutoenhard_questions not found!";
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        grammar_question_hard_db_.close();
        translation_ru_to_en_easy_db_.close();
        translation_ru_to_en_hard_db_.close();
        return false;
    }

    // Подключение к translation_en_to_ru_easy
    translation_en_to_ru_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "translation_en_to_ru_easy");
    translation_en_to_ru_easy_db_.setDatabaseName(translation_en_to_ru_easy_db_path);
    if (!translation_en_to_ru_easy_db_.open()) {
        qCritical() << "Error opening database (Translation EN->RU Easy):" << translation_en_to_ru_easy_db_.lastError().
                text();
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        grammar_question_hard_db_.close();
        translation_ru_to_en_easy_db_.close();
        translation_ru_to_en_hard_db_.close();
        return false;
    }
    QSqlQuery translation_en_to_ru_easy_check_query(translation_en_to_ru_easy_db_);
    if (!translation_en_to_ru_easy_check_query.exec(
            "SELECT name FROM sqlite_master WHERE type='table' AND name='translationentorueasy_questions'") ||
        !translation_en_to_ru_easy_check_query.next()) {
        qCritical() << "Table translationentorueasy_questions not found!";
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        grammar_question_hard_db_.close();
        translation_ru_to_en_easy_db_.close();
        translation_ru_to_en_hard_db_.close();
        translation_en_to_ru_easy_db_.close();
        return false;
    }

    // Подключение к translation_en_to_ru_hard
    translation_en_to_ru_hard_db_ = QSqlDatabase::addDatabase("QSQLITE", "translation_en_to_ru_hard");
    translation_en_to_ru_hard_db_.setDatabaseName(translation_en_to_ru_hard_db_path);
    if (!translation_en_to_ru_hard_db_.open()) {
        qCritical() << "Error opening database (Translation EN->RU Hard):" << translation_en_to_ru_hard_db_.lastError().
                text();
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        grammar_question_hard_db_.close();
        translation_ru_to_en_easy_db_.close();
        translation_ru_to_en_hard_db_.close();
        translation_en_to_ru_easy_db_.close();
        return false;
    }
    QSqlQuery translation_en_to_ru_hard_check_query(translation_en_to_ru_hard_db_);
    if (!translation_en_to_ru_hard_check_query.exec(
            "SELECT name FROM sqlite_master WHERE type='table' AND name='translationentoruhard_questions'") ||
        !translation_en_to_ru_hard_check_query.next()) {
        qCritical() << "Table translationentoruhard_questions not found!";
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        grammar_question_easy_db_.close();
        grammar_question_hard_db_.close();
        translation_ru_to_en_easy_db_.close();
        translation_ru_to_en_hard_db_.close();
        translation_en_to_ru_easy_db_.close();
        translation_en_to_ru_hard_db_.close();
        return false;
    }

    qDebug() << "All databases initialized successfully";
    return true;
}

Controller::GrammarQuestion Controller::GetNextGrammarQuestion(int question_index,
                                                               QuestionType type,
                                                               DifficultyLevel difficulty) {
    GrammarQuestion question;
    question.type = type;

    // Determine which database to use based on question type and difficulty
    QSqlDatabase *db = nullptr;
    QString table_name;

    if (type == QuestionType::MultipleChoice) {
        db = (difficulty == DifficultyLevel::Easy) ? &grammar_test_easy_db_ : &grammar_test_hard_db_;
        table_name = (difficulty == DifficultyLevel::Easy) ? "grammartesteasy_questions" : "grammartesthard_questions";
    } else { // GapFill
        db = (difficulty == DifficultyLevel::Easy) ? &grammar_question_easy_db_ : &grammar_question_hard_db_;
        table_name = (difficulty == DifficultyLevel::Easy) ? "grammarquestioneasy_questions" : "grammarquestionhard_questions";
    }

    if (!db || !db->isOpen()) {
        question.question = "Database not open";
        return question;
    }

    QSqlQuery query(*db);

    if (type == QuestionType::MultipleChoice) {
        query.prepare(QString("SELECT question, correct_answers, option1, option2, option3, option4, hint "
            "FROM %1 WHERE id = ?").arg(table_name));
    } else { // GapFill
        query.prepare(QString("SELECT question, correct_answers, hint "
            "FROM %1 WHERE id = ?").arg(table_name));
    }

    query.addBindValue(question_index);

    if (query.exec() && query.next()) {
        question.question = query.value("question").toString();
        question.correct_answers = query.value("correct_answers").toString().split("|", Qt::SkipEmptyParts);
        question.hint = query.value("hint").toString();

        // For MultipleChoice questions, collect and shuffle options
        if (type == QuestionType::MultipleChoice) {
            // Collect answer options
            for (int i = 1; i <= 4; ++i) {
                QString opt = query.value("option" + QString::number(i)).toString();
                if (!opt.isEmpty() && opt != "null") {
                    question.options << opt;
                }
            }

            // Shuffle options for multiple choice questions
            ShuffleOptions(question);
        } else {
            // For GapFill questions, we might want to include some common options
            // or leave options empty for free-form answer
            question.options.clear();
        }
    } else {
        question.question = "Question not found or query error";
        qDebug() << "Query error:" << query.lastError().text();
        qDebug() << "Executed query:" << query.lastQuery();
    }

    return question;
}

Controller::TranslationQuestion Controller::GetNextTranslationQuestion(int question_index,
                                                                      QuestionType type,
                                                                      DifficultyLevel difficulty) {
    TranslationQuestion question;
    question.type = type;

    // Determine which database to use based on question type and difficulty
    QSqlDatabase *db = nullptr;
    QString table_name;

    if (type == QuestionType::TranslationRuToEn) {
        db = (difficulty == DifficultyLevel::Easy) ? &translation_ru_to_en_easy_db_ : &translation_ru_to_en_hard_db_;
        table_name = (difficulty == DifficultyLevel::Easy) ? "translationrutoeneasy_questions" : "translationrutoenhard_questions";
    } else { // TranslationEnToRu
        db = (difficulty == DifficultyLevel::Easy) ? &translation_en_to_ru_easy_db_ : &translation_en_to_ru_hard_db_;
        table_name = (difficulty == DifficultyLevel::Easy) ? "translationentorueasy_questions" : "translationentoruhard_questions";
    }

    if (!db || !db->isOpen()) {
        question.source_text = "Database not open";
        qDebug() << "Error: Database not open for type:" << static_cast<int>(type)
                 << "difficulty:" << static_cast<int>(difficulty)
                 << "table:" << table_name;
        return question;
    }

    QSqlQuery query(*db);
    query.prepare(QString("SELECT question, correct_answers, hint FROM %1 WHERE id = :id").arg(table_name));
    query.bindValue(":id", question_index);

    if (!query.exec()) {
        question.source_text = "Query execution failed";
        qDebug() << "Query error:" << query.lastError().text();
        qDebug() << "Executed query:" << query.lastQuery();
        qDebug() << "Bound value (id):" << question_index << "table:" << table_name;
        return question;
    }

    if (query.next()) {
        question.source_text = query.value("question").toString();
        question.correct_translations = query.value("correct_answers").toString().split("|", Qt::SkipEmptyParts);
        question.hint = query.value("hint").toString();
        qDebug() << "Fetched question id:" << question_index << "question:" << question.source_text;
    } else {
        question.source_text = "Question not found";
        qDebug() << "No question found for id:" << question_index << "in table:" << table_name;
    }

    return question;
}

std::vector<Controller::GrammarQuestion> Controller::RequestGrammarQuestionSet(
    QuestionType type,
    DifficultyLevel difficulty,
    TestStats &stats) {
    std::vector<GrammarQuestion> questions;
    stats.Clear();
    stats.type = type;
    stats.difficulty = difficulty;
    stats.testId = 0;
    stats.answers.resize(kTestSize, AnswerType::NoAnswer);

    // Determine which stats vector to use based on question type and difficulty
    std::vector<TestStats>* stats_vector = nullptr;

    if (type == QuestionType::MultipleChoice) {
        stats_vector = (difficulty == DifficultyLevel::Easy)
            ? &grammar_test_easy_stats_
            : &grammar_test_hard_stats_;
    } else { // GapFill
        stats_vector = (difficulty == DifficultyLevel::Easy)
            ? &grammar_gap_fill_easy_stats_
            : &grammar_gap_fill_hard_stats_;
    }

    // Determine which set of questions to use based on completed tests
    int set_index = 0;
    bool all_sets_completed = true;

    // Check stats for completed tests
    for (size_t i = 0; i < stats_vector->size(); ++i) {
        if ((*stats_vector)[i].questionsAnswered < kTestSize) {
            all_sets_completed = false;
            set_index = i;
            break;
        }
    }

    // If all sets are completed, return -1 as testId
    if (all_sets_completed) {
        stats.testId = -1;
        GrammarQuestion error_question;
        error_question.question = "All question sets completed";
        questions.push_back(error_question);
        return questions;
    }

    // Calculate starting index for the question set
    int start_index = (set_index * kTestSize) + 1;

    // Populate questions vector
    for (int i = 0; i < kTestSize; ++i) {
        GrammarQuestion question = GetNextGrammarQuestion(start_index + i, type, difficulty);
        questions.push_back(question);
    }

    // Update testId in stats
    stats.testId = set_index + 1;

    // Ensure stats vector is large enough
    if (static_cast<size_t>(stats.testId) > stats_vector->size()) {
        stats_vector->resize(stats.testId);
    }

    qDebug() << "Generated question set for testId:" << stats.testId
             << "with startIndex:" << start_index
             << "type:" << (type == QuestionType::MultipleChoice ? "MultipleChoice" : "GapFill")
             << "difficulty:" << (difficulty == DifficultyLevel::Easy ? "Easy" : "Hard");

    return questions;
}

std::vector<Controller::TranslationQuestion> Controller::RequestTranslationQuestionSet(
    QuestionType type,
    DifficultyLevel difficulty,
    TestStats &stats) {
    std::vector<TranslationQuestion> questions;
    stats.Clear();
    stats.type = type;
    stats.difficulty = difficulty;
    stats.testId = 0;
    stats.answers.resize(kTestSize, AnswerType::NoAnswer);

    // Determine which stats vector to use based on question type and difficulty
    std::vector<TestStats>* stats_vector = nullptr;

    if (type == QuestionType::TranslationRuToEn) {
        stats_vector = (difficulty == DifficultyLevel::Easy)
            ? &translation_test_ru_to_en_easy_stats_
            : &translation_test_ru_to_en_hard_stats_;
    } else { // TranslationEnToRu
        stats_vector = (difficulty == DifficultyLevel::Easy)
            ? &translation_test_en_to_ru_easy_stats_
            : &translation_test_en_to_ru_hard_stats_;
    }

    // Determine which set of questions to use based on completed tests
    int set_index = 0;
    bool all_sets_completed = true;

    // Check stats for completed tests
    for (size_t i = 0; i < stats_vector->size(); ++i) {
        if ((*stats_vector)[i].questionsAnswered < kTestSize) {
            all_sets_completed = false;
            set_index = i;
            break;
        }
    }

    // If all sets are completed, return -1 as testId
    if (all_sets_completed) {
        stats.testId = -1;
        TranslationQuestion error_question;
        error_question.source_text = "All question sets completed";
        questions.push_back(error_question);
        return questions;
    }

    // Calculate starting index for the question set
    int start_index = (set_index * kTestSize) + 1;

    // Populate questions vector
    for (int i = 0; i < kTestSize; ++i) {
        TranslationQuestion question = GetNextTranslationQuestion(start_index + i, type, difficulty);
        questions.push_back(question);
    }

    // Update testId in stats
    stats.testId = set_index + 1;

    // Ensure stats vector is large enough
    if (static_cast<size_t>(stats.testId) > stats_vector->size()) {
        stats_vector->resize(stats.testId);
    }

    qDebug() << "Generated translation question set for testId:" << stats.testId
             << "with startIndex:" << start_index
             << "type:" << (type == QuestionType::TranslationRuToEn ? "TranslationRuToEn" : "TranslationEnToRu")
             << "difficulty:" << (difficulty == DifficultyLevel::Easy ? "Easy" : "Hard");

    return questions;
}

void Controller::SendDataAboutTest(QuestionType type, DifficultyLevel difficulty, const TestStats &stats) {
    std::vector<TestStats>* stats_vector = nullptr;

    if (type == QuestionType::MultipleChoice) {
        stats_vector = (difficulty == DifficultyLevel::Easy)
            ? &grammar_test_easy_stats_
            : &grammar_test_hard_stats_;
    } else if (type == QuestionType::GapFill) {
        stats_vector = (difficulty == DifficultyLevel::Easy)
            ? &grammar_gap_fill_easy_stats_
            : &grammar_gap_fill_hard_stats_;
    } else if (type == QuestionType::TranslationRuToEn) {
        stats_vector = (difficulty == DifficultyLevel::Easy)
            ? &translation_test_ru_to_en_easy_stats_
            : &translation_test_ru_to_en_hard_stats_;
    } else { // TranslationEnToRu
        stats_vector = (difficulty == DifficultyLevel::Easy)
            ? &translation_test_en_to_ru_easy_stats_
            : &translation_test_en_to_ru_hard_stats_;
    }

    if (stats.testId >= 1 && stats.testId <= static_cast<int>(stats_vector->size())) {
        (*stats_vector)[stats.testId - 1] = stats;
        qDebug() << "Saved stats for test" << stats.testId
                 << "type:" << (type == QuestionType::MultipleChoice ? "MultipleChoice" :
                                type == QuestionType::GapFill ? "GapFill" :
                                type == QuestionType::TranslationRuToEn ? "TranslationRuToEn" : "TranslationEnToRu")
                 << "difficulty:" << (difficulty == DifficultyLevel::Easy ? "Easy" : "Hard");
    }

    if (stats.rightAnswers == kTestSize) {
        if (difficulty == DifficultyLevel::Easy) {
            ball_++;
        } else {
            ball_ += 2;
        }
    }
}

void Controller::ShuffleOptions(GrammarQuestion &question) {
    for (int i = question.options.size() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        question.options.swapItemsAt(i, j);
    }
}

void Controller::TestStats::Clear() {
    questionsAnswered = 0;
    timeElapsed = 0;
    rightAnswers = 0;
    testId = 0;
    mistakes = 0;
    answers.clear();
}

bool Controller::CheckAnswer(const QString &user_answer, const QStringList &correct_answers) const {
    // Normalize user answer: keep only letters, convert to lowercase
    QString normalized_user_answer = user_answer.toLower();
    QStringList user_words;
    QString current_word;
    for (const QChar &c : normalized_user_answer) {
        if (c.isLetter()) {
            current_word += c;
        } else if (!current_word.isEmpty()) {
            user_words << current_word;
            current_word.clear();
        }
    }
    if (!current_word.isEmpty()) {
        user_words << current_word;
    }

    // Check against each correct answer
    for (const QString &correct_answer : correct_answers) {
        // Normalize correct answer
        QString normalized_correct_answer = correct_answer.toLower();
        QStringList correct_words;
        current_word.clear();
        for (const QChar &c : normalized_correct_answer) {
            if (c.isLetter()) {
                current_word += c;
            } else if (!current_word.isEmpty()) {
                correct_words << current_word;
                current_word.clear();
            }
        }
        if (!current_word.isEmpty()) {
            correct_words << current_word;
        }

        // Compare word lists
        if (user_words == correct_words) {
            return true;
        }
    }

    return false;
}