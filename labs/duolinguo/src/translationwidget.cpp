/*#include "controller.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QSqlError>
#include <QSqlQuery>
#include <cstddef>

Controller::Controller() {
    const QString grammar_test_easy_db_path = "labs/duolinguo/src/db/grammar_test_easy.db";
    const QString grammar_test_hard_db_path = "labs/duolinguo/src/db/grammar_test_hard.db";
    const QString grammar_question_easy_db_path = "labs/duolinguo/src/db/grammar_question_easy.db";
    const QString grammar_question_hard_db_path = "labs/duolinguo/src/db/grammar_question_hard.db";
    const QString translation_en_to_ru_easy_db_path = "labs/duolinguo/src/db/translation_en_to_ru_easy.db";
    const QString translation_en_to_ru_hard_db_path = "labs/duolinguo/src/db/translation_en_to_ru_hard.db";
    const QString translation_ru_to_en_easy_db_path = "labs/duolinguo/src/db/translation_ru_to_en_easy.db";
    const QString translation_ru_to_en_hard_db_path = "labs/duolinguo/src/db/translation_ru_to_en_hard.db";

    const QFileInfo grammar_test_easy_db_file(grammar_test_easy_db_path);
    const QFileInfo grammar_test_hard_db_file(grammar_test_hard_db_path);
    const QFileInfo grammar_question_easy_db_file(grammar_question_easy_db_path);
    const QFileInfo grammar_question_hard_db_file(grammar_question_hard_db_path);
    const QFileInfo translation_en_to_ru_easy_db_file(translation_en_to_ru_easy_db_path);
    const QFileInfo translation_en_to_ru_hard_db_file(translation_en_to_ru_hard_db_path);
    const QFileInfo translation_ru_to_en_easy_db_file(translation_ru_to_en_easy_db_path);
    const QFileInfo translation_ru_to_en_hard_db_file(translation_ru_to_en_hard_db_path);

    const QString grammar_test_easy_absolute_path = grammar_test_easy_db_file.absoluteFilePath();
    const QString grammar_test_hard_absolute_path = grammar_test_hard_db_file.absoluteFilePath();
    const QString grammar_question_easy_absolute_path = grammar_question_easy_db_file.absoluteFilePath();
    const QString grammar_question_hard_absolute_path = grammar_question_hard_db_file.absoluteFilePath();
    const QString translation_en_to_ru_easy_absolute_path = translation_en_to_ru_easy_db_file.absoluteFilePath();
    const QString translation_en_to_ru_hard_absolute_path = translation_en_to_ru_hard_db_file.absoluteFilePath();
    const QString translation_ru_to_en_easy_absolute_path = translation_ru_to_en_easy_db_file.absoluteFilePath();
    const QString translation_ru_to_en_hard_absolute_path = translation_ru_to_en_hard_db_file.absoluteFilePath();

    grammar_test_easy_stats_.resize(2);
    grammar_test_hard_stats_.resize(2);

    if (!InitializeDatabase(grammar_test_easy_absolute_path, grammar_test_hard_absolute_path,
                            grammar_question_easy_absolute_path, grammar_question_hard_absolute_path,
                            translation_en_to_ru_easy_absolute_path, translation_en_to_ru_hard_absolute_path,
                            translation_ru_to_en_easy_absolute_path, translation_ru_to_en_hard_absolute_path)) {
        qCritical() << "Не удалось инициализировать базы данных!";
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

bool Controller::InitializeDatabase(
    const QString &grammar_test_easy_db_path,
    const QString &grammar_test_hard_db_path,
    const QString &grammar_question_easy_db_path,
    const QString &grammar_question_hard_db_path,
    const QString &translation_en_to_ru_easy_db_path,
    const QString &translation_en_to_ru_hard_db_path,
    const QString &translation_ru_to_en_easy_db_path,
    const QString &translation_ru_to_en_hard_db_path) {
    // Проверяем существование файлов баз данных
    const QFileInfo easy_db_file(grammar_test_easy_db_path);
    if (!easy_db_file.exists()) {
        qCritical() << "Файл базы данных (Grammar Test Easy) не найден:" << grammar_test_easy_db_path;
        return false;
    }

    const QFileInfo hard_db_file(grammar_test_hard_db_path);
    if (!hard_db_file.exists()) {
        qCritical() << "Файл базы данных (Grammar Test Hard) не найден:" << grammar_test_hard_db_path;
        return false;
    }

    const QFileInfo grammar_question_easy_db_file(grammar_question_easy_db_path);
    if (!grammar_question_easy_db_file.exists()) {
        qCritical() << "Файл базы данных (Grammar Question Easy) не найден:" << grammar_question_easy_db_path;
        return false;
    }

    const QFileInfo grammar_question_hard_db_file(grammar_question_hard_db_path);
    if (!grammar_question_hard_db_file.exists()) {
        qCritical() << "Файл базы данных (Grammar Question Hard) не найден:" << grammar_question_hard_db_path;
        return false;
    }

    const QFileInfo translation_en_to_ru_easy_db_file(translation_en_to_ru_easy_db_path);
    if (!translation_en_to_ru_easy_db_file.exists()) {
        qCritical() << "Файл базы данных (Translation En->Ru Easy) не найден:" << translation_en_to_ru_easy_db_path;
        return false;
    }

    const QFileInfo translation_en_to_ru_hard_db_file(translation_en_to_ru_hard_db_path);
    if (!translation_en_to_ru_hard_db_file.exists()) {
        qCritical() << "Файл базы данных (Translation En->Ru Hard) не найден:" << translation_en_to_ru_hard_db_path;
        return false;
    }

    const QFileInfo translation_ru_to_en_easy_db_file(translation_ru_to_en_easy_db_path);
    if (!translation_ru_to_en_easy_db_file.exists()) {
        qCritical() << "Файл базы данных (Translation Ru->En Easy) не найден:" << translation_ru_to_en_easy_db_path;
        return false;
    }

    const QFileInfo translation_ru_to_en_hard_db_file(translation_ru_to_en_hard_db_path);
    if (!translation_ru_to_en_hard_db_file.exists()) {
        qCritical() << "Файл базы данных (Translation Ru->En Hard) не найден:" << translation_ru_to_en_hard_db_path;
        return false;
    }

    grammar_test_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_test_easy");
    grammar_test_easy_db_.setDatabaseName(grammar_test_easy_db_path);

    grammar_test_hard_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_test_hard");
    grammar_test_hard_db_.setDatabaseName(grammar_test_hard_db_path);

    grammar_question_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_question_easy");
    grammar_question_easy_db_.setDatabaseName(grammar_question_easy_db_path);

    grammar_question_hard_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_question_hard");
    grammar_question_hard_db_.setDatabaseName(grammar_question_hard_db_path);

    translation_en_to_ru_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "translation_en_to_ru_easy");
    translation_en_to_ru_easy_db_.setDatabaseName(translation_en_to_ru_easy_db_path);

    translation_en_to_ru_hard_db_ = QSqlDatabase::addDatabase("QSQLITE", "translation_en_to_ru_hard");
    translation_en_to_ru_hard_db_.setDatabaseName(translation_en_to_ru_hard_db_path);

    translation_ru_to_en_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "translation_ru_to_en_easy");
    translation_ru_to_en_easy_db_.setDatabaseName(translation_ru_to_en_easy_db_path);

    translation_ru_to_en_hard_db_ = QSqlDatabase::addDatabase("QSQLITE", "translation_ru_to_en_hard");
    translation_ru_to_en_hard_db_.setDatabaseName(translation_ru_to_en_hard_db_path);

    bool all_opened = true;

    if (!grammar_test_easy_db_.open()) {
        qCritical() << "Ошибка открытия базы данных (Grammar Test Easy):" << grammar_test_easy_db_.lastError().text();
        all_opened = false;
    }

    if (!grammar_test_hard_db_.open()) {
        qCritical() << "Ошибка открытия базы данных (Grammar Test Hard):" << grammar_test_hard_db_.lastError().text();
        all_opened = false;
    }

    if (!grammar_question_easy_db_.open()) {
        qCritical() << "Ошибка открытия базы данных (Grammar Question Easy):" << grammar_question_easy_db_.lastError().
                text();
        all_opened = false;
    }

    if (!grammar_question_hard_db_.open()) {
        qCritical() << "Ошибка открытия базы данных (Grammar Question Hard):" << grammar_question_hard_db_.lastError().
                text();
        all_opened = false;
    }

    if (!translation_en_to_ru_easy_db_.open()) {
        qCritical() << "Ошибка открытия базы данных (Translation En->Ru Easy):" << translation_en_to_ru_easy_db_.
                lastError().text();
        all_opened = false;
    }

    if (!translation_en_to_ru_hard_db_.open()) {
        qCritical() << "Ошибка открытия базы данных (Translation En->Ru Hard):" << translation_en_to_ru_hard_db_.
                lastError().text();
        all_opened = false;
    }

    if (!translation_ru_to_en_easy_db_.open()) {
        qCritical() << "Ошибка открытия базы данных (Translation Ru->En Easy):" << translation_ru_to_en_easy_db_.
                lastError().text();
        all_opened = false;
    }

    if (!translation_ru_to_en_hard_db_.open()) {
        qCritical() << "Ошибка открытия базы данных (Translation Ru->En Hard):" << translation_ru_to_en_hard_db_.
                lastError().text();
        all_opened = false;
    }

    if (!all_opened) {
        if (grammar_test_easy_db_.isOpen()) {
            grammar_test_easy_db_.close();
        }
        if (grammar_test_hard_db_.isOpen()) {
            grammar_test_hard_db_.close();
        }
        if (grammar_question_easy_db_.isOpen()) {
            grammar_question_easy_db_.close();
        }
        if (grammar_question_hard_db_.isOpen()) {
            grammar_question_hard_db_.close();
        }
        if (translation_en_to_ru_easy_db_.isOpen()) {
            translation_en_to_ru_easy_db_.close();
        }
        if (translation_en_to_ru_hard_db_.isOpen()) {
            translation_en_to_ru_hard_db_.close();
        }
        if (translation_ru_to_en_easy_db_.isOpen()) {
            translation_ru_to_en_easy_db_.close();
        }
        if (translation_ru_to_en_hard_db_.isOpen()) {
            translation_ru_to_en_hard_db_.close();
        }

        return false;
    }

    qDebug() << "Все базы данных успешно инициализированы";
    return true;
}

Controller::GrammarQuestion Controller::GetNextGrammarQuestion(int question_index,
                                                               QuestionType type,
                                                               DifficultyLevel difficulty) {
    GrammarQuestion question;
    question.type = type;

    QSqlDatabase *db = (difficulty == DifficultyLevel::Easy) ? &grammar_test_easy_db_ : &grammar_test_hard_db_;
    QString tableName = (difficulty == DifficultyLevel::Easy)
                            ? "grammartesteasy_questions"
                            : "grammartesthard_questions";

    if (!db->isOpen()) {
        question.question = "База данных не открыта";
        return question;
    }

    QSqlQuery query(*db);
    query.prepare(QString("SELECT question, correct_answers, option1, option2, option3, option4, hint "
        "FROM %1 WHERE id = ?").arg(tableName));
    query.addBindValue(question_index);

    if (query.exec() && query.next()) {
        question.question = query.value("question").toString();
        question.correct_answers = query.value("correct_answers").toString().split("|");
        question.hint = query.value("hint").toString();

        // Собираем варианты ответов
        for (int i = 1; i <= 4; ++i) {
            QString opt = query.value("option" + QString::number(i)).toString();
            if (!opt.isEmpty()) {
                question.options << opt;
            }
        }

        // Перемешиваем варианты для вопросов с выбором
        if (type == QuestionType::MultipleChoice) {
            ShuffleOptions(question);
        }
    } else {
        question.question = "Вопрос не найден";
        qDebug() << "Ошибка запроса:" << query.lastError().text();
    }

    return question;
}

Controller::TranslationQuestion Controller::GetNextTranslationQuestion() {
    TranslationQuestion question;
    question.source_text = "Перевод не реализован";
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

    std::vector<TestStats> &statsVector = (difficulty == DifficultyLevel::Easy)
                                              ? grammar_test_easy_stats_
                                              : grammar_test_hard_stats_;

    int set_index = 0;
    bool all_sets_completed = true;

    for (size_t i = 0; i < statsVector.size(); ++i) {
        if (statsVector[i].questionsAnswered < kTestSize) {
            all_sets_completed = false;
            set_index = i;
            break;
        }
    }

    if (all_sets_completed) {
        stats.testId = -1;
        GrammarQuestion error_question;
        error_question.question = "Все наборы вопросов завершены";
        questions.push_back(error_question);
        return questions;
    }

    int start_index = (set_index * kTestSize) + 1;

    for (int i = 0; i < kTestSize; ++i) {
        GrammarQuestion question = GetNextGrammarQuestion(start_index + i, type, difficulty);
        questions.push_back(question);
    }

    stats.testId = set_index + 1;

    if (static_cast<size_t>(stats.testId) > statsVector.size()) {
        statsVector.resize(stats.testId);
    }

    qDebug() << "Generated question set for testId:" << stats.testId
            << "with startIndex:" << start_index
            << "difficulty:" << (difficulty == DifficultyLevel::Easy ? "Easy" : "Hard");

    return questions;
}

void Controller::SendDataAboutTest(QuestionType type, DifficultyLevel difficulty, const TestStats &stats) {
    std::vector<TestStats> &stats_vector = (difficulty == DifficultyLevel::Easy)
                                               ? grammar_test_easy_stats_
                                               : grammar_test_hard_stats_;

    if (stats.testId >= 1 && stats.testId <= static_cast<int>(stats_vector.size())) {
        stats_vector[stats.testId - 1] = stats;
        qDebug() << "Сохранена статистика для теста" << stats.testId
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
*/