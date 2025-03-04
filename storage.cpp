#include "storage.h"

#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include <format>
#include <iostream>
#include <ostream>

enum Role {
    USER,
    AI,
    SYSTEM
};

static int toInt(Role e)
{
    switch (e) {
        case Role::USER:
            return 1;
        case Role::AI:
            return 2;
        case Role::SYSTEM:
            return 3;
        default:
            throw std::invalid_argument("Invalid enum value");
    }
}

static Role fromInt(int i)
{
    switch (i) {
        case 1:
            return Role::USER;
        case 2:
            return Role::AI;
        case 3:
            return Role::SYSTEM;
        default:
            throw std::invalid_argument("Invalid enum value");
    }
}

SQLite::Database initDatabase()
{
    auto db = SQLite::Database("lm.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );

    db.exec(R"(
    CREATE TABLE IF NOT EXISTS messages (
        id INTEGER PRIMARY KEY,
        author INTEGER,
        message TEXT
    );)");

    db.exec(R"(
    CREATE TABLE IF NOT EXISTS QuestionsAnswers (
        id INTEGER PRIMARY KEY,
        Question TEXT,
        Answer TEXT,
        Model TEXT
    );)");

    std::cout << "Db init Success" << std::endl;
    return db;
}




void addRequest(SQLite::Database& db, const Request &req)
{

    int res = db.exec(std::format(R"(
        INSERT INTO QuestionsAnswers VALUES
        (
            NULL,
            '{}',
            '{}',
            '{}'
        );)",
    req.msg, req.answer, req.model));
    std::cout << "INSER RET: " << res << std::endl;
}
