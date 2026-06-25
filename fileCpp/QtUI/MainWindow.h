#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDate>
#include "../dataora/dataora.h"
#include "../impegno/impegno.h"
#include "DayWeekView.h"

class EventModel;
class DayWeekView;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QSqlDatabase db, int userId,
               const QString& name,
               const QString& email,
               QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onImportJson();
    void onNewEvent();
    void onModeChanged(int index);
    void onCalendarDateChanged();
    void onDaySelectedFromView(const QDate& date);

private:
    QString sqlDate(const dateTime& dt) const;
    QString sqlTime(const orario& o) const;

    bool insertEvento(const std::shared_ptr<evento>& baseEv);
    void loadEventsFromDb();
    void loadEventsForDate(const QDate& date);

    void on_calendarWidget_currentPageChanged(int year, int month);
    void updateCalendarLabel(const QDate& date);
    void updateCalendarLabelMode(DayWeekView::Mode mode, const QDate& date);
    void updateCalendarLabelColor(const QDate& date);

    bool monthHasEvents(const QDate& date);

    void updateEventsForCurrentMode();
    void updateLabelEvents(const QDate& date);

    void updateModeFeedback();

    void updateAllLabelsForCurrentMode();

    void onLabelCalendarClicked();

    void onDeleteEventClicked();

    void onFilterTypeChanged(const QString& type);

    void loadEventsForVisibleRange();

    Ui::MainWindow* ui;

    QSqlDatabase m_db;
    int m_userId;
    QString m_userName;
    QString m_userEmail;

    EventModel* m_eventModel;
    DayWeekView* m_dayWeekView;
};

#endif
