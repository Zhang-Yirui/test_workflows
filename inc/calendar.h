#ifndef C_CALENDAR_CALENDAR_H
#define C_CALENDAR_CALENDAR_H



#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

/**
* @brief 公历日期信息结构体
*/
typedef struct SolarInfo {
    short year;/*!< 公历年份 */
    short month;/*!< 公历月份 */
    short day;/*!< 公历日期 */
    short daysOfYear;/*!< 该年的天数 */
    short daysOfMonth;/*!< 该月的天数 */
    short dayOfYear;/*!< 距今年元旦的天数 */
    short weekOfYear;/*!< 一年中的第几个星期 */
    short week;/*!< 星期 */
} SolarInfo;

/**
 * @brief 农历日期信息结构体
 */
typedef struct LunarInfo {
    short lunarYear;/*!< 农历年 */
    short lunarMonth;/*!< 农历月 */
    short lunarDay;/*!< 农历日 */
    short tiangan;/*!< 天干 */
    short dizhi;/*!< 地支 */
    short shengxiao;/*!< 生肖 */
    bool isLeapMonth;/*!< 闰月情况 */
    bool isDXYue;/*!< 大小月情况 */
} LunarInfo;

/**
 * @brief 节气信息结构体
 */
typedef struct JieqiInfo {
    short jieqi;/*!< 当前节气 */
    short nextJieqi;/*!< 下个节气 */
    short nextJieqiRemainDays;/*!< 到下个节气剩余天数 */
    bool isJieqiToday;/*!< 今天是否为节气开始的第一天 */
} JieqiInfo;

/**
 * @brief 公历信息, 农历信息和节气信息结构体
 */
typedef struct Calendar {
    SolarInfo solar;/*!< 公历信息 */
    LunarInfo lunar;/*!< 农历信息 */
    JieqiInfo jieqi;/*!< 节气信息 */
} Calendar;

/**
 * @name checkDate
 * @brief 判断公历日期是否合法
 * @param[in] year 年份
 * @param[in] month 月份
 * @param[in] day 日期
 * @return 如果日期合法, 返回 true, 否则返回 false
 * @note 注意:
 *       日期非法的因有: 1. 年份超出范围 [1600, 6399]; 2. 月份超出范围 [1, 12]; 3.日期超过了当前月份最大日期或小于 1.
 */
bool checkDate(int year, int month, int day);

/**
 * @name isLeapYear
 * @brief 判断年份是否为闰年
 * @param[in] year 要判断的年份
 * @return 如果年份是闰年, 返回 true, 是平年返回 false
 * @note 注意:
 *       如果年份超出范围 [1600, 6399], 也会返回 false.
 */
bool isLeapYear(int year);

/**
 * @name getDayOrdinal
 * @brief 获取指定日期的年内序数
 * @param[in] year 年份
 * @param[in] month 月份
 * @param[in] day 日期
 * @param[out] ordinal 返回值, 年内序数
 * @return 获取成功返回true, 否则返回false
 * @warning ordinal 不能为空.
 * @note 注意:
 *       如果获取失败, 则会修改 ordinal 的值为 0x3f3f3f3f.
 *       获取失败的原因有: 参数错误或 ordinal 为空.
 *       例如: 年份超出范围 [1600, 6399],
 *       又例如: 月份超出范围 [1, 12],
 *       又例如: 输入的日期超过了当前月份最大日期或小于 1.
 */
bool getDayOrdinal(int year, int month, int day, int *ordinal);

/**
 * @name getDateFromOrdinal
 * @brief 从日序号获取日期
 * @param[in] year 年份
 * @param[in] ordinal 年内序数
 * @param[out] month 返回值, 月份
 * @param[out] day 返回值, 日期
 * @return 获取成功返回 true, 否则返回 false
 * @warning month 或 day 不能为空.
 * @note 注意:
 *       如果获取失败, 则会修改 month 和 day 的值为 0x3f3f3f3f.
 *       获取失败的因有: month 或 day 为空, 参数错误.
 *       例如: 年份超出范围 [1600, 6399],
 *       又例如: 年内序数大于当前年份最大年内序数或小于 0,
 *       又例如: 输入的日期大于当前月份最大日期或小于 1.
 */
bool getDateFromOrdinal(int year, int ordinal, int *month, int *day);

/**
 * @name getOrdinalFromDate
 * @brief 获取指定月份的天数
 * @param[in] year 年份
 * @param[in] month 月份
 * @param[out] days 返回值, 该月份的天数
 * @return 获取成功返回 true, 否则返回 false
 * @warning
 *       days 不能为空.
 * @note 注意:
 *       如果获取失败, 则会修改 days 的值为 0x3f3f3f3f.
 *       获取失败的原因有: 参数错误或 days 为空.
 *       例如: 年份超出范围 [1600, 6399],
 *       又例如: 月份超出范围 [1, 12].
 */
bool getDaysOfMonth(int year, int month, int *days);

/**
 * @name getDayOfWeek
 * @brief 获取指定日期是星期几
 * @param[in] year 年份
 * @param[in] month 月份
 * @param[in] day 日期
 * @param[out] week 返回值, 星期几 (0-7, 0表示星期日, 1表示星期一, 依此类推)
 * @return 获取成功返回 true, 否则返回 false
 * @warning
 *       week 不能为空.
 * @note 注意:
 *       如果获取失败, 则会修改 week 的值为 0x3f3f3f3f.
 *       获取失败的原因可能是: 参数错误或 week 为空.
 *       例如: 年份超出范围 [1600, 6399],
 *       又例如: 月份超出范围 [1, 12],
 *       又例如: 输入的日期超过了当前月份最大日期或小于 1.
 */
bool getDayOfWeek(int year, int month, int day, int *week);

/**
 * @name getSolarInfo
 * @brief 获取公历信息
 * @param[in] year 年份
 * @param[in] month 月份
 * @param[in] day 日期
 * @param solarInfo 返回值, 公历信息结构体
 * @return 获取成功返回 true, 否则返回 false
 * @warning
 *       solarInfo 不能为空.
 * @note 注意:
 *       如果获取失败, 则会用 0x3f3f 填充 solarInfo 中成员的值.
 *       获取失败的原因可能是: 参数错误或 solarInfo 为空.
 *       例如: 年份超出范围 [1600-6399],
 *       又例如: 月份超出范围 [1-12],
 *       又例如: 输入的日期超过了当前月份最大日期或小于 1.
 */
bool getSolarInfo(int year, int month, int day, SolarInfo *solarInfo);

/**
 * @name getLunarInfo
 * @brief 获取农历信息
 * @param[in] year 年份
 * @param[in] month 月份
 * @param[in] day 日期
 * @param[out] lunarInfo 返回值, 农历信息结构体
 * @return 获取成功返回 true, 否则返回 false
 * @warning
 *       lunarInfo 不能为空.
 * @note 注意:
 *       如果获取失败, 则会用 0x3f3f 填充 lunarInfo 中成员的值.
 *       获取失败的原因可能是: 参数错误或 lunarInfo 为空.
 *       例如: 年份超出范围 [1600, 6399],
 *       又例如: 月份超出范围 [1, 12],
 *       又例如: 输入的日期超过了当前月份最大日期或小于 1.
 */
bool getLunarInfo(int year, int month, int day, LunarInfo *lunarInfo);

/**
 * @name getJieqiInfo
 * @brief 获取公历信息
 * @param[in] year 年份
 * @param[in] month 月份
 * @param[in] day 日期
 * @param[out] jieqiInfo 返回值, 节气信息结构体
 * @return 获取成功返回 true, 否则返回 false
 * @warning
 *       jieqiInfo 不能为空.
 * @note 注意:
 *       如果获取失败, 则会用 0x3f3f 填充 jieqiInfo 中成员的值.
 *       获取失败的原因可能是: 参数错误或 jieqiInfo 为空.
 *       例如: 年份超出范围 [1600, 6399],
 *       又例如: 月份超出范围 [1, 12],
 *       又例如: 输入的日期超过了当前月份最大日期或小于 1.
 */
bool getJieqiInfo(int year, int month, int day, JieqiInfo *jieqiInfo);

/**
 * @name getCalendarInfo
 * @brief 获取公历信息
 * @param[in] year 年份
 * @param[in] month 月份
 * @param[in] day 日期
 * @param[out] calendar 返回值, 日历信息结构体
 * @return 获取成功返回 true, 否则返回 false
 * @warning
 *       calendar 不能为空.
 * @note 注意:
 *       如果获取失败, 则会用 0x3f3f 填充 calendar 中成员的值.
 *       获取失败的原因可能是: 参数错误或 calendar 为空.
 *       例如: 年份超出范围 [1600, 6399],
 *       又例如: 月份超出范围 [1, 12],
 *       又例如: 输入的日期超过了当前月份最大日期或小于 1.
 */
bool getCalendarInfo(int year, int month, int day, Calendar *calendar);

#ifdef __cplusplus
};
#endif

#endif//C_CALENDAR_CALENDAR_H
