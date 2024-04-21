#ifndef C_CALENDAR_MAIN_H
#define C_CALENDAR_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "calendar.h"

#define nullptr NULL
#define VERSION "v1.0.1"
#define REPOSITORY_URL "https://github.com/jingsong88/calendar/issues"

#define CheckBit(x, n) (((x) >> (n)) & 1)
#define SetBit(x, n) (x |= (1 << (n)))
#define ResetBit(x, n) (x &= ~(1 << (n)))
#define ReverseBit(x, n) (x ^= (1 << (n)))

void help_en();

void help_zh();

char *getVersion();

char *toData(int year, int month, int day);

char *toJson(int year, int month, int day);

char *toString(int year, int month, int day);

void getTodayDate();

unsigned char getDateFromInput(char *date_str);

#endif //C_CALENDAR_MAIN_H
