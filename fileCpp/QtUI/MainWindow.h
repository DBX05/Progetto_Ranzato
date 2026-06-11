#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QListView>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtGui/QAction>
#include <QtCore/QDate>
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
