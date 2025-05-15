#include "controller.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QSqlError>
#include <qcontainerfwd.h>
#include <qsqlquery.h>

Controller::Controller(const QString &grammar_db_path, const QString &translationDbPath) {
    const QString grammarPath = "data/" + grammar_db_path;
    const QString translationPath = "data/" + translationDbPath;

    if (!InitializeDatabase(grammarPath, translationPath)) {
        qCritical() << "Failed to initialize database!";
    }
}

Controller::DifficultyLevel Controller::GetDifficulty() const {
    return difficulty_;
}

void Controller::SetDifficulty(DifficultyLevel difficulty) {
    difficulty_ = difficulty;
}

bool Controller::InitializeDatabase(const QString &grammar_db_path, const QString &translationDbPath) {
    QFile grammarFile(grammar_db_path);
    if (!grammarFile.exists()) {
        if (!grammarFile.open(QIODevice::WriteOnly)) {
            qCritical() << "Failed to create grammar db file";
            return false;
        }
        grammarFile.close();
    }

    qDebug() << "Вызван InitializeDatabase с параметрами:";
    qDebug() << "grammarDbPath:" << grammar_db_path;
    qDebug() << "translationDbPath:" << translationDbPath;

    QFileInfo grammarFileInfo(grammar_db_path);
    if (!grammarFileInfo.exists()) {
        qDebug() << "Файл базы данных грамматики не существует по пути:" << grammar_db_path;
        qDebug() << "Абсолютный путь:" << grammarFileInfo.absoluteFilePath();
        return false;
    }

    grammar_db_ = QSqlDatabase::addDatabase("QSQLITE", "grammar_db");
    grammar_db_.setDatabaseName(grammar_db_path);
    qDebug() << "Попытка открыть базу данных грамматики...";
    if (!grammar_db_.open()) {
        qDebug() << "Ошибка открытия базы данных (грамматика):" << grammar_db_.lastError().text();
        return false;
    }
    qDebug() << "База данных грамматики успешно открыта.";

    QSqlQuery grammar_query(grammar_db_);
    grammar_query.exec("CREATE TABLE IF NOT EXISTS grammar_questions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "type TEXT NOT NULL, "
        "question TEXT NOT NULL, "
        "correct_answers TEXT NOT NULL, "
        "option1 TEXT, "
        "option2 TEXT, "
        "option3 TEXT, "
        "option4 TEXT)");
    if (grammar_query.lastError().isValid()) {
        qDebug() << "Ошибка при создании таблицы grammar_questions:" << grammar_query.lastError().text();
        grammar_db_.close();
        return false;
    }

    // Добавляем тестовые данные, если таблица пуста
    grammar_query.exec("SELECT COUNT(*) FROM grammar_questions");
    if (grammar_query.next() && grammar_query.value(0).toInt() == 0) {
        qDebug() << "Таблица grammar_questions пуста, добавляем тестовые данные.";
        grammar_query.exec(
            "INSERT INTO grammar_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'She ___ very happy today.', 'is', 'am', 'is', 'are', 'be')");
        grammar_query.exec(
            "INSERT INTO grammar_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'I ___ to the store yesterday.', 'went', 'go', 'gone', 'went', 'going')");
        grammar_query.exec(
            "INSERT INTO grammar_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'They ___ playing football now.', 'are', 'is', 'are', 'was', 'be')");
        grammar_query.exec(
            "INSERT INTO grammar_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'He ___ a doctor.', 'is', 'am', 'are', 'is', 'be')");
        grammar_query.exec(
            "INSERT INTO grammar_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'We ___ dinner at 7 p.m. every day.', 'have', 'have', 'has', 'had', 'having')");
        grammar_query.exec(
            "INSERT INTO grammar_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'My sister ___ like coffee.', 'doesn’t', 'don’t', 'doesn’t', 'isn’t', 'didn’t')");
        grammar_query.exec(
            "INSERT INTO grammar_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', '___ you speak English?', 'Do', 'Do', 'Does', 'Is', 'Are')");
        grammar_query.exec(
            "INSERT INTO grammar_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'There ___ a cat on the sofa.', 'is', 'is', 'are', 'be', 'am')");
        grammar_query.exec(
            "INSERT INTO grammar_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'We ___ at home last weekend.', 'were', 'was', 'is', 'were', 'are')");
        grammar_query.exec(
            "INSERT INTO grammar_questions (type, question, correct_answers, option1, option2, option3, option4) "
            "VALUES ('multiple_choice', 'I have ___ apple in my bag.', 'an', 'an', 'a', 'the', 'some')");
        if (grammar_query.lastError().isValid()) {
            qDebug() << "Ошибка при добавлении тестовых данных:" << grammar_query.lastError().text();
            grammar_db_.close();
            return false;
        }
    }

    return true;
}

Controller::GrammarQuestion Controller::GetNextGrammarQuestion(QuestionType type) {
    GrammarQuestion question;
    if (!grammar_db_.isOpen()) {
        qDebug() <<
                "База данных грамматики не открыта. Возможно, InitializeDatabase не был вызван или завершился с ошибкой.";
        question.question = "Ошибка: база данных не открыта.";
        return question;
    }

    QSqlQuery query(grammar_db_);
    int currentId = 0;
    const int maxId = 10; // Максимальный id вопросов (всего 10 вопросов в базе)

    // Определяем текущий id в зависимости от типа и сложности
    switch (type) {
        case QuestionType::MultipleChoice:
            if (difficulty_ == DifficultyLevel::Easy) {
                currentId = currentGrammarTestEasyId_;
            } else {
                qDebug() << "Сложные вопросы по грамматике (тест) пока не реализованы.";
                question.question = "Сложные вопросы по грамматике (тест) пока не реализованы.";
                return question;
            }
            break;
        case QuestionType::GapFill:
            if (difficulty_ == DifficultyLevel::Easy) {
                currentId = currentGrammarGapFillEasyId_;
            } else {
                qDebug() << "Сложные вопросы по грамматике (вставка слов) пока не реализованы.";
                question.question = "Сложные вопросы по грамматике (вставка слов) пока не реализованы.";
                return question;
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
    query.prepare("SELECT type, question, correct_answers, option1, option2, option3, option4 "
        "FROM grammar_questions WHERE id = :id");
    query.bindValue(":id", currentId);
    qDebug() << "Выполняем запрос для id:" << currentId;

    if (query.exec() && query.next()) {
        qDebug() << "Запрос выполнен успешно. Результаты:";
        qDebug() << "type:" << query.value("type").toString();
        qDebug() << "question:" << query.value("question").toString();
        qDebug() << "correct_answers:" << query.value("correct_answers").toString();
        qDebug() << "option1:" << query.value("option1").toString();
        qDebug() << "option2:" << query.value("option2").toString();
        qDebug() << "option3:" << query.value("option3").toString();
        qDebug() << "option4:" << query.value("option4").toString();

        question.type = type; // Устанавливаем переданный тип
        question.question = query.value("question").toString();
        question.correct_answers = query.value("correct_answers").toString().split("|", Qt::SkipEmptyParts);

        // Проверяем, что все опции доступны, иначе заполняем пустыми строками
        question.options.clear();
        question.options << (query.value("option1").toString().isEmpty() ? "" : query.value("option1").toString())
                << (query.value("option2").toString().isEmpty() ? "" : query.value("option2").toString())
                << (query.value("option3").toString().isEmpty() ? "" : query.value("option3").toString())
                << (query.value("option4").toString().isEmpty() ? "" : query.value("option4").toString());

        qDebug() << "Сформированный список options:" << question.options;
        qDebug() << "Размер options:" << question.options.size();

        // Перемешиваем варианты ответа только для MultipleChoice
        if (type == QuestionType::MultipleChoice && question.options.size() >= 4) {
            ShuffleOptions(question);
            qDebug() << "Options после перемешивания:" << question.options;
        } else if (type == QuestionType::MultipleChoice && question.options.size() < 4) {
            qDebug() << "Ошибка: Недостаточно вариантов ответа (" << question.options.size() << " < 4).";
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
        // Если вопрос не найден, сбрасываем id на начало
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
        qDebug() <<
                "База данных перевода не открыта. Возможно, InitializeDatabase не был вызван или завершился с ошибкой.";
        question.source_text = "Ошибка: база данных не открыта.";
        return question;
    }

    // Пока перевод не реализован, возвращаем заглушку
    question.source_text = "Функционал перевода пока не реализован.";
    return question;
}

void Controller::ShuffleOptions(GrammarQuestion &question) {
    QStringList all_options = question.options;

    for (int i = all_options.size() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(0, i + 1); // Границы: [0, i]
        all_options.swapItemsAt(i, j);
    }

    question.options = all_options;
}

void Controller::TestStats::Clear() {
    questionsAnswered = 0;
    answers.clear();
}
