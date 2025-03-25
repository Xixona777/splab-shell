
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "user.h"
#include "db.h"
#include "enums.h"
#include "date_utils.h"

#define DB_FILE "db.txt"
#define MAX_INPUT 256

void print_current_time() {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    printf("Current time: %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
}

void print_current_date() {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    printf("Current date: %02d:%02d:%d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
}

int parse_input(char* input, char** command, char** arg1, char** arg2) {
    *command = strtok(input, " \t\n");
    *arg1 = strtok(NULL, " \t\n");
    *arg2 = strtok(NULL, " \t\n");
    return (*command != NULL);
}

int main() {
    struct User user = {0};

    FILE* test_db = fopen(DB_FILE, "a");
    if (test_db == NULL) {
        printf("Cannot access database file!\n");
        return 1;
    }
    fclose(test_db);

    printf("Welcome to the shell. Please login or register.\n");
    printf("Type 'help' to see available commands.\n");

    while (1) {
        printf(user.is_logged_in ? "[%s]> " : "[unauthorized]> ", user.login ? user.login : "");
        char input[MAX_INPUT];
        if (!fgets(input, sizeof(input), stdin)) continue;

        char *cmd, *arg1, *arg2;
        if (!parse_input(input, &cmd, &arg1, &arg2)) continue;

        if (user.is_logged_in && !user.is_admin && user.attempts == 0) {
            printf("No more allowed commands in this session.\n");
            continue;
        }

        if (strcmp(cmd, "exit") == 0) break;

        else if (strcmp(cmd, "register") == 0) {
            if (!arg1 || !arg2) {
                printf("Usage: register <login> <pin>\n");
                continue;
            }
            int res = register_user(DB_FILE, arg1, arg2);
            if (res == SUCCESS) printf("User registered!\n");
            else if (res == LOGIN_EXISTS) printf("User already exists.\n");
            else if (res == INVALID_FORMAT) printf("Invalid login or pin.\n");
            else printf("Registration failed.\n");
        }

        else if (strcmp(cmd, "login") == 0) {
            if (!arg1 || !arg2) {
                printf("Usage: login <login> <pin>\n");
                continue;
            }
            if (user.is_logged_in) {
                printf("Already logged in.\n");
                continue;
            }
            int res = login_user(DB_FILE, arg1, arg2, &user);
            if (res == SUCCESS) printf("Login successful!\n");
            else if (res == WRONG_PASSWORD) printf("Wrong pin.\n");
            else if (res == NOT_FOUND) printf("User not found.\n");
            else printf("Login error.\n");
        }

        else if (strcmp(cmd, "logout") == 0) {
            if (!user.is_logged_in) {
                printf("You are not logged in.\n");
                continue;
            }
            printf("User %s logged out.\n", user.login);
            free(user.login);
            user.login = NULL;
            user.is_logged_in = 0;
            user.is_admin = 0;
            user.attempts = -1;
        }

        else if (strcmp(cmd, "time") == 0) {
            if (!user.is_logged_in) {
                printf("Login first.\n");
                continue;
            }
            print_current_time();
            if (!user.is_admin) user.attempts--;
        }

        else if (strcmp(cmd, "date") == 0) {
            if (!user.is_logged_in) {
                printf("Login first.\n");
                continue;
            }
            print_current_date();
            if (!user.is_admin) user.attempts--;
        }

        else if (strcmp(cmd, "howmuch") == 0) {
            if (!user.is_logged_in || !arg1 || !arg2 || strlen(arg2) != 2 || arg2[0] != '-') {
                printf("Usage: howmuch <dd/mm/yyyy> <-s|-m|-h|-y|-d>\n");
                continue;
            }
            struct Date input_date;
            if (!parse_date(arg1, &input_date)) {
                printf("Invalid date format!\n");
                continue;
            }
            time_t now = time(NULL);
            struct tm* t = localtime(&now);
            struct Date today = { t->tm_mday, t->tm_mon + 1, t->tm_year + 1900 };
            int days = days_between(today, input_date);

            switch (arg2[1]) {
                case 's': printf("Seconds: %d\n", days * 24 * 3600); break;
                case 'm': printf("Minutes: %d\n", days * 24 * 60); break;
                case 'h': printf("Hours: %d\n", days * 24); break;
                case 'y': printf("Years: %d\n", days / 365); break;
                case 'd': printf("Days: %d\n", days); break;
                default: printf("Unknown flag. Use -s/-m/-h/-y/-d\n"); break;
            }

            if (!user.is_admin) user.attempts--;
        }

        else if (strcmp(cmd, "sanctions") == 0) {
            if (!user.is_logged_in || !user.is_admin || !arg1 || !arg2) {
                printf("Usage: sanctions <login> <number>\n");
                continue;
            }

            int num = atoi(arg2);
            if (num < 0) {
                printf("Number must be >= 0\n");
                continue;
            }

            printf("Input 12345 to confirm: ");
            char confirm[32];
            if (!fgets(confirm, sizeof(confirm), stdin)) {
                printf("Input error.\n");
                continue;
            }
            confirm[strcspn(confirm, "\n")] = '\0';
            if (strcmp(confirm, "12345") != 0) {
                printf("Confirmation failed.\n");
                continue;
            }

            int res = change_attempts(DB_FILE, arg1, num);
            if (res == SUCCESS) printf("Sanctions updated.\n");
            else if (res == NOT_FOUND) printf("User not found.\n");
            else printf("Sanction error.\n");
        }

        else if (strcmp(cmd, "listusers") == 0) {
            if (!user.is_logged_in || !user.is_admin) {
                printf("Only admin can use this command.\n");
                continue;
            }
            int res = list_users(DB_FILE);
            if (res != SUCCESS) printf("Failed to read database.\n");
        }

        else if (strcmp(cmd, "help") == 0) {
            if (!user.is_logged_in) {
                printf("Available commands:\n");
                printf("  register <login> <pin>\n");
                printf("  login <login> <pin>\n");
                printf("  exit\n");
            } else {
                printf("Available commands:\n");
                printf("  logout\n");
                printf("  time\n");
                printf("  date\n");
                printf("  howmuch <dd/mm/yyyy> <-s|-m|-h|-y|-d>\n");
                if (user.is_admin) {
                    printf("  sanctions <login> <number>  [admin only]\n");
                    printf("  listusers [admin only]\n");
                }
                printf("  help\n");
                printf("  exit\n");
            }
        }

        else {
            printf("Unknown command.\n");
        }
    }

    free(user.login);
    return 0;
}
