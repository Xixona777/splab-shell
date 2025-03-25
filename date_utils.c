#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "date_utils.h"

// Парсит дату из строки "dd/mm/yyyy"
int parse_date(const char* str, struct Date* out) {
    if (!str || !out) return 0;

    if (sscanf(str, "%d/%d/%d", &out->day, &out->month, &out->year) != 3) {
        return 0;
    }

    if (out->day <= 0 || out->day > 31 || out->month <= 0 || out->month > 12 || out->year < 1900) {
        return 0;
    }

    return 1;
}

// Определение високосного года
int is_leap(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

// Кол-во дней в месяце
int days_in_month(int month, int year) {
    const int days[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (month == 2 && is_leap(year)) return 29;
    return days[month - 1];
}

// Перевод даты в "кол-во дней с 01.01.0000"
long date_to_days(struct Date d) {
    long days = d.day;

    for (int y = 0; y < d.year; y++) {
        days += is_leap(y) ? 366 : 365;
    }

    for (int m = 1; m < d.month; m++) {
        days += days_in_month(m, d.year);
    }

    return days;
}

// Разница в днях между двумя датами
int days_between(struct Date d1, struct Date d2) {
    long a = date_to_days(d1);
    long b = date_to_days(d2);
    return (int)(a > b ? a - b : b - a);
}
