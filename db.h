#ifndef DB_H
#define DB_H

#include "user.h"
#include "enums.h"

int list_users(const char* db_filename);

//
int change_attempts(const char* db_filename, const char* login, int new_limit);

// Регистрирует пользователя: login + pin
int register_user(const char* db_filename, const char* login, const char* pin);

// Пытается войти: login + pin → обновляет структуру user
int login_user(const char* db_filename, const char* login, const char* pin, struct User* user);

// Проверка: есть ли пользователь
int find_user(const char* db_filename, const char* login);

// Хеширование пароля (PIN)
int encrypt_password(const char* pin, char** hashed);

// Сравнение паролей
int compare_passwords(const char* pin, const char* hashed, int* result);

#endif
