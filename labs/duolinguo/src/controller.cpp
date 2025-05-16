#include "controller.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QSqlError>
#include <QSqlQuery>

Controller::Controller(const QString &grammartesteasy_db_path,
                      const QString &,
                      const QString &)
{
    const QString dbPath = "labs/duolinguo/src/db/grammartesteasy.db";

    // Получаем абсолютный путь
    QFileInfo dbFile(dbPath);
    QString absolutePath = dbFile.absoluteFilePath();

    qDebug() << "Путь к базе данных:" << absolutePath;

    grammar_test_easy_stats_.resize(2);

    if (!InitializeDatabase(absolutePath, "", "")) {
        qCritical() << "Не удалось инициализировать базу данных!";
    }
}

Controller::DifficultyLevel Controller::GetDifficulty() const {
    return difficulty_;
}

void Controller::SetDifficulty(DifficultyLevel difficulty) {
    difficulty_ = difficulty;
}

bool Controller::InitializeDatabase(const QString &grammartesteasy_db_path,
                                  const QString &,
                                  const QString &)
{
    // Проверяем существование файла
    QFileInfo dbFile(grammartesteasy_db_path);
    if (!dbFile.exists()) {
        qCritical() << "Файл базы данных не найден:" << grammartesteasy_db_path;
        return false;
    }

    // Настройка подключения к БД
    grammar_test_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_easy");
    grammar_test_easy_db_.setDatabaseName(grammartesteasy_db_path);

    if (!grammar_test_easy_db_.open()) {
        qCritical() << "Ошибка открытия базы данных:" << grammar_test_easy_db_.lastError().text();
        return false;
    }

    // Проверяем наличие таблицы
    QSqlQuery checkQuery(grammar_test_easy_db_);
    if (!checkQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='grammartesteasy_questions'") ||
        !checkQuery.next()) {
        qCritical() << "Таблица grammartesteasy_questions не найдена!";
        grammar_test_easy_db_.close();
        return false;
    }

    qDebug() << "База данных успешно инициализирована";
    return true;
}

Controller::GrammarQuestion Controller::GetNextGrammarQuestion(int questionIndex,
                                                             QuestionType type,
                                                             DifficultyLevel difficulty)
{
    GrammarQuestion question;

    if (difficulty != DifficultyLevel::Easy) {
        question.question = "Сложные вопросы не поддерживаются";
        return question;
    }

    if (!grammar_test_easy_db_.isOpen()) {
        question.question = "База данных не открыта";
        return question;
    }

    QSqlQuery query(grammar_test_easy_db_);
    query.prepare("SELECT question, correct_answers, option1, option2, option3, option4, hint "
                  "FROM grammartesteasy_questions WHERE id = ?");
    query.addBindValue(questionIndex);

    if (query.exec() && query.next()) {
        question.type = type;
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

void Controller::ShuffleOptions(GrammarQuestion &question) {
    for (int i = question.options.size() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        question.options.swapItemsAt(i, j);
    }
}

void Controller::SendDataAboutTest(QuestionType type, DifficultyLevel difficulty, const TestStats& stats) {
    if (difficulty == DifficultyLevel::Easy) {
        if (stats.testId >= 1 && stats.testId <= grammar_test_easy_stats_.size()) {
            grammar_test_easy_stats_[stats.testId - 1] = stats;
            qDebug() << "Сохранена статистика для теста" << stats.testId;
        }
    }
    if (stats.rightAnswers == kTestSize) {
        if (difficulty_ == DifficultyLevel::Easy) {
            ball_++;
        } else {
            ball_ += 2;
        }
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

    // For now, only Easy difficulty is supported based on provided code
    if (difficulty != DifficultyLevel::Easy) {
        GrammarQuestion errorQuestion;
        errorQuestion.question = "Сложные вопросы не поддерживаются";
        questions.push_back(errorQuestion);
        return questions;
    }

    // Determine which set of questions to use based on completed tests
    int setIndex = 0;
    bool allSetsCompleted = true;

    // Check grammar_test_easy_stats_ for completed tests
    for (size_t i = 0; i < grammar_test_easy_stats_.size(); ++i) {
        if (grammar_test_easy_stats_[i].questionsAnswered < kTestSize) {
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

    // Ensure grammar_test_easy_stats_ is large enough
    if (static_cast<size_t>(stats.testId) > grammar_test_easy_stats_.size()) {
        grammar_test_easy_stats_.resize(stats.testId);
    }

    qDebug() << "Generated question set for testId:" << stats.testId
             << "with startIndex:" << start_index;

    return questions;
}

int Controller::GetBall() const {
    return ball_;
}

void Controller::SetBall(const int ball) {
    ball_ = ball;
}
