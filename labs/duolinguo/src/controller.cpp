#include "controller.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QSqlError>
#include <qcontainerfwd.h>
#include <qsqlquery.h>

Controller::Controller(const QString &grammartesteasy_db_path, const QString &grammarTestHardDbPath, const QString &translationDbPath) {
    const QString grammartesteasyPath = "data/" + grammartesteasy_db_path;
    const QString grammarTestHardPath = "data/" + grammarTestHardDbPath;
    const QString translationPath = "data/" + translationDbPath;

    if (!InitializeDatabase(grammartesteasyPath, grammarTestHardPath, translationPath)) {
        qCritical() << "Failed to initialize databases!";
    }
}

Controller::DifficultyLevel Controller::GetDifficulty() const {
    return difficulty_;
}

void Controller::SetDifficulty(DifficultyLevel difficulty) {
    difficulty_ = difficulty;
}

bool Controller::InitializeDatabase(const QString &grammartesteasy_db_path, const QString &grammarTestHardDbPath, const QString &translationDbPath) {
    // Initialize easy grammar database
    QFile grammartesteasyFile(grammartesteasy_db_path);
    if (!grammartesteasyFile.exists()) {
        if (!grammartesteasyFile.open(QIODevice::WriteOnly)) {
            qCritical() << "Failed to create grammartesteasy db file";
            return false;
        }
        grammartesteasyFile.close();
    }

    // Initialize hard grammar database
    QFile grammarTestHardFile(grammarTestHardDbPath);
    if (!grammarTestHardFile.exists()) {
        if (!grammarTestHardFile.open(QIODevice::WriteOnly)) {
            qCritical() << "Failed to create grammarTestHard db file";
            return false;
        }
        grammarTestHardFile.close();
    }

    qDebug() << "Вызван InitializeDatabase с параметрами:";
    qDebug() << "grammartesteasyDbPath:" << grammartesteasy_db_path;
    qDebug() << "grammarTestHardDbPath:" << grammarTestHardDbPath;
    qDebug() << "translationDbPath:" << translationDbPath;

    // Check easy grammar file existence
    QFileInfo grammartesteasyFileInfo(grammartesteasy_db_path);
    if (!grammartesteasyFileInfo.exists()) {
        qDebug() << "Файл базы данных грамматических тестов (простые) не существует по пути:" << grammartesteasy_db_path;
        qDebug() << "Абсолютный путь:" << grammartesteasyFileInfo.absoluteFilePath();
        return false;
    }

    // Check hard grammar file existence
    QFileInfo grammarTestHardFileInfo(grammarTestHardDbPath);
    if (!grammarTestHardFileInfo.exists()) {
        qDebug() << "Файл базы данных грамматических тестов (сложные) не существует по пути:" << grammarTestHardDbPath;
        qDebug() << "Абсолютный путь:" << grammarTestHardFileInfo.absoluteFilePath();
        return false;
    }

    // Open easy grammar database
    grammar_test_easy_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammartesteasy_db");
    grammar_test_easy_db_.setDatabaseName(grammartesteasy_db_path);
    qDebug() << "Попытка открыть базу данных грамматических тестов (простые)...";
    if (!grammar_test_easy_db_.open()) {
        qDebug() << "Ошибка открытия базы данных (грамматические тесты, простые):" << grammar_test_easy_db_.lastError().text();
        return false;
    }
    qDebug() << "База данных грамматических тестов (простые) успешно открыта.";

    // Open hard grammar database
    grammar_test_hard_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammarTestHard_db");
    grammar_test_hard_db_.setDatabaseName(grammarTestHardDbPath);
    qDebug() << "Попытка открыть базу данных грамматических тестов (сложные)...";
    if (!grammar_test_hard_db_.open()) {
        qDebug() << "Ошибка открытия базы данных (грамматические тесты, сложные):" << grammar_test_hard_db_.lastError().text();
        grammar_test_easy_db_.close();
        return false;
    }
    qDebug() << "База данных грамматических тестов (сложные) успешно открыта.";

    // Create easy grammar questions table
    QSqlQuery grammartesteasy_query(grammar_test_easy_db_);
    grammartesteasy_query.exec("CREATE TABLE IF NOT EXISTS grammartesteasy_questions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "type TEXT NOT NULL, "
        "question TEXT NOT NULL, "
        "correct_answers TEXT NOT NULL, "
        "option1 TEXT, "
        "option2 TEXT, "
        "option3 TEXT, "
        "option4 TEXT)");
    if (grammartesteasy_query.lastError().isValid()) {
        qDebug() << "Ошибка при создании таблицы grammartesteasy_questions:" << grammartesteasy_query.lastError().text();
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        return false;
    }

    // Create hard grammar questions table
    QSqlQuery grammarTestHard_query(grammar_test_hard_db_);
    grammarTestHard_query.exec("CREATE TABLE IF NOT EXISTS grammarTestHard_questions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "type TEXT NOT NULL, "
        "question TEXT NOT NULL, "
        "correct_answers TEXT NOT NULL, "
        "option1 TEXT, "
        "option2 TEXT, "
        "option3 TEXT, "
        "option4 TEXT)");
    if (grammarTestHard_query.lastError().isValid()) {
        qDebug() << "Ошибка при создании таблицы grammarTestHard_questions:" << grammarTestHard_query.lastError().text();
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        return false;
    }

    // Add test data to easy grammar questions if table is empty
    grammartesteasy_query.exec("SELECT COUNT(*) FROM grammartesteasy_questions");
    if (grammartesteasy_query.next() && grammartesteasy_query.value(0).toInt() == 0) {
        qDebug() << "Таблица grammartesteasy_questions пуста, добавляем тестовые данные.";
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'She ___ very happy today.', 'is', 'am', 'is', 'are', 'be')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'I ___ to the store yesterday.', 'went', 'go', 'gone', 'went', 'going')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'They ___ playing football now.', 'are', 'is', 'are', 'was', 'be')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'He ___ a doctor.', 'is', 'am', 'are', 'is', 'be')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'We ___ dinner at 7 p.m. every day.', 'have', 'have', 'has', 'had', 'having')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'My sister ___ like coffee.', 'doesn’t', 'don’t', 'doesn’t', 'isn’t', 'didn’t')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', '___ you speak English?', 'Do', 'Do', 'Does', 'Is', 'Are')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'There ___ a cat on the sofa.', 'is', 'is', 'are', 'be', 'am')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'We ___ at home last weekend.', 'were', 'was', 'is', 'were', 'are')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'I have ___ apple in my bag.', 'an', 'an', 'a', 'the', 'some')");
        if (grammartesteasy_query.lastError().isValid()) {
            qDebug() << "Ошибка при добавлении тестовых данных (простые):" << grammartesteasy_query.lastError().text();
            grammar_test_easy_db_.close();
            grammar_test_hard_db_.close();
            return false;
        }
    }

    // Add test data to hard grammar questions if table is empty
    grammarTestHard_query.exec("SELECT COUNT(*) FROM grammarTestHard_questions");
    if (grammarTestHard_query.next() && grammarTestHard_query.value(0).toInt() == 0) {
        qDebug() << "Таблица grammarTestHard_questions пуста, добавляем тестовые данные.";
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'If he ___ earlier, he would have caught the train.', 'had arrived', 'arrived', 'had arrived', 'was arriving', 'would arrive')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'She denied ___ the vase.', 'having broken', 'to break', 'having broken', 'break', 'have broken')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'The report must ___ before the meeting.', 'be completed', 'be completed', 'complete', 'have completed', 'completed')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'Neither of the answers ___ correct.', 'is', 'are', 'is', 'were', 'be')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'He said that he ___ the book by the end of the week.', 'would finish', 'will finish', 'finishes', 'would finish', 'finish')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'I regret ___ you that your application was unsuccessful.', 'to tell', 'to tell', 'telling', 'told', 'tell')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'By the time we got to the cinema, the film ___ already ___.', 'had started', 'had started', 'has started', 'have start', 'will start')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'The students, one of whom ___ from France, are very talented.', 'is', 'is', 'are', 'were', 'be')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'I’d rather you ___ home now.', 'went', 'go', 'went', 'had gone', 'going')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', '___ people know how to solve this kind of problem.', 'Few', 'A few', 'Little', 'Few', 'A little')");
        if (grammarTestHard_query.lastError().isValid()) {
            qDebug() << "Ошибка при добавлении тестовых данных (сложные):" << grammarTestHard_query.lastError().text();
            grammar_test_easy_db_.close();
            grammar_test_hard_db_.close();
            return false;
        }
    }

    return true;
}

Controller::GrammarQuestion Controller::GetNextGrammarQuestion(QuestionType type) {
    GrammarQuestion question;
    QSqlDatabase* target_db = (difficulty_ == DifficultyLevel::Easy) ? &grammar_test_easy_db_ : &grammar_test_hard_db_;
    QString table_name = (difficulty_ == DifficultyLevel::Easy) ? "grammartesteasy_questions" : "grammarTestHard_questions";

    if (!target_db->isOpen()) {
        qDebug() << "База данных грамматических тестов (" << (difficulty_ == DifficultyLevel::Easy ? "простые" : "сложные")
                 << ") не открыта. Возможно, InitializeDatabase не был вызван или завершился с ошибкой.";
        question.question = "Ошибка: база данных не открыта.";
        return question;
    }

    QSqlQuery query(*target_db);
    int currentId = 0;
    const int maxId = 10; // Максимальный id вопросов (всего 10 вопросов в каждой базе)

    // Определяем текущий id в зависимости от типа и сложности
    switch (type) {
        case QuestionType::MultipleChoice:
            if (difficulty_ == DifficultyLevel::Easy) {
                currentId = currentGrammarTestEasyId_;
            } else {
                currentId = currentGrammarTestHardId_;
            }
            break;
        case QuestionType::GapFill:
            if (difficulty_ == DifficultyLevel::Easy) {
                currentId = currentGrammarGapFillEasyId_;
            } else {
                currentId = currentGrammarGapFillHardId_;
            }
            break;
        case QuestionType::TranslationRuToEn:
        case QuestionType::TranslationEnToRu:
            qDebug() << "Переводы обрабатываются через GetNextTranslationQuestion.";
            question.question = "Переводы обрабатываются через GetNextTranslationQuestion.";
            return question;
        default:
            qDebug() << "Неизвестный тип вопроса.";
            question.question = "Неизвестный тип вопроса.";
            return question;
    }

    // Выбираем вопрос по порядку на основе текущего id
    query.prepare(QString("SELECT type, question, correct_answers, option1, option2, option3, option4 "
                          "FROM %1 WHERE id = :id").arg(table_name));
    query.bindValue(":id", currentId);
    qDebug() << "Выполняем запрос для id:" << currentId << "в таблице:" << table_name;

    if (query.exec() && query.next()) {
        qDebug() << "Запрос выполнен успешно. Результаты:";
        qDebug() << "type:" << query.value("type").toString();
        qDebug() << "question:" << query.value("question").toString();
        qDebug() << "correct_answers:" << query.value("correct_answers").toString();
        qDebug() << "option1:" << query.value("option1").toString();
        qDebug() << "option2:" << query.value("option2").toString();
        qDebug() << "option3:" << query.value("option3").toString();
        qDebug() << "option4:" << query.value("option4").toString();

        question.type = type;
        question.question = query.value("question").toString();
        question.correct_answers = query.value("correct_answers").toString().split("|", Qt::SkipEmptyParts);

        question.options.clear();
        question.options << (query.value("option1").toString().isEmpty() ? "" : query.value("option1").toString())
                         << (query.value("option2").toString().isEmpty() ? "" : query.value("option2").toString())
                         << (query.value("option3").toString().isEmpty() ? "" : query.value("option3").toString())
                         << (query.value("option4").toString().isEmpty() ? "" : query.value("option4").toString());

        if (type == QuestionType::MultipleChoice && question.options.size() >= 4) {
            ShuffleOptions(question);
        } else if (type == QuestionType::MultipleChoice && question.options.size() < 4) {
            qDebug() << "Недостаточно вариантов ответа для вопроса с множественным выбором.";
        }

        // Увеличиваем текущий id и сбрасываем на 1, если достигли максимума
        if (difficulty_ == DifficultyLevel::Easy) {
            if (type == QuestionType::MultipleChoice) {
                currentGrammarTestEasyId_++;
                if (currentGrammarTestEasyId_ > maxId) {
                    currentGrammarTestEasyId_ = 1;
                }
            } else if (type == QuestionType::GapFill) {
                currentGrammarGapFillEasyId_++;
                if (currentGrammarGapFillEasyId_ > maxId) {
                    currentGrammarGapFillEasyId_ = 1;
                }
            }
        } else {
            if (type == QuestionType::MultipleChoice) {
                currentGrammarTestHardId_++;
                if (currentGrammarTestHardId_ > maxId) {
                    currentGrammarTestHardId_ = 1;
                }
            } else if (type == QuestionType::GapFill) {
                currentGrammarGapFillHardId_++;
                if (currentGrammarGapFillHardId_ > maxId) {
                    currentGrammarGapFillHardId_ = 1;
                }
            }
        }
    } else {
        qDebug() << "Ошибка выполнения запроса или вопрос не найден. Ошибка:" << query.lastError().text();
        // Сбрасываем id на начало
        if (difficulty_ == DifficultyLevel::Easy) {
            if (type == QuestionType::MultipleChoice) {
                currentGrammarTestEasyId_ = 1;
            } else if (type == QuestionType::GapFill) {
                currentGrammarGapFillEasyId_ = 1;
            }
        } else {
            if (type == QuestionType::MultipleChoice) {
                currentGrammarTestHardId_ = 1;
            } else if (type == QuestionType::GapFill) {
                currentGrammarGapFillHardId_ = 1;
            }
        }
        question.question = "Вопросы отсутствуют.";
    }

    return question;
}

Controller::TranslationQuestion Controller::GetNextTranslationQuestion() {
    TranslationQuestion question;
    if (!translation_db_.isOpen()) {
        qDebug() << "База данных перевода не открыта. Возможно, InitializeDatabase не был вызван или завершился с ошибкой.";
        question.source_text = "Ошибка: база данных не открыта.";
        return question;
    }

    question.source_text = "Функционал перевода пока не реализован.";
    return question;
}

void Controller::ShuffleOptions(GrammarQuestion &question) {
    QStringList all_options = question.options;

    for (int i = all_options.size() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(0, i + 1);
        all_options.swapItemsAt(i, j);
    }

    question.options = all_options;
}

void Controller::TestStats::Clear() {
    questionsAnswered = 0;
    timeElapsed = 0;
    answers.clear();
}