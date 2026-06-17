#include "util_dateTime.h"

QString dateTimeToQString(const dateTime& dt) {
    // Prova a ottenere la stringa direttamente dalla classe domain
    QString s = QString::fromStdString(dt.getDateTime());
    // Se la stringa non è nel formato atteso, usa QDateTime come fallback
    if (s.size() < 19) {
        return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    }
    return s.mid(0,19);
}

dateTime qStringToDateTime(const QString& s) {
    // s expected "YYYY-MM-DD HH:MM:SS"
    QDateTime qdt = QDateTime::fromString(s.trimmed(), "yyyy-MM-dd HH:mm:ss");
    if (!qdt.isValid()) {
        qdt = QDateTime::currentDateTime();
    }
    int an = qdt.date().year();
    int mo = qdt.date().month();
    int day = qdt.date().day();
    int hr = qdt.time().hour();
    int mn = qdt.time().minute();
    int sc = qdt.time().second();

    // Adatta la chiamata al costruttore della tua classe dateTime
    // Se la tua classe usa mese 0-based o ordine diverso, modifica qui
    return dateTime(an, mo, day, hr, mn, sc);
}
