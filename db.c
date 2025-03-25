#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <ctype.h>

#include "db.h"

int list_users(const char* db_filename) {
    FILE* f = fopen(db_filename, "r");
    if (!f) return FILE_ERROR;

    char line[512];
    printf("Users in system:\n");

    while (fgets(line, sizeof(line), f)) {
        char* saved = strdup(line);  // сохраним оригинал

        char* login = strtok(line, " \t\n");
        char* hash = strtok(NULL, " \t\n");
        char* attempts = strtok(NULL, " \t\n");

        if (!login || !hash || !attempts || strcmp(login, "NU") == 0) {
            free(saved);
            continue;
        }

        printf("  %s (attempts: %s)\n", login, attempts);
        free(saved);
    }

    fclose(f);
    return SUCCESS;
}

int change_attempts(const char* db_filename, const char* login, int new_limit) {
    FILE* f = fopen(db_filename, "r+");
    if (!f) return FILE_ERROR;

    char line[512];
    long offset = 0;

    while (fgets(line, sizeof(line), f)) {
        long line_start = offset;
        offset += strlen(line);

        // Копия строки для обработки
        char temp[512];
        strcpy(temp, line);

        char* username = strtok(temp, " \t\n");
        char* hash     = strtok(NULL, " \t\n");

        if (username && hash && strcmp(username, login) == 0) {
            // Помечаем старую строку как NU
            fseek(f, line_start, SEEK_SET);
            fputs("NU", f);
            fclose(f);

            // Записываем новую строку в конец
            f = fopen(db_filename, "a");
            if (!f) return FILE_ERROR;

            fprintf(f, "%s %s %d\n", login, hash, new_limit);
            fclose(f);
            return SUCCESS;
        }
    }

    fclose(f);
    return NOT_FOUND;
}

int encrypt_password(const char* pin, char** hashed) {
    char* salt = crypt_gensalt_ra(NULL, 0, NULL, 0);
    if (!salt) return MEMORY_ERROR;

    void* ctx = NULL;
    int ctx_sz = 0;

    char* result = crypt_ra(pin, salt, &ctx, &ctx_sz);
    free(salt);
    if (!result) return MEMORY_ERROR;

    *hashed = strdup(result);
    if (!*hashed) {
        free(ctx);
        return MEMORY_ERROR;
    }

    free(ctx);
    return SUCCESS;
}

int compare_passwords(const char* pin, const char* hashed, int* result) {
    void* ctx = NULL;
    int ctx_sz = 0;

    char* hashed_input = crypt_ra(pin, hashed, &ctx, &ctx_sz);
    if (!hashed_input) return MEMORY_ERROR;

    *result = strcmp(hashed, hashed_input);
    free(ctx);
    return SUCCESS;
}

// Проверка: допустим ли логин
static int validate_login(const char* login) {
    if (!login || strlen(login) > MAX_LOGIN_LEN) return 0;
    for (size_t i = 0; i < strlen(login); i++) {
        if (!isalnum(login[i])) return 0;
    }
    return 1;
}

// Проверка: допустим ли pin
static int validate_pin(const char* pin) {
    if (!pin) return 0;
    for (size_t i = 0; i < strlen(pin); i++) {
        if (!isdigit(pin[i])) return 0;
    }
    long val = strtol(pin, NULL, 10);
    return (val >= 0 && val <= 100000);
}

int find_user(const char* db_filename, const char* login) {
    FILE* f = fopen(db_filename, "r");
    if (!f) return FILE_ERROR;

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        char* token = strtok(line, " \t\n");
        if (token && strcmp(token, login) == 0) {
            fclose(f);
            return SUCCESS;
        }
    }

    fclose(f);
    return NOT_FOUND;
}

int register_user(const char* db_filename, const char* login, const char* pin) {
    if (!validate_login(login) || !validate_pin(pin)) return INVALID_FORMAT;

    if (find_user(db_filename, login) == SUCCESS) {
        return LOGIN_EXISTS;
    }

    char* hashed = NULL;
    int res = encrypt_password(pin, &hashed);
    if (res != SUCCESS) return res;

    FILE* f = fopen(db_filename, "a");
    if (!f) {
        free(hashed);
        return FILE_ERROR;
    }

    // Формат: login hashed_pin attempts
    fprintf(f, "%s %s -1\n", login, hashed);
    fclose(f);
    free(hashed);

    return SUCCESS;
}

int login_user(const char* db_filename, const char* login, const char* pin, struct User* user) {
    if (!validate_login(login) || !validate_pin(pin)) return INVALID_FORMAT;

    FILE* f = fopen(db_filename, "r");
    if (!f) return FILE_ERROR;

    char line[512];
    int line_num = 0;

    while (fgets(line, sizeof(line), f)) {
        line_num++;
        char* saved = strdup(line);
        char* username = strtok(line, " \t\n");
        char* hash = strtok(NULL, " \t\n");
        char* attempts_str = strtok(NULL, " \t\n");

        if (username && hash && attempts_str && strcmp(username, login) == 0) {
            int cmp = 1;
            compare_passwords(pin, hash, &cmp);

            if (cmp == 0) {
                user->login = strdup(username);
                user->attempts = atoi(attempts_str);
                user->is_admin = (line_num == 1); // первый — админ
                user->is_logged_in = 1;
                fclose(f);
                free(saved);
                return SUCCESS;
            } else {
                free(saved);
                fclose(f);
                return WRONG_PASSWORD;
            }
        }

        free(saved);
    }

    fclose(f);
    return NOT_FOUND;
}
