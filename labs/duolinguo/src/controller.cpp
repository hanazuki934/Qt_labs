#include "controller.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QSqlError>
#include <QSqlQuery>

Controller::Controller(const QString &grammartesteasy_db_path,
                      const QString &grammartesthard_db_path,
                      const QString &)
{
    const QString easyDbPath = "labs/duolinguo/src/db/grammartesteasy.db";
    const QString hardDbPath = "labs/duolinguo/src/db/grammartesthard.db";

    // Получаем абсолютные пути
    QFileInfo easyDbFile(easyDbPath);
    QFileInfo hardDbFile(hardDbPath);
    QString easyAbsolutePath = easyDbFile.absoluteFilePath();
    QString hardAbsolutePath = hardDbFile.absoluteFilePath();

    qDebug() << "Путь к базе данных (Easy):" << easyAbsolutePath;
    qDebug() << "Путь к базе данных (Hard):" << hardAbsolutePath;

    grammar_test_easy_stats_.resize(2);
    grammar_test_hard_stats_.resize(2);

    if (!InitializeDatabase(easyAbsolutePath, hardAbsolutePath, "")) {
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

bool Controller::InitializeDatabase(const QString &grammartesteasy_db_path,
                                    const QString &grammartesthard_db_path,
                                    const QString &)
{
    // Проверяем существование файлов баз данных
    QFileInfo easyDbFile(grammartesteasy_db_path);
    if (!easyDbFile.exists()) {
        qCritical() << "Файл базы данных (Easy) не найден:" << grammartesteasy_db_path;
        return false;
    }

    QFileInfo hardDbFile(grammartesthard_db_path);
    if (!hardDbFile.exists()) {
        qCritical() << "Файл базы данных (Hard) не найден:" << grammartesthard_db_path;
        return false;
    }

    // Настройка подключения к базе данных Easy
    grammar_test_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_easy");
    grammar_test_easy_db_.setDatabaseName(grammartesteasy_db_path);

    if (!grammar_test_easy_db_.open()) {
        qCritical() << "Ошибка открытия базы данных (Easy):" << grammar_test_easy_db_.lastError().text();
        return false;
    }

    // Проверяем наличие таблицы grammartesteasy_questions
    QSqlQuery easyCheckQuery(grammar_test_easy_db_);
    if (!easyCheckQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='grammartesteasy_questions'") ||
        !easyCheckQuery.next()) {
        qCritical() << "Таблица grammartesteasy_questions не найдена!";
        grammar_test_easy_db_.close();
        return false;
    }

    // Настройка подключения к базе данных Hard
    grammar_test_hard_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_hard");
    grammar_test_hard_db_.setDatabaseName(grammartesthard_db_path);

    if (!grammar_test_hard_db_.open()) {
        qCritical() << "Ошибка открытия базы данных (Hard):" << grammar_test_hard_db_.lastError().text();
        grammar_test_easy_db_.close();
        return false;
    }

    // Проверяем наличие таблицы grammartesthard_questions
    QSqlQuery hardCheckQuery(grammar_test_hard_db_);
    if (!hardCheckQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='grammartesthard_questions'") ||
        !hardCheckQuery.next()) {
        qCritical() << "Таблица grammartesthard_questions не найдена!";
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        return false;
    }

    qDebug() << "Базы данных успешно инициализированы";
    return true;
}

Controller::GrammarQuestion Controller::GetNextGrammarQuestion(int questionIndex,
                                                             QuestionType type,
                                                             DifficultyLevel difficulty)
{
    GrammarQuestion question;
    question.type = type;

    QSqlDatabase* db = (difficulty == DifficultyLevel::Easy) ? &grammar_test_easy_db_ : &grammar_test_hard_db_;
    QString tableName = (difficulty == DifficultyLevel::Easy) ? "grammartesteasy_questions" : "grammartesthard_questions";

    if (!db->isOpen()) {
        question.question = "База данных не открыта";
        return question;
    }

    QSqlQuery query(*db);
    query.prepare(QString("SELECT question, correct_answers, option1, option2, option3, option4, hint "
                         "FROM %1 WHERE id = ?").arg(tableName));
    query.addBindValue(questionIndex);

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
    TestStats& stats)
{
    std::vector<GrammarQuestion> questions;
    stats.Clear();
    stats.type = type;
    stats.difficulty = difficulty;
    stats.testId = 0;
    stats.answers.resize(kTestSize, AnswerType::NoAnswer);

    // Определяем статистику и размер в зависимости от сложности
    std::vector<TestStats>& statsVector = (difficulty == DifficultyLevel::Easy) ?
        grammar_test_easy_stats_ : grammar_test_hard_stats_;

    // Determine which set of questions to use based on completed tests
    int setIndex = 0;
    bool allSetsCompleted = true;

    // Check stats for completed tests
    for (size_t i = 0; i < statsVector.size(); ++i) {
        if (statsVector[i].questionsAnswered < kTestSize) {
            allSetsCompleted = false;
            setIndex = i;
            break;
        }
    }

    // If all sets are completed, return -1 as testId
    if (allSetsCompleted) {
        stats.testId = -1;
        GrammarQuestion errorQuestion;
        errorQuestion.question = "Все наборы вопросов завершены";
        questions.push_back(errorQuestion);
        return questions;
    }

    // Calculate starting index for the question set
    int start_index = setIndex * kTestSize + 1;

    // Populate questions vector
    for (int i = 0; i < kTestSize; ++i) {
        GrammarQuestion question = GetNextGrammarQuestion(start_index + i, type, difficulty);
        questions.push_back(question);
    }

    // Update testId in stats
    stats.testId = setIndex + 1;

    // Ensure stats vector is large enough
    if (static_cast<size_t>(stats.testId) > statsVector.size()) {
        statsVector.resize(stats.testId);
    }

    qDebug() << "Generated question set for testId:" << stats.testId
             << "with startIndex:" << start_index
             << "difficulty:" << (difficulty == DifficultyLevel::Easy ? "Easy" : "Hard");

    return questions;
}

void Controller::SendDataAboutTest(QuestionType type, DifficultyLevel difficulty, const TestStats& stats) {
    std::vector<TestStats>& statsVector = (difficulty == DifficultyLevel::Easy) ?
        grammar_test_easy_stats_ : grammar_test_hard_stats_;

    if (stats.testId >= 1 && stats.testId <= static_cast<int>(statsVector.size())) {
        statsVector[stats.testId - 1] = stats;
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