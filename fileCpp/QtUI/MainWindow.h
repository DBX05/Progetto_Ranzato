// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDate>
#include <QStackedWidget>
#include "../dataora/dataora.h"
#include "../impegno/impegno.h"
#include "DayWeekView.h"

class EventModel;
class DayWeekView;
class QLineEdit;
class QDateEdit;
class QTimeEdit;
class QComboBox;
class QTextEdit;
class QSpinBox;

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
    void onCalendarDateChanged();
    void onDaySelectedFromView(const QDate& date);
    void onCreateEventSubmit();
    void onDeleteEventSubmit();

private:
    void setupFileMenu();
    void setupStackedPages();
    void showMainPage();
    void showCreateEventPage();
    void showDeleteEventPage();
    void updateViewForCurrentMonth();

    QString sqlDate(const dateTime& dt) const;
    QString sqlTime(const orario& o) const;

    bool insertEvento(const std::shared_ptr<evento>& baseEv);
    bool insertEventoFull(const std::shared_ptr<eventoLungo>& el,
                          const QString& eventType,
                          int priority,
                          const QString& description);
    void loadEventsFromDb();
    void loadEventsForDate(const QDate& date);

    void on_calendarWidget_currentPageChanged(int year, int month);
    void updateCalendarLabel(const QDate& date);
    void updateLabelEvents(const QDate& date);

    bool monthHasEvents(const QDate& date);

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

    QStackedWidget* m_stack = nullptr;
    QWidget* m_mainPage = nullptr;
    QWidget* m_createPage = nullptr;
    QWidget* m_deletePage = nullptr;

    QLineEdit* m_newEventNameEdit = nullptr;
    QDateEdit* m_newEventDateEdit = nullptr;
    QTimeEdit* m_newEventStartEdit = nullptr;
    QTimeEdit* m_newEventEndEdit = nullptr;
    QComboBox* m_newEventTypeCombo = nullptr;
    QTextEdit* m_newEventDescEdit = nullptr;
    QSpinBox*  m_newEventPriorityEdit = nullptr;

    QLineEdit* m_deleteEventNameEdit = nullptr;
    QDateEdit* m_deleteEventDateEdit = nullptr;
};

#endif
