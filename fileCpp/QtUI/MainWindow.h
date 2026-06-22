#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDate>
#include <memory>

#include "fileCpp/impegno/impegno.h"

class EventModel;
class DayWeekView;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QSqlDatabase db,
                        int userId,
                        const QString& name,
                        const QString& email,
                        QWidget* parent = nullptr);

private slots:
    void onImportJson();

private:
    QString sqlDate(const dateTime& dt) const;
    QString sqlTime(const orario& o) const;

    bool insertEvento(const std::shared_ptr<evento>& ev);
    void loadEventsForDate(const QDate& date);
    void loadEventsFromDb();

    QSqlDatabase m_db;
    int m_userId;
    QString m_userName;
    QString m_userEmail;

    EventModel*  m_eventModel = nullptr;
    DayWeekView* m_dayWeekView = nullptr;

    QAction* m_importJsonAction = nullptr;
};

int ensureUserExists(QSqlDatabase& db, const QString& name, const QString& email, const QString& password);

#endif // MAINWINDOW_H
