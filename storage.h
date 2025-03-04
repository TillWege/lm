#ifndef STORAGE_H
#define STORAGE_H

#include "SQLiteCpp/Database.h"
#include <optional>

struct Message
{
    int id;
    int author;
    std::string msg;
};

struct Request
{
    std::optional<int> id;
    std::string msg;
    std::string answer;
    std::string model;
};

SQLite::Database initDatabase();

int addChat();
std::vector<int>& getChatList();
void addMessage(const Message& msg, int chatId);
void addRequest(SQLite::Database& db, const Request &req);

#endif //STORAGE_H
