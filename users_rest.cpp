#define CROW_MAIN
#include <string>
#include <utility>
#include <vector>
#include <set>
#include <map>
#include "crow.hpp"

using std::string;
using std::vector;
using std::map;
using std::set;
using std::pair;

class UserManager {
private:

    map<int, map<string, string>>  _users;
    set<int> _userIds{};

    int getUniqueId();

public:

    explicit UserManager(const map<int, map<string, string>> &/*users*/);

    crow::json::wvalue getAll() const;

    crow::json::wvalue get(int /*id*/);

    crow::json::wvalue add(const crow::json::rvalue &/*user*/);

    crow::json::wvalue update(int /*userId*/, const crow::json::rvalue &/*user*/);

    crow::json::wvalue remove(int /*id*/);

};

int UserManager::getUniqueId() {
    std::srand(std::time(nullptr));
    int newUserId = std::rand();
    while (_userIds.find(newUserId) != _userIds.end()) {
        newUserId = std::rand();
    }
    _userIds.insert(newUserId);
    return newUserId;
}

UserManager::UserManager(const map<int, map<string, string>> &users) {
    for (auto [id, user] : users) {
        _users[id] = user;
        _userIds.insert(id);
    }
};

crow::json::wvalue UserManager::getAll() const {
    vector<crow::json::wvalue> users;

    for (auto [userId, userMap]: _users) {
        crow::json::wvalue user;

        user["id"] = userId;
        user["firstname"] = userMap["firstname"];
        user["lastname"] = userMap["lastname"];

        users.emplace_back(user);
    }
    return crow::json::wvalue(users);
}

crow::json::wvalue UserManager::get(int id) {
    if (_users.find(id) == _users.end()) {
        crow::json::wvalue response;
        response["status"] = "user with ID " + std::to_string(id) + " does not exist";
        return response;
    }

    crow::json::wvalue user;
    user["id"] = id;
    user["firstname"] = _users[id]["firstname"];
    user["lastname"] = _users[id]["lastname"];
    return user;
}

crow::json::wvalue UserManager::add(const crow::json::rvalue &user) {
    if (!user.has("firstname") || !user.has("lastname")) {
        crow::json::wvalue response;
        response["status"] = "incorrect input data";
        return response;
    }
    int id = getUniqueId();
    _users[id] = map<string, string>{
        {"firstname", user["firstname"].s()},
        {"lastname", user["lastname"].s()}
    };
    crow::json::wvalue response;
    response["status"] = "ok";
    return response;
}

crow::json::wvalue UserManager::update(int userId, const crow::json::rvalue &user) {
    crow::json::wvalue response;
    if (_users.find(userId) == _users.end()) {
        response["status"] = "user with ID " + std::to_string(userId) + " does not exist";
        return response;
    }

    if (user.has("firstname")) {
        _users[userId]["firstname"] = user["firstname"].s();
    }
    if (user.has("lastname")) {
        _users[userId]["lastname"] = user["lastname"].s();
    }

    response["status"] = "ok";
    return response;
}

crow::json::wvalue UserManager::remove(int id) {
    crow::json::wvalue response;
    if (_users.find(id) == _users.end()) {
        response["status"] = "user with ID " + std::to_string(id) + " does not exist";
        return response;
    }
    _users.erase(id);
    response["status"] = "ok";
    return response;
}


int main(int argc, char **argv) {
    int port = argc > 1 ? std::stoi(argv[1]) : 8080;
    map<int, map<string, string>> users{
        {
            1, {
                {"firstname", "Федор"},
                {"lastname", "Достоевский"},
            }
        },
        {
            2, {
                {"firstname", "Александр"},
                {"lastname", "Пушкин"},
            }
        }
    };

    crow::SimpleApp app;
    UserManager userManager(users);

    CROW_ROUTE(app, "/users")
    .methods("GET"_method)
    ([&userManager](const crow::request& req) {
        return userManager.getAll();
    });

    CROW_ROUTE(app, "/users/<int>")
    .methods("GET"_method)
    ([&userManager](int userId) {
        return userManager.get(userId);
    });

    CROW_ROUTE(app, "/users/")
    .methods("POST"_method)
    ([&userManager](const crow::request& req) {
        auto newUser = crow::json::load(req.body);
        auto response = userManager.add(newUser);
        return response;
    });

    CROW_ROUTE(app, "/users/<int>")
    .methods("PUT"_method)
    ([&userManager](const crow::request& req, int userId) {
        auto updatedUser = crow::json::load(req.body);
        auto response = userManager.update(userId, updatedUser);
        return response;
    });

    CROW_ROUTE(app, "/users/<int>")
    .methods("DELETE"_method)
    ([&userManager](int userId) {
        auto response = userManager.remove(userId);
        return response;
    });


    app.port(port)
        .multithreaded()
        .run();
}
