// util_datetime.cpp
#include "util_datetime.h"
#include <QDateTime>
#include <QDebug>
#include <chrono>
#include <ctime> // For std::localtime, std::time_t
#include <string> // For std::stoi, std::string
#include <optional> // For std::optional

/**
 * @brief Converts an internal dateTime object to a QString in ISO format (YYYY-MM-DD HH:mm:ss).
 *
 * @param dt The dateTime object to convert.
 * @return QString The formatted date-time string. Returns current date-time if input is invalid.
 */
QString dateTimeToQString(const dateTime& dt) {
    QString s = QString::fromStdString(dt.getDateTime());
    // Ensure the string is long enough for the expected format. If not, fallback.
    if (s.size() < 19) {
        qWarning() << "[dateTimeToQString] Input dateTime string too short, falling back to current time.";
        return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    }
    return s.mid(0, 19); // Return the first 19 characters (YYYY-MM-DD HH:mm:ss)
}

/**
 * @brief Converts a QString (expected in "yyyy-MM-dd HH:mm:ss" or ISO format) to an internal dateTime object.
 *
 * @param s The QString date-time string.
 * @return dateTime The converted dateTime object. Returns current date-time if parsing fails.
 */
dateTime qStringToDateTime(const QString& s)
{
    // Attempt to parse using standard Qt ISO format first
    QDateTime qdt = QDateTime::fromString(s, Qt::ISODate);
    if (!qdt.isValid()) {
        // If ISO parsing fails, try the specific "yyyy-MM-dd HH:mm:ss" format
        qdt = QDateTime::fromString(s, "yyyy-MM-dd HH:mm:ss");
    }

    // If parsing still fails, fallback to the current date and time
    if (!qdt.isValid()) {
        qWarning() << "[qStringToDateTime] Failed to parse string:" << s << ". Falling back to current date/time.";
        qdt = QDateTime::currentDateTime();
    }

    // Extract components for the internal dateTime object
    int year  = qdt.date().year();
    int month = qdt.date().month() - 1;      // Internal dateTime uses 0-11 for months
    int day   = qdt.date().day();            // Day of the month (1-31)
    int dow   = qdt.date().dayOfWeek() - 1;  // Internal dateTime expects day-of-week in 0-6

    // Clamp values to ensure they are within valid ranges for the dateTime constructor,
    // though QDateTime should already provide valid components. This adds robustness.
    if (month < 0) month = 0;
    if (month > 11) month = 11;
    if (day < 1) day = 1;
    if (day > 31) day = 31; // Basic clamp, doesn't account for days in month
    if (dow < 0) dow = 0;
    if (dow > 6) dow = 6;

    // Construct the internal dateTime object with the correct argument order:
    // year, month, day-of-month, day-of-week, hour, minute, second
    return dateTime(year, month, day, dow, qdt.time().hour(), qdt.time().minute(), qdt.time().second());
}

/**
 * @brief Converts a std::chrono::system_clock::time_point to an internal dateTime object.
 *
 * @param tp The time_point to convert.
 * @return dateTime The converted dateTime object.
 */
dateTime chronoToDateTime(const std::chrono::system_clock::time_point& tp)
{
    // Convert time_point to time_t
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    // Convert time_t to a local time structure (tm)
    std::tm* tm = std::localtime(&tt);

    // Check if tm is null (can happen on error)
    if (!tm) {
        qWarning() << "[chronoToDateTime] Failed to convert time_point to local time.";
        return dateTime(); // Return default (likely current time)
    }

    // Construct the internal dateTime object
    // tm_year is years since 1900
    // tm_mon is months since January (0-11)
    // tm_mday is day of the month (1-31)
    return dateTime(
        tm->tm_year + 1900, // Adjust year
        tm->tm_mon,         // Month is already 0-11
        tm->tm_mday,        // Day of the month
        tm->tm_wday,        // Day of week (0-6)
        tm->tm_hour,        // Hour
        tm->tm_min,         // Minute
        tm->tm_sec          // Second
    );
}

/**
 * @brief Converts an optional string representing time (e.g., "HH:MM:SS") to an orario object.
 *
 * @param opt An optional string containing the time.
 * @return orario The parsed orario object, or {0,0,0} if input is invalid or missing.
 */
orario orarioFromOptional(const std::optional<std::string>& opt)
{
    // If the optional is not present, return a default orario (00:00:00)
    if (!opt.has_value()) {
        return orario(0,0,0);
    }

    const std::string& s = *opt; // Get the string value

    // Basic validation for expected format HH:MM:SS (length 8)
    if (s.length() < 8 || s[2] != ':' || s[5] != ':') {
        qWarning() << "[orarioFromOptional] Invalid time format string:" << QString::fromStdString(s);
        return orario(0,0,0); // Return default on format error
    }

    try {
        // Extract hour, minute, second using substr and stoi
        int h = std::stoi(s.substr(0, 2));
        int m = std::stoi(s.substr(3, 2));
        int sec = std::stoi(s.substr(6, 2));

        // Basic sanity check on parsed values (optional, depends on expected input range)
        if (h < 0 || h > 23 || m < 0 || m > 59 || sec < 0 || sec > 59) {
             qWarning() << "[orarioFromOptional] Parsed time values out of range.";
             return orario(0,0,0);
        }

        return orario(h, m, sec);
    } catch (const std::invalid_argument& e) {
        qWarning() << "[orarioFromOptional] stoi argument error for time string:" << QString::fromStdString(s) << e.what();
        return orario(0,0,0);
    } catch (const std::out_of_range& e) {
        qWarning() << "[orarioFromOptional] stoi out_of_range error for time string:" << QString::fromStdString(s) << e.what();
        return orario(0,0,0);
    }
}
