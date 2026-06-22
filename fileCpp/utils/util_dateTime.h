// util_datetime.h
#ifndef UTILS_DATETIME_H
#define UTILS_DATETIME_H

#include <QString>
#include <QDateTime>
#include "fileCpp/dataora/dataora.h"

QString dateTimeToQString(const dateTime& dt);
dateTime qStringToDateTime(const QString& s);

#endif // UTILS_DATETIME_H
