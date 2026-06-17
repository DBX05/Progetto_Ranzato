// utils_datetime.h
#ifndef UTILS_DATETIME_H
#define UTILS_DATETIME_H

#include <QString>
#include <QDateTime>
#include "fileCpp/dataora/dataora.h"

// Converte dateTime -> "YYYY-MM-DD HH:MM:SS"
QString dateTimeToQString(const dateTime& dt);

// Converte "YYYY-MM-DD HH:MM:SS" -> dateTime
dateTime qStringToDateTime(const QString& s);

#endif // UTILS_DATETIME_H
