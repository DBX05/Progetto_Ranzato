#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QListView>
#include <QComboBox>
#include <memory>

class EventModel;
class DayWeekView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

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

    void populateSampleData();
    void refreshListForDate(const QDate& date);
};

#endif // MAINWINDOW_H
