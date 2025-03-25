#ifndef ENUMS_H
#define ENUMS_H

enum ReturnCode {
    SUCCESS = 0,
    MEMORY_ERROR,
    FILE_ERROR,
    NULL_ERROR,
    LOGIN_EXISTS,
    WRONG_PASSWORD,
    NOT_FOUND,
    NOT_LOGGED_IN,
    ALREADY_LOGGED,
    INVALID_ARGS,
    INVALID_FORMAT,
    NO_ATTEMPTS,
    DENIED,
    EXIT_CODE
};

#endif
