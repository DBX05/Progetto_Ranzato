#ifndef NEWEVENTDIALOG_H
#define NEWEVENTDIALOG_H

#include <QDialog>
#include <memory>
#include <fileCpp/dataora/dataora.h>
#include <fileCpp/impegno/impegno.h>

class QLineEdit;
class QTimeEdit;
class QDialogButtonBox;
class QDateEdit;
class QComboBox;

class NewEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewEventDialog(QWidget* parent = nullptr);

    // Crea un evento (eventoLungo) con i dati inseriti
    std::shared_ptr<evento> createEvento() const;

    // Accessori per mostrare i dettagli dopo la creazione
    QString summary() const;

private:
    QLineEdit* m_nameEdit;
    QTimeEdit* m_startEdit;
    QTimeEdit* m_endEdit;
    QDateEdit* m_dateEdit;
    QComboBox* m_typeCombo;
    QDialogButtonBox* m_buttons;
};

#endif // NEWEVENTDIALOG_H
