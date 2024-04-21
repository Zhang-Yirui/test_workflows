#include "main.h"

char *Riming[30] = {"初一", "初二", "初三", "初四", "初五", "初六", "初七", "初八", "初九", "初十", "十一", "十二",
                    "十三", "十四", "十五", "十六", "十七", "十八", "十九", "二十", "廿一", "廿二", "廿三", "廿四",
                    "廿五", "廿六", "廿七", "廿八", "廿九", "三十"};
char *Yueming[12] = {"正月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "冬月", "腊月"};
char *Tiangan[10] = {"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"};
char *Dizhi[12] = {"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"};
char *Shengxiao[12] = {"鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"};
char *Jieqi[24] = {"立春", "雨水", "惊蛰", "春分", "清明", "谷雨", "立夏", "小满", "芒种", "夏至", "小暑", "大暑",
                   "立秋", "处暑", "白露", "秋分", "寒露", "霜降", "立冬", "小雪", "大雪", "冬至", "小寒", "大寒"};
char *Xingqi[7] = {"日", "一", "二", "三", "四", "五", "六"};
char *Other[4] = {"小", "大", "", "闰"};

struct {
    char show_help;//'h' or '\0'
    char show_version;//'v' or '\0'
    char is_blocked;//0b000
    char data_type;//'d', 'j' or 's'
    int date[3];//year, month, day
    char string[512];
} test = {0, 0, 0b000, 'd', {0}, {0}};

int main(int argc, char *argv[]) {
    getTodayDate();
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                test.show_help = 'e';
                break;
            } else if (strcmp(argv[i], "--help-zh") == 0) {
                test.show_help = 'z';
                break;
            } else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
                test.show_version = 'v';
                break;
            } else if (strcmp(argv[i], "--today") == 0 || strcmp(argv[i], "-t") == 0) {
                if (CheckBit(test.is_blocked, 0) == 0) {
                    SetBit(test.is_blocked, 0);
                }
            } else if (strncmp(argv[i], "--date", 6) == 0 || strncmp(argv[i], "-d", 2) == 0) {
                int str_len = (int) strlen(argv[i]);
                if (str_len == 2 || str_len == 6) {
                    if (CheckBit(test.is_blocked, 0) == 0) {
                        SetBit(test.is_blocked, 0);
                    }
                } else if ((str_len != 10 && str_len != 14) || !getDateFromInput(argv[i])) {
                    printf("Date error!\n");
                    test.show_help = 'e';
                    break;
                }
            } else if (strcmp(argv[i], "--json") == 0 || strcmp(argv[i], "-j") == 0) {
                if (CheckBit(test.is_blocked, 2) == 0) {
                    test.data_type = 'j';
                    SetBit(test.is_blocked, 2);
                }
            } else if (strcmp(argv[i], "--string") == 0 || strcmp(argv[i], "-s") == 0) {
                if (CheckBit(test.is_blocked, 2) == 0) {
                    test.data_type = 's';
                    SetBit(test.is_blocked, 2);
                }
            } else {
                printf("Parameter(s) error!\n");
                test.show_help = 'e';
                break;
            }
        } else {
            printf("Parameter(s) error!\n");
            test.show_help = 'e';
            break;
        }
    }

    if (test.show_help) {
        if (test.show_help == 'e') {
            help_en();
        } else if (test.show_help == 'z') {
            help_zh();
        }
        return 'h';
    }

    if (test.show_version == 'v') {
        printf("%s\n", getVersion());
        return 'v';
    }
    switch (test.data_type) {
        case 'j':
            printf("%s", toJson(test.date[0], test.date[1], test.date[2]));
            break;
        case 's':
            printf("%s", toString(test.date[0], test.date[1], test.date[2]));
            break;
        default:
            printf("%s", toData(test.date[0], test.date[1], test.date[2]));
            break;
    }
    return 0;
}

void help_en() {
    printf("\nUsage: calendar [options ...]\n");
    printf("\n  Show the calendar data of the given date, or the current date if no date is given.\n");
    printf("Options: Operation instruction,  if there is no instruction, defaults to the today date,\n");
    printf("         and the output format is space-spaced calendar data.\n");
    printf("  --date[date string],\n");
    printf("   -d[date string]          Input date.\n");
    printf("   -t, --today              Date today.\n");
    printf("  --json, -j                Output calendar data in JSON format.\n");
    printf("  --string, -s              Output calendar string.\n");
    printf("  --version, -v             Output version information and exit.\n");
    printf("  --help, -h, --help-zh     Display help information and exit.\n");
    printf("\n");
    printf("date string: The date entered or the current date if not entered.\n");
    printf("  format: YYYYMMDD\n");
    printf("  range:\n");
    printf("       YYYY: [1600, 6399]\n");
    printf("         MM: [1, 12]\n");
    printf("         DD: [1, max days]\n");
    printf("\n");
    printf("Report bugs to <%s/issues>.\n\n", REPOSITORY_URL);
}

void help_zh() {
    printf("\n使用: calendar [options ...]\n");
    printf("\n  显示给定日期的日历数据, 如果没有给出日期, 则显示当前日期.\n\n");
    printf("options: 操作指令, 如果没有任何指令, 默认为今天日期, 输出格式为空格间隔的日历数据.\n");
    printf("  -d[date string],\n");
    printf(" --date[date string]        输入日期.\n");
    printf("  -t, --today               日期为今天.\n");
    printf("  -j, --json                以JSON格式输出日历数据.\n");
    printf("  -s, --string              输出日历字符串.\n");
    printf("  -v, --version             输出版本信息并退出.\n");
    printf("  -h, --help, --help-zh     显示帮助信息并退出.\n");
    printf("\n");
    printf("date string: 输入的日期, 如果不输入则为当前日期.\n");
    printf("  格式: YYYYMMDD\n");
    printf("  范围:\n");
    printf("       YYYY: [1600, 6399]\n");
    printf("         MM: [1, 12]\n");
    printf("         DD: [1, 本月最大天数]\n");
    printf("\n");
    printf("提交BUG请前往 <%s/issues>.\n\n", REPOSITORY_URL);
}

char *getVersion() {
#ifdef VERSION
    return VERSION;
#else
    return NULL;
#endif
}

void getTodayDate() {
    time_t now = time(NULL); //获取当前时间戳
    struct tm *today = localtime(&now);//获取今天日期和当前时间
    test.date[0] = today->tm_year + 1900;
    test.date[1] = today->tm_mon + 1;
    test.date[2] = today->tm_mday;
}

unsigned char getDateFromInput(char *date_str) {
    char *p_str;
    for (p_str = date_str; *p_str != '\0'; p_str++) {//找到日期开始的位置
        if ('0' <= *p_str && *p_str <= '9') {
            break;
        }
    }
    if (*p_str == '\0') {//找不到, 例如-d_abandon
        return 0;
    }
    for (char *p = p_str; *p != '\0'; p++) {//确保全是数字, 如-d1abandon
        if (!('0' <= *p && *p <= '9')) {
            return 0;
        }
    }
    if (CheckBit(test.is_blocked, 0) == 0) {
        SetBit(test.is_blocked, 0);
        test.date[0] = (int) (p_str[0] - '0') * 1000 + (int) (p_str[1] - '0') * 100 +
                       (int) (p_str[2] - '0') * 10 + (int) (p_str[3] - '0') * 1;
        test.date[1] = (int) (p_str[4] - '0') * 10 + (int) (p_str[5] - '0') * 1;
        test.date[2] = (int) (p_str[6] - '0') * 10 + (int) (p_str[7] - '0') * 1;

    }
    return 1;
}

char *toData(int year, int month, int day) {
    Calendar cal;
    getCalendarInfo(year, month, day, &cal);
    sprintf(test.string, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
            cal.solar.year, cal.solar.month, cal.solar.day, cal.solar.week, cal.solar.daysOfMonth,
            cal.solar.daysOfYear, cal.solar.dayOfYear, cal.solar.weekOfYear, cal.lunar.lunarYear,
            cal.lunar.lunarMonth, cal.lunar.lunarDay, cal.lunar.tiangan, cal.lunar.dizhi, cal.lunar.shengxiao,
            cal.lunar.isLeapMonth, cal.lunar.isDXYue, cal.jieqi.jieqi, cal.jieqi.nextJieqi,
            cal.jieqi.nextJieqiRemainDays, cal.jieqi.isJieqiToday);
    return test.string;
}

char *toJson(int year, int month, int day) {
    Calendar cal;
    getCalendarInfo(year, month, day, &cal);
    sprintf(test.string,
            "{\"result\":\"success\",\"data\":{\"solarInfo\":{\"year\":%d,\"month\":%d,\"day\":%d,\"week\":%d,\"daysOfMonth\":%d,\"daysOfYear\":%d,\"dayOfYear\":%d,\"weekOfYear\":%d},\"lunarInfo\":{\"lunarYear\":%d,\"lunarMonth\":%d,\"lunarDay\":%d,\"tiangan\":%d,\"dizhi\":%d,\"shengxiao\":%d,\"isLeapMonth\":%s,\"isDXYue\":%s},\"jieqiInfo\":{\"jieqi\":%d,\"nextJieqi\":%d,\"nextJieqiRemainDays\":%d,\"isJieqiToday\":%s}}}",
            cal.solar.year, cal.solar.month, cal.solar.day, cal.solar.week, cal.solar.daysOfMonth,
            cal.solar.daysOfYear, cal.solar.dayOfYear, cal.solar.weekOfYear, cal.lunar.lunarYear,
            cal.lunar.lunarMonth, cal.lunar.lunarDay, cal.lunar.tiangan, cal.lunar.dizhi, cal.lunar.shengxiao,
            cal.lunar.isLeapMonth ? "true" : "false", cal.lunar.isDXYue ? "true" : "false", cal.jieqi.jieqi,
            cal.jieqi.nextJieqi, cal.jieqi.nextJieqiRemainDays, cal.jieqi.isJieqiToday ? "true" : "false");
    return test.string;
}

char *toString(int year, int month, int day) {
    Calendar cal;
    if (getCalendarInfo(year, month, day, &cal)) {
        sprintf(test.string, "%4d-%02d-%02d 星期%s %s%s(%s)年 %s%s(%s)%s %s%s 距离%s还有%d天",
                cal.solar.year, cal.solar.month, cal.solar.day, Xingqi[cal.solar.week],
                Tiangan[cal.lunar.tiangan - 1], Dizhi[cal.lunar.dizhi - 1], Shengxiao[cal.lunar.shengxiao - 1],
                Other[cal.lunar.isLeapMonth + 2], Yueming[cal.lunar.lunarMonth - 1], Other[cal.lunar.isDXYue],
                Riming[cal.lunar.lunarDay - 1], Jieqi[cal.jieqi.jieqi - 1], cal.jieqi.isJieqiToday ? "*" : "",
                Jieqi[cal.jieqi.nextJieqi - 1], cal.jieqi.nextJieqiRemainDays);
        return test.string;
    }
    return nullptr;
}