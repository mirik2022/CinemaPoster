#ifndef BOOKINGDIALOG_H
#define BOOKINGDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QScrollArea>
#include "moviedatabase.h"

class BookingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookingDialog(int sessionId, const QString& username, QWidget *parent = nullptr);

signals:
    void bookingCompleted();

private slots:
    void onBookClicked();
    void onSeatClicked(int seatNumber);
    void refreshSeatMap();
    void checkAndUpdateSeats(); // ДОБАВЬТЕ ЭТУ СТРОКУ

private:
    void updateSeatButton(int seatNumber);
    int sessionId;
    QString username;
    CinemaSession session;
    Movie movie;

    QList<int> selectedSeats;
    QLabel *totalLabel;
    QPushButton *bookButton;
    QPushButton *refreshBtn;
    QWidget *seatContainer;

    void setupUI();
    void createSeatLayout();
};

#endif // BOOKINGDIALOG_H
