// util_datetime.cpp
#include "util_datetime.h"

QString dateTimeToQString(const dateTime& dt) {
    QString s = QString::fromStdString(dt.getDateTime());
    if (s.size() < 19)
        return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    return s.mid(0, 19);
}

dateTime qStringToDateTime(const QString& s) {
    QDateTime qdt = QDateTime::fromString(s.trimmed(), "yyyy-MM-dd HH:mm:ss");
    if (!qdt.isValid())
        qdt = QDateTime::currentDateTime();

    int an  = qdt.date().year();
    int mo  = qdt.date().month() - 1;
    int day = qdt.date().day();
    int hr  = qdt.time().hour();
    int mn  = qdt.time().minute();
    int sc  = qdt.time().second();

    return dateTime(an, mo, day, hr, mn, sc);
}

dateTime chronoToDateTime(const std::chrono::system_clock::time_point& tp)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm* tm = std::localtime(&tt);

    return dateTime(
        tm->tm_year + 1900,
        tm->tm_mon + 1,
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
