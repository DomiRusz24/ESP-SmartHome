#include "UserGroup.hpp"

int UserGroup::getId() { return id; }
String UserGroup::getName() { return name; }

void UserGroup::addUser(User* user) { users.add(user); }

void UserGroup::removeUser(User* user) { users.removeElement(user); }

bool UserGroup::containsUser(int id) {
    for (User* user : users) {
        if (user->getId() == id) {
            return true;
        }
    }

    return false;
}

void UserGroup::recieveMessage(int from, String content) {
    for (User* user : users) {
        user->recieveMessage(from, content);
    }
}

void UserGroup::kick(int who) { users.clear(); }

MsgType UserGroup::getType() { return MsgType::GROUP; }

boolean UserGroup::isEmpty() { return users.isEmpty(); }