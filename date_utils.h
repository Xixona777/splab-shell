#ifndef DATE_UTILS_H
#define DATE_UTILS_H

struct Date {
    int day;
    int month;
    int year;
};

int parse_date(const char* str, struct Date* out);
int days_between(struct Date d1, struct Date d2);

#endif
