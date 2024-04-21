#include "calendar.h"
#include "LunarData.h"
#include "memory.h"

int DayOrdinal[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
int DayOrdinalLeap[13] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

void p_memset(void *dst, int val, int size) {
    unsigned char *Dst = (unsigned char *) dst;
    for (int i = 0; i < size; ++i) {
        Dst[i] = (unsigned char) val;
    }
}

bool p_isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

bool p_checkDate(int year, int month, int day, bool checkAllYear) {
    // all year -> [1600, 6399]
    if (checkAllYear) {
        if (year < 1600 || year > 6399) {
            return false;
        }
    } else {
        if (year < START_YEAR || year >= END_YEAR) {
            return false;
        }
    }
    if (month < 1 || month > 12) {
        return false;
    }
    if (day < 1 || day > 31) {
        return false;
    } else {
        if (month == 4 || month == 6 || month == 9 || month == 11) {
            if (day == 31) {
                return false;
            }
        } else if (month == 2) {
            if (p_isLeapYear(year)) {
                if (day > 29) {
                    return false;
                }
            } else {
                if (day > 28) {
                    return false;
                }
            }
        }
    }
    return true;
}

int p_getDayOrdinal(int year, int month, int day) {
    int ordinal;
    if (p_isLeapYear(year)) {//判断是否为闰年
        ordinal = DayOrdinalLeap[month - 1] + day - 1;//元旦为序数0, 因此减1
    } else {
        ordinal = DayOrdinal[month - 1] + day - 1;
    }
    return ordinal;
}

int p_getDateFromOrdinal(int year, int ordinal, int *month, int *day) {
    int *dayOrdinal = p_isLeapYear(year) ? DayOrdinalLeap : DayOrdinal;
    *month = *day = 0;
    for (int i = 0; i < 12; ++i) {
        if (dayOrdinal[i] <= ordinal && ordinal < dayOrdinal[i + 1]) {//找出月份
            *month = i + 1;
            *day = ordinal - dayOrdinal[i] + 1;//计算出“日”
            break;
        }
    }
    return *month << 8 | *day;
}

int p_getDaysOfMonth(int year, int month) {
    int days = 0x3f3f3f3f;
    if (month == 1 || month == 12) {
        days = 31;
    } else if (month == 2) {
        days = p_isLeapYear(year) ? 29 : 28;
    } else if (3 <= month && month <= 11) {
        days = DayOrdinal[month] - DayOrdinal[month - 1];
    }
    return days;
}

int p_getDayOfWeek(int year, int month, int day) {
    int week, dayOrdinal = p_getDayOrdinal(year, month, day);
    dayOrdinal++;//一年中的第几天, 因为GetDayOrdinal所得到的是索引, 因此要加1
    year -= 1;
    week = (year + year / 4 - year / 100 + year / 400 + dayOrdinal) % 7;//这个只是算星期的通用公式
    return week;
}

int p_getMonthInfo(int lunarYear) {
    int data = 0, startPos = (lunarYear - START_YEAR) * 3;
    data |= monthInfo[startPos + 0] << 16;
    data |= monthInfo[startPos + 1] << 8;
    data |= monthInfo[startPos + 2] << 0;
    return data;
}

int p_getLunarLeapMonth(int lunarYear) {
    return (p_getMonthInfo(lunarYear) >> 13) & 0x0F;
}

int p_getLunarNewYearOrdinal(int lunarYear) {
    return (p_getMonthInfo(lunarYear) >> 17) & 0x3F;
}

int p_getLunarDaysOfMonth(int lunarYear, int lunarMonth, bool leapMonth) {
    int DX_data;//该年大小月情况
    int Acc_LeapMonth;
    if (lunarYear == START_YEAR - 1) {//农历还在起始年份的前一年
#if (PRE_LEAP_INDEX == -1)//无闰月
        if (!leapMonth) {
            return preMonth[lunarMonth - 9] - preMonth[lunarMonth - 10];
        } else {
            return 0;
        }
#else
        Acc_LeapMonth = PRE_LEAP_INDEX + 9;
        if (Acc_LeapMonth > lunarMonth) {
            if (!leapMonth) {
                return preMonth[lunarMonth - 9] - preMonth[lunarMonth - 10];
            } else {
                return 0;
            }
        } else {
            if ((leapMonth && lunarMonth == Acc_LeapMonth) || lunarMonth > Acc_LeapMonth) {
                return preMonth[lunarMonth - 8] - preMonth[lunarMonth - 9];
            } else {
                return preMonth[lunarMonth - 9] - preMonth[lunarMonth - 10];
            }
        }
#endif // PRE_LEAP_INDEX == -1
    }

    DX_data = p_getMonthInfo(lunarYear) & 0x1FFF;//整年大小月情况
    Acc_LeapMonth = p_getLunarLeapMonth(lunarYear);//获取真实闰月月份

    if (leapMonth) {//指定查询的当前月是闰月
        if (Acc_LeapMonth != lunarMonth) {
            return 0;//不存在的闰月
        }
        DX_data >>= lunarMonth;//闰月要多找一位
    } else {
        if (Acc_LeapMonth > 0) {//存在闰月
            if (lunarMonth <= Acc_LeapMonth) {
                DX_data >>= (lunarMonth - 1);//月份在闰月之前, 和无闰月一样
            } else {
                DX_data >>= lunarMonth;//月份在闰月之后, 倒找需要多找一位
            }
        } else {
            DX_data >>= (lunarMonth - 1);//无闰月
        }
    }
    return (DX_data & 0b1) == 1 ? 30 : 29;
}

bool p_lunarExpandDX(int lunarYear, int daysOfMonth[15]) {
    int pos = 0;//数组写入位置
    unsigned int leapMonth;//闰月
    p_memset(daysOfMonth, 0, 15);//清空数组
    if (lunarYear == START_YEAR) {
#if (PRE_LEAP_INDEX == -1)//处理起始年份之前一年数据
        daysOfMonth[pos] = preMonth[2] - preMonth[1];//农历上一年十一月总天数
        pos++;
#else
#if (PRE_LEAP_INDEX == 1)//闰十月
        daysOfMonth[pos] = preMonth[3] - preMonth[2];
        pos++;
#else//闰十一月或闰十二月
        daysOfMonth[pos] = preMonth[2] - preMonth[1];
        pos++;
        daysOfMonth[pos] = preMonth[3] - preMonth[2];
        pos++;
#endif
#endif
        daysOfMonth[pos] = (int) p_getLunarNewYearOrdinal(lunarYear) - preMonth[2];//(闰)十二月
        pos++;
    } else {
        leapMonth = p_getLunarLeapMonth(lunarYear - 1);//取得前一个农历年的闰月情况
        if (leapMonth < 11) {//一月至十月的闰月
            daysOfMonth[pos] = p_getLunarDaysOfMonth(lunarYear - 1, 11, false);//取上一年十一月天数
            pos++;
            daysOfMonth[pos] = p_getLunarDaysOfMonth(lunarYear - 1, 12, false);//取上一年十二月天数
            pos++;
        } else {
            daysOfMonth[pos] = p_getLunarDaysOfMonth(lunarYear - 1, 11, false);//取上一年十一月的天数
            pos++;
            if (leapMonth == 11) {//闰十一月
                daysOfMonth[pos] = p_getLunarDaysOfMonth(lunarYear - 1, 11, true);//取上一年闰十一月的天数
                pos++;
                daysOfMonth[pos] = p_getLunarDaysOfMonth(lunarYear - 1, 12, false);//取上一年十二月天数
                pos++;
            } else if (leapMonth == 12) {//闰十二月
                daysOfMonth[pos] = p_getLunarDaysOfMonth(lunarYear - 1, 12, false);//取上一年十二月天数
                pos++;
                daysOfMonth[pos] = p_getLunarDaysOfMonth(lunarYear - 1, 12, true);//取上一年闰十二月天数
                pos++;
            }
        }
    }

    leapMonth = p_getLunarLeapMonth(lunarYear);//获取当前农历年的闰月情况
    if (leapMonth == 0) {//无闰月
        for (int i = 0; i < 12; ++i) {
            daysOfMonth[pos] = p_getLunarDaysOfMonth(lunarYear, i + 1, false);//取每个农历月天数
            pos++;
        }
    } else {
        for (int i = 0; i < 12; ++i) {
            if (i == leapMonth) {
                daysOfMonth[pos] = p_getLunarDaysOfMonth(lunarYear, i, true);//取闰月的天数
                pos++;
            }
            daysOfMonth[pos] = p_getLunarDaysOfMonth(lunarYear, i + 1, false);//取非闰月的天数
            pos++;
        }
    }
    return true;
}

bool p_getJieqiData(int year, int month, int jieqiDate[2], int jieqiOrdinal[2]) {
    int index = (year - START_YEAR) * 3;//对应每公历年首个节气所在字节的索引
    int xiaohanOrder;//小寒年内序数
    int jqData;//节气数据
    int curJQData;//当前计算的节气数据
    int days;//当前节气距离该年小寒的天数
    int iRem;

    jqData = STSource[STIndex[index]];
    xiaohanOrder = (jqData >> 14) + 3;//加上3，转为小寒的年内序数
    curJQData = (jqData >> 12) & 0x03;//当前计算的节气与上一个节气的天数差信息

    if (month == 1) {
        jieqiOrdinal[0] = xiaohanOrder;//第一个节气的年内序数
        jieqiDate[0] = xiaohanOrder + 1;//加1转到日期

        jieqiOrdinal[1] = curJQData + 14 + xiaohanOrder;//第二个节气的年内序数
        jieqiDate[1] = curJQData + 14 + xiaohanOrder + 1;//大寒：小寒的年内序数加上距离小寒的天数
    } else {
        days = curJQData + 14;//距离小寒的天数，当前为大寒距离小寒的天数
        days += xiaohanOrder;//加上小寒，转为年内序数
        for (int i = 1; i < month; i++) {
            iRem = i % 4;
            curJQData = (jqData >> (18 - ((iRem + 1) << 2))) & 0x03;
            days += curJQData + 14;
            curJQData = (jqData >> (16 - ((iRem + 1) << 2))) & 0x03;
            days += curJQData + 14;
            if (iRem == 3) {
                jqData = STSource[STIndex[index + (i + 1) / 4]];
            }
        }
        jieqiOrdinal[0] = days - curJQData - 14;//第一个节气的年内序数
        jieqiOrdinal[1] = days;//第二个节气的年内序数

        p_getDateFromOrdinal(year, jieqiOrdinal[0], &month, jieqiDate);//wMonth中的第一个节气
        p_getDateFromOrdinal(year, jieqiOrdinal[1], &month, jieqiDate + 1);//wMonth中的第二个节气
    }

    return true;
}

SolarInfo p_getSolarInfo(int year, int month, int day) {
    SolarInfo solarDate;
    short base = (short) p_getDayOfWeek(year, 1, 1);
    solarDate.year = (short) year;
    solarDate.month = (short) month;
    solarDate.day = (short) day;
    solarDate.week = (short) p_getDayOfWeek(year, month, day);
    solarDate.daysOfMonth = (short) p_getDaysOfMonth(year, month);
    solarDate.dayOfYear = (short) p_getDayOrdinal(year, month, day);
    solarDate.daysOfYear = p_isLeapYear(year) ? 366 : 365;
    solarDate.weekOfYear = (short) ((solarDate.dayOfYear + base) / 7 + 1);
    return solarDate;
}

LunarInfo p_getLunarInfo(int year, int month, int day) {
    int index, daysNum, lunarNewYear, remain_days;//循环变量, 年内序数, 农历新年的公历年内序数, 剩余天数
    int daysOfLunarMonth[15] = {0};//存放农历月份天数
    int preLeapMonth, leapMonth = 0;//农历上一年闰月, 今年闰月
    LunarInfo lunarInfo;

    lunarInfo.lunarYear = (short) year;
    lunarInfo.isLeapMonth = false;//默认不是闰月//
    daysNum = p_getDayOrdinal(year, month, day);//获取日期的年内序数
    lunarNewYear = p_getLunarNewYearOrdinal(lunarInfo.lunarYear);//获取春节的年内序数
    remain_days = daysNum - lunarNewYear;//距离农历新年的天数
    p_lunarExpandDX(lunarInfo.lunarYear, daysOfLunarMonth);//获取月份天数, 数组从上一年十一月开始到今年（闰）十二月, 包含闰月
    preLeapMonth = p_getLunarLeapMonth(lunarInfo.lunarYear - 1);//获取上一年的闰月
    if (preLeapMonth == 0) {//上一年没有闰月
        leapMonth = p_getLunarLeapMonth(lunarInfo.lunarYear);//查询今年的闰月
    }
    index = preLeapMonth > 10 ? 3 : 2;//上一年十一月或十二月有闰月时, 今年正月在 daysOfLunarMonth 中的索引为3, 否则为2
    if (lunarNewYear > daysNum) {//年内序数早于农历新年
        lunarInfo.lunarYear -= 1;//农历年减1
        while (remain_days < 0) {
            index--;//第一次先减去是因为当前i是正月, 减1表示上一年十二月（或闰十二月）
            remain_days += daysOfLunarMonth[index];//加上上一年十二月、十一月的总天数（含闰月）直到日数大于0
        }
        lunarInfo.isDXYue = daysOfLunarMonth[index] == 30;
        if (preLeapMonth < 11) {//如果上一年十一月或十二月不存在闰月
            lunarInfo.lunarMonth = (short) (11 + index);
            lunarInfo.isLeapMonth = false;
        } else if (preLeapMonth == 11) {//闰十一月
            if (index == 0) {//十一月（即在闰月之前）
                lunarInfo.lunarMonth = (short) (11 + index);//转换到月份
                lunarInfo.isLeapMonth = false;
            } else {
                lunarInfo.lunarMonth = (short) (10 + index);//转换到月份
                lunarInfo.isLeapMonth = lunarInfo.lunarMonth == preLeapMonth;
            }
        } else if (preLeapMonth == 12) {//闰十二月
            if (index < 2) {//十一月（即在闰月之前）
                lunarInfo.lunarMonth = (short) (11 + index);//转换到月份
                lunarInfo.isLeapMonth = false;
            } else {
                lunarInfo.lunarMonth = (short) (10 + index);//转换到月份
                lunarInfo.isLeapMonth = lunarInfo.lunarMonth == preLeapMonth;
            }
        }
        lunarInfo.lunarDay = (short) remain_days;
    } else {
        while (remain_days >= daysOfLunarMonth[index]) {
            remain_days -= daysOfLunarMonth[index];//寻找农历月
            index++;//移至下个月
        }
        if (preLeapMonth > 10) {
            lunarInfo.lunarMonth = (short) (index - 2);
            lunarInfo.isLeapMonth = false;
        } else {
            if (0 < leapMonth && leapMonth <= index - 2) {
                lunarInfo.lunarMonth = (short) (index - 2);
                lunarInfo.isLeapMonth = lunarInfo.lunarMonth == leapMonth;
            } else {
                lunarInfo.lunarMonth = (short) (index - 1);
                lunarInfo.isLeapMonth = false;
            }
        }
        lunarInfo.lunarDay = (short) (remain_days);
    }
    lunarInfo.lunarDay += 1;//索引转换到数量
    lunarInfo.tiangan = (short) ((lunarInfo.lunarYear - 4) % 10 + 1);
    lunarInfo.dizhi = lunarInfo.shengxiao = (short) ((lunarInfo.lunarYear - 4) % 12 + 1);
    lunarInfo.isDXYue = daysOfLunarMonth[index] == 30;//daysOfLunarMonth[index]为当前农历月的天数
    return lunarInfo;
}

JieqiInfo p_getJieqiInfo(int year, int month, int day) {
    int jieqiDate[2] = {0}, jieqiOrdinal[2] = {0};
    int index = (2 * month + 21) % 24;
    int curOrdinal;
    JieqiInfo jieqiInfo;

    p_getJieqiData(year, month, jieqiDate, jieqiOrdinal);
    jieqiInfo.jieqi = (short) (day < jieqiDate[0] ? (index + 22) % 24 + 1 : (day >= jieqiDate[1] ? index + 1 : index));
    jieqiInfo.isJieqiToday = (day == jieqiDate[0] || day == jieqiDate[1]);
    jieqiInfo.nextJieqi = (short) ((jieqiInfo.jieqi % 24) + 1);

    if (day < jieqiDate[0]) {
        jieqiInfo.nextJieqiRemainDays = (short) (jieqiDate[0] - day);
    } else if (jieqiDate[0] <= day && day < jieqiDate[1]) {
        jieqiInfo.nextJieqiRemainDays = (short) (jieqiDate[1] - day);
    } else {
        curOrdinal = p_getDayOrdinal(year, month, day);
        month = ((jieqiInfo.nextJieqi + 1) / 2) % 12 + 1;//将下一个节气的序号转换到月份
        if (month == 1) {
            if (year + 1 < END_YEAR) {
                p_getJieqiData(year + 1, month, jieqiDate, jieqiOrdinal);
                jieqiInfo.nextJieqiRemainDays = (short) (31 - day + jieqiDate[0]);
            } else {
                jieqiInfo.nextJieqiRemainDays = 0x3f3f;
            }
        } else {
            p_getJieqiData(year, month, jieqiDate, jieqiOrdinal);
            jieqiInfo.nextJieqiRemainDays = (short) (jieqiOrdinal[0] - curOrdinal);
        }
    }

    return jieqiInfo;
}

Calendar p_getCalendarInfo(int year, int month, int day) {
    Calendar calendar;
    calendar.solar = p_getSolarInfo(year, month, day);
    calendar.lunar = p_getLunarInfo(year, month, day);
    calendar.jieqi = p_getJieqiInfo(year, month, day);
    return calendar;
}

//----------------------------------------------------------------------------------------------------------------------

bool isLeapYear(int year) {
    if (year < 1600 || year > 6399) {
        return false;
    }
    return (year % 100 != 0 && year % 4 == 0) || year % 400 == 0;
}

bool checkDate(int year, int month, int day) {
    return p_checkDate(year, month, day, true);
}

bool getDayOrdinal(int year, int month, int day, int *ordinal) {
    if (ordinal) {
        if (p_checkDate(year, month, day, true)) {
            *ordinal = p_getDayOrdinal(year, month, day);
            return true;
        } else {
            *ordinal = 0x3f3f3f3f;
        }
    }

    return false;
}

bool getDateFromOrdinal(int year, int ordinal, int *month, int *day) {
    if (month && day) {
        bool leap = p_isLeapYear(year);//是否是闰年
        if (year < 1600 || year > 6399 || (leap && ordinal > 366) || (!leap && ordinal > 365)) {
            *month = *day = 0x3f3f3f3f;
            return false;
        }
        p_getDateFromOrdinal(year, ordinal, month, day);
    }
    return true;
}

bool getDaysOfMonth(int year, int month, int *days) {
    if (days) {
        if (p_checkDate(year, month, 1, true)) {
            *days = p_getDaysOfMonth(year, month);
            return true;
        } else {
            *days = 0x3f3f3f3f;
        }
    }
    return false;
}

bool getDayOfWeek(int year, int month, int day, int *week) {
    if (week) {
        if (p_checkDate(year, month, day, true)) {
            *week = p_getDayOfWeek(year, month, day);
            return true;
        } else {
            *week = 0x3f3f3f3f;
        }
    }
    return false;
}

bool getSolarInfo(int year, int month, int day, SolarInfo *solarInfo) {
    if (solarInfo) {
        if (p_checkDate(year, month, day, false)) {
            *solarInfo = p_getSolarInfo(year, month, day);
            return true;
        } else {
            p_memset(solarInfo, 0x3f, sizeof(SolarInfo));
        }
    }
    return false;
}

bool getLunarInfo(int year, int month, int day, LunarInfo *lunarInfo) {
    if (lunarInfo) {
        if (p_checkDate(year, month, day, false)) {
            *lunarInfo = p_getLunarInfo(year, month, day);
            return true;
        } else {
            p_memset(lunarInfo, 0x3f, sizeof(LunarInfo));
        }
    }
    return false;
}

bool getJieqiInfo(int year, int month, int day, JieqiInfo *jieqiInfo) {
    if (jieqiInfo) {
        if (p_checkDate(year, month, day, false)) {
            *jieqiInfo = p_getJieqiInfo(year, month, day);
            return true;
        } else {
            p_memset(jieqiInfo, 0x3f, sizeof(JieqiInfo));
        }
    }
    return false;
}

bool getCalendarInfo(int year, int month, int day, Calendar *calendar) {
    if (calendar) {
        if (p_checkDate(year, month, day, false)) {
            *calendar = p_getCalendarInfo(year, month, day);
            return true;
        } else {
            p_memset(calendar, 0x3f, sizeof(Calendar));
        }
    }
    return false;
}
