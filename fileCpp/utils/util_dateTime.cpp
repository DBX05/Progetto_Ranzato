// util_datetime.cpp
#include "util_datetime.h"

QString dateTimeToQString(const dateTime& dt) {
    QString s = QString::fromStdString(dt.getDateTime());
    if (s.size() < 19)
        return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    return s.mid(0, 19);
}

dateTime qStringToDateTime(const QString& s)
{
    QDateTime qdt = QDateTime::fromString(s, "yyyy-MM-dd HH:mm:ss");
    if (!qdt.isValid())
        qdt = QDateTime::currentDateTime();

    int year  = qdt.date().year();
    int month = qdt.date().month() - 1;      // 0–11
    int day   = qdt.date().day();            // 1–31
    int dow   = qdt.date().dayOfWeek() - 1;  // 0–6

    // clamp mese
    if (month < 0) month = 0;
    if (month > 11) month = 11;

    // clamp giorno del mese
    if (day < 1) day = 1;
    if (day > 31) day = 31;

    // clamp giorno della settimana
    if (dow < 0) dow = 0;
    if (dow > 6) dow = 6;

    return dateTime(year, month, day, qdt.time().hour(), qdt.time().minute(), qdt.time().second());
}



dateTime chronoToDateTime(const std::chrono::system_clock::time_point& tp)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm* tm = std::localtime(&tt);

    return dateTime(
        tm->tm_year,
        tm->tm_mon,
        tm->tm_mday,
        tm->tm_hour,
        tm->tm_min,
        tm->tm_sec
    );
}

orario orarioFromOptional(const std::optional<std::string>& opt)
{
    if (!opt.has_value())
        return orario(0,0,0);

    const std::string& s = *opt;

    if (s.size() < 8)
        return orario(0,0,0);

    int h = std::stoi(s.substr(0,2));
    int m = std::stoi(s.substr(3,2));
    int sec = std::stoi(s.substr(6,2));

    return orario(h, m, sec);
}
