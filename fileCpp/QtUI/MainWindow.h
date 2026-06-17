#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QListView>
#include <QtWidgets/QComboBox>
#include <QtCore/QDate>
#include <QSqlDatabase>
#include <memory>
#include "./impegno/impegno.h"

class EventModel;
class DayWeekView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // ora riceve DB e info utente
    explicit MainWindow(QSqlDatabase db, int userId, const QString& userName, const QString& userEmail, QWidget* parent = nullptr);

private slots:
    void onAddEvent();
    void onCalendarSelectionChanged(const QDate& date);
    void onModeChanged(int index);

private:
    QCalendarWidget* m_calendar;
    QListView* m_listView;
    QComboBox* m_modeCombo;
    DayWeekView* m_dayWeekView;
    EventModel* m_model;

    QSqlDatabase m_db;
    int m_userId;
    QString m_userName;
    QString m_userEmail;

    void populateSampleData();
    void refreshListForDate(const QDate& date);

    // DB helpers
    void loadEventsFromDb();
    void saveEventToDb(const std::shared_ptr<eventoLungo>& ev);
};

#endif // MAINWINDOW_H
