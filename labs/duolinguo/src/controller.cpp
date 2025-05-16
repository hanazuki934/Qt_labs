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

    /*QDir dir;
    QString absoluteGrammartesteasyPath = dir.absoluteFilePath(grammartesteasyPath);
    QString absoluteGrammarTestHardPath = dir.absoluteFilePath(grammarTestHardPath);

    // Выводим для отладки
    qDebug() << "Текущая рабочая директория:" << dir.currentPath();
    qDebug() << "Абсолютный путь grammartesteasy.db:" << absoluteGrammartesteasyPath;
    qDebug() << "Абсолютный путь grammarTestHard.db:" << absoluteGrammarTestHardPath;*/

    grammar_test_easy_stats_.resize(2);
    grammar_test_hard_stats_.resize(2);
    grammar_gap_fill_easy_stats_.resize(2);
    grammar_gap_fill_hard_stats_.resize(2);
    translation_test_en_to_ru_easy_stats_.resize(2);
    translation_test_en_to_ru_hard_stats_.resize(2);
    translation_test_ru_to_en_easy_stats_.resize(2);
    translation_test_ru_to_en_hard_stats_.resize(2);

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

    // Create easy grammar questions table with hint column
    QSqlQuery grammartesteasy_query(grammar_test_easy_db_);
    grammartesteasy_query.exec("CREATE TABLE IF NOT EXISTS grammartesteasy_questions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "type TEXT NOT NULL, "
        "question TEXT NOT NULL, "
        "correct_answers TEXT NOT NULL, "
        "option1 TEXT, "
        "option2 TEXT, "
        "option3 TEXT, "
        "option4 TEXT, "
        "hint TEXT)");
    if (grammartesteasy_query.lastError().isValid()) {
        qDebug() << "Ошибка при создании таблицы grammartesteasy_questions:" << grammartesteasy_query.lastError().text();
        grammar_test_easy_db_.close();
        grammar_test_hard_db_.close();
        return false;
    }

    // Create hard grammar questions table with hint column
    QSqlQuery grammarTestHard_query(grammar_test_hard_db_);
    grammarTestHard_query.exec("CREATE TABLE IF NOT EXISTS grammarTestHard_questions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "type TEXT NOT NULL, "
        "question TEXT NOT NULL, "
        "correct_answers TEXT NOT NULL, "
        "option1 TEXT, "
        "option2 TEXT, "
        "option3 TEXT, "
        "option4 TEXT, "
        "hint TEXT)");
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
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'She ___ very happy today.', 'is', 'am', 'is', 'are', 'be', 'Подсказка: \"She\" требует форму \"is\" (to be в Present Simple).')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'I ___ to the store yesterday.', 'went', 'go', 'gone', 'went', 'going', 'Подсказка: \"yesterday\" — это прошедшее время, нужен Past Simple.')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'They ___ playing football now.', 'are', 'is', 'are', 'was', 'be', 'Подсказка: \"They\" требует \"are\" в Present Continuous (\"now\").')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'He ___ a doctor.', 'is', 'am', 'are', 'is', 'be', 'Подсказка: \"He\" требует \"is\" в Present Simple. To be.')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'We ___ dinner at 7 p.m. every day.', 'have', 'have', 'has', 'had', 'having', 'Подсказка: \"We\" в Present Simple — используем \"have\".')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'My sister ___ مانند кофе.', 'doesn’t', 'don’t', 'doesn’t', 'isn’t', 'didn’t', 'Подсказка: 3-е лицо ед. число в отрицании — \"doesn''t\".')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', '___ you speak English?', 'Do', 'Do', 'Does', 'Is', 'Are', 'Подсказка: \"you\" требует \"Do\" в вопросе Present Simple.')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'There ___ a cat on the sofa.', 'is', 'is', 'are', 'be', 'am', 'Подсказка: \"a cat\" — ед. число, нужно \"is\".')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'We ___ at home last weekend.', 'were', 'was', 'is', 'were', 'are', 'Подсказка: \"We\" в Past Simple требует \"were\".')");
        grammartesteasy_query.exec(
            "INSERT INTO grammartesteasy_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'I have ___ apple in my bag.', 'an', 'an', 'a', 'the', 'some', 'Подсказка: Перед гласным звуком (\"apple\") используем \"an\".')");
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
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'If he ___ earlier, he would have caught the train.', 'had arrived', 'arrived', 'had arrived', 'was arriving', 'would arrive', 'Подсказка: Третий тип условного предложения — Past Perfect.')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'She denied ___ the vase.', 'having broken', 'to break', 'having broken', 'break', 'have broken', 'Подсказка: После \"deny\" используется герундий или perfect gerund.')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'The report must ___ before the meeting.', 'be completed', 'be completed', 'complete', 'have completed', 'completed', 'Подсказка: После \"must\" в пассиве — \"be + V3\".')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'Neither of the answers ___ correct.', 'is', 'are', 'is', 'were', 'be', 'Подсказка: \"Neither\" — единственное число.')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'He said that he ___ the book by the end of the week.', 'would finish', 'will finish', 'finishes', 'would finish', 'finish', 'Подсказка: Косвенная речь, будущее из прошлого — \"would\".')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'I regret ___ you that your application was unsuccessful.', 'to tell', 'to tell', 'telling', 'told', 'tell', 'Подсказка: \"regret to do\" = с сожалением сообщать.')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'By the time we got to the cinema, the film ___ already ___.', 'had started', 'had started', 'has started', 'have start', 'will start', 'Подсказка: Действие, завершённое до другого в прошлом — Past Perfect.')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'The students, one of whom ___ from France, are very talented.', 'is', 'is', 'are', 'were', 'be', 'Подсказка: \"one of whom\" — ед. число, глагол \"is\".')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', 'I’d rather you ___ home now.', 'went', 'go', 'went', 'had gone', 'going', 'Подсказка: После \"I''d rather\" — форма прошедшего времени.')");
        grammarTestHard_query.exec(
            "INSERT INTO grammarTestHard_questions (type, question, correct_answers, option1, option2, option3, option4, hint) "
            "VALUES ('multiple_choice', '___ people know how to solve this kind of problem.', 'Few', 'A few', 'Little', 'Few', 'A little', 'Подсказка: \"Few\" — исчисляемые существительные, значение \"мало\".')");
        if (grammarTestHard_query.lastError().isValid()) {
            qDebug() << "Ошибка при добавлении тестовых данных (сложные):" << grammarTestHard_query.lastError().text();
            grammar_test_easy_db_.close();
            grammar_test_hard_db_.close();
            return false;
        }
    }

    return true;
}

Controller::GrammarQuestion Controller::GetNextGrammarQuestion(int questionIndex, QuestionType type, DifficultyLevel difficulty) {
    GrammarQuestion question;

    // Определяем базу данных и таблицу на основе сложности
    QSqlDatabase* target_db = (difficulty == DifficultyLevel::Easy) ? &grammar_test_easy_db_ : &grammar_test_hard_db_;
    QString table_name = (difficulty == DifficultyLevel::Easy) ? "grammartesteasy_questions" : "grammarTestHard_questions";

    // Проверяем, открыта ли база данных
    if (!target_db->isOpen()) {
        qDebug() << "База данных грамматических тестов (" << (difficulty == DifficultyLevel::Easy ? "простые" : "сложные")
                 << ") не открыта. Возможно, InitializeDatabase не был вызван или завершился с ошибкой.";
        question.question = "Ошибка: база данных не открыта.";
        question.hint = "Подсказка недоступна.";
        return question;
    }

    // Проверяем корректность индекса
    if (questionIndex < 1 || questionIndex > 10) {
        qDebug() << "Недопустимый индекс вопроса:" << questionIndex << ". Должен быть от 1 до 10.";
        question.question = "Ошибка: недопустимый индекс вопроса.";
        question.hint = "Подсказка недоступна.";
        return question;
    }

    // Определяем строковое представление типа вопроса
    QString type_str;
    switch (type) {
        case QuestionType::MultipleChoice:
            type_str = "multiple_choice";
            break;
        case QuestionType::GapFill:
            type_str = "gap_fill";
            break;
        case QuestionType::TranslationRuToEn:
        case QuestionType::TranslationEnToRu:
            qDebug() << "Переводы обрабатываются через GetNextTranslationQuestion.";
            question.question = "Переводы обрабатываются через GetNextTranslationQuestion.";
            question.hint = "Подсказка недоступна.";
            return question;
        default:
            qDebug() << "Неизвестный тип вопроса.";
            question.question = "Неизвестный тип вопроса.";
            question.hint = "Подсказка недоступна.";
            return question;
    }

    // Выполняем запрос к базе данных с позиционными параметрами
    QSqlQuery query(*target_db);
    query.prepare(QString("SELECT type, question, correct_answers, option1, option2, option3, option4, hint "
                          "FROM %1 WHERE id = ? AND type = ?").arg(table_name));
    query.addBindValue(questionIndex);
    query.addBindValue(type_str);
    qDebug() << "Подготовленный запрос:" << query.lastQuery();
    qDebug() << "Привязанные значения: id =" << questionIndex << ", type =" << type_str;

    if (query.exec() && query.next()) {
        qDebug() << "Запрос выполнен успешно. Результаты:";
        qDebug() << "type:" << query.value("type").toString();
        qDebug() << "question:" << query.value("question").toString();
        qDebug() << "correct_answers:" << query.value("correct_answers").toString();
        qDebug() << "option1:" << query.value("option1").toString();
        qDebug() << "option2:" << query.value("option2").toString();
        qDebug() << "option3:" << query.value("option3").toString();
        qDebug() << "option4:" << query.value("option4").toString();
        qDebug() << "hint:" << query.value("hint").toString();

        // Заполняем структуру вопроса
        question.type = type;
        question.question = query.value("question").toString();
        question.correct_answers = query.value("correct_answers").toString().split("|", Qt::SkipEmptyParts);
        question.hint = query.value("hint").toString();

        // Собираем варианты ответа
        question.options.clear();
        question.options << (query.value("option1").toString().isEmpty() ? "" : query.value("option1").toString())
                         << (query.value("option2").toString().isEmpty() ? "" : query.value("option2").toString())
                         << (query.value("option3").toString().isEmpty() ? "" : query.value("option3").toString())
                         << (query.value("option4").toString().isEmpty() ? "" : query.value("option4").toString());

        // Проверяем и перемешиваем варианты для MultipleChoice
        if (type == QuestionType::MultipleChoice && question.options.size() >= 4) {
            ShuffleOptions(question);
        } else if (type == QuestionType::MultipleChoice && question.options.size() < 4) {
            qDebug() << "Недостаточно вариантов ответа для вопроса с множественным выбором.";
            question.question = "Ошибка: недостаточно вариантов ответа.";
            question.hint = "Подсказка недоступна.";
        }
    } else {
        qDebug() << "Ошибка выполнения запроса или вопрос не найден. Ошибка:" << query.lastError().text();
        qDebug() << "Последний запрос:" << query.lastQuery();
        question.question = QString("Вопрос с индексом %1 и типом %2 не найден.").arg(questionIndex).arg(type_str);
        question.hint = "Подсказка недоступна.";
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
    rightAnswers = 0;
    answers.clear();
}