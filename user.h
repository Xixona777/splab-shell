#ifndef USER_H
#define USER_H

#define MAX_LOGIN_LEN 6

struct User {
    char *login;        // логин пользователя
    int attempts;       // сколько команд осталось
    int is_admin;       // 1 — админ, 0 — обычный
    int is_logged_in;   // 1 — в системе, 0 — нет
};

#endif
