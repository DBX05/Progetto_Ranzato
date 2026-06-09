#ifndef NEWEVENTDIALOG_H
#define NEWEVENTDIALOG_H

#include <QDialog>
#include <memory>
#include <fileCpp/dataora/dataora.h>
#include <fileCpp/impegno/impegno.h>

class QLineEdit;
class QTimeEdit;
class QDialogButtonBox;

class NewEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewEventDialog(QWidget* parent = nullptr);

    // Create a new evento shared_ptr using dialog inputs
    std::shared_ptr<evento> createEvento() const;

private:
    QLineEdit* m_nameEdit;
    QTimeEdit* m_startEdit;
    QTimeEdit* m_endEdit;
    QDialogButtonBox* m_buttons;
};

#endif // NEWEVENTDIALOG_H
