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
    int mo  = qdt.date().month();
    int day = qdt.date().day();
    int hr  = qdt.time().hour();
    int mn  = qdt.time().minute();
    int sc  = qdt.time().second();

    return dateTime(an, mo, day, hr, mn, sc);
}
